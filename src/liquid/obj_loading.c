#include "obj_loading.h"
#include "common.h"
#include "maths.h"

#define OBJMODEL_INITIAL_VERTEX_CAPACITY 10000
#define OBJMODEL_INITIAL_INDEX_CAPACITY 10000

#define IS_DIGIT(x) ((u32)((x) - '0') < (u32)(10))
#define IS_WHITESPACE(x) (((x) == ' ') || ((x) == '\n') || ((x) == '\t') || ((x) == '\r'))

typedef union
{
	struct { u32 pos, uv, normal; };
	u32 arr[3];
} OBJIndex;

typedef struct
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
} Vertex;

typedef struct
{
	vec3 *positions;
	vec2 *uvs;
	vec3 *normals;

	OBJIndex *indices;
	u32 *num_indices_in_face;

	u32 num_indices;
	u32 num_faces;

	bool has_uvs;
	bool has_normals;
} RawOBJData;

typedef struct
{
	Vertex *vertices;
	u32 *indices;
	u32 num_vertices;
	u32 num_indices;
} IndexedModel;

static char *get_file_contents(const char *path)
{
	FILE *f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	u64 size = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *text = malloc(size + 1);
	fread(text, size, 1, f);
	fclose(f);
	text[size] = 0;
	return text;
}

static char *advance_to_space(char *ptr)
{
	while (*++ptr != ' ') {}
	return ptr;
}

static char *parse_vertex(char *line_start, u32 tag_len, u32 num_comps, f32 *dest)
{
	char *c = line_start + tag_len;
	while (!IS_DIGIT(*++c) && *c != '-') {}
	for (u32 i = 0; i < num_comps; i++) {
		char *space_loc = advance_to_space(c);
		*space_loc = 0;
		dest[i] = atof(c);
		*space_loc = ' ';
		c = space_loc + 1;
	}
	return c;
}

static u32 parse_face(char *line_start, OBJIndex *face_start, size_t num_indices, size_t num_positions, size_t num_uvs, size_t num_normals)
{
	char *index_start = line_start + 2; // Skip "f"
	u32 num_indices_in_face = 0;

	while (*(index_start - 1) != '\n' && *(index_start - 1) != 0) {
		OBJIndex *target = face_start + num_indices_in_face;

		char *cut = index_start;
		while (IS_DIGIT(*++cut)) {}
		char temp = *cut;
		*cut = 0;
		i32 i = atoi(index_start);
		*cut = temp;

		if (i < 0) {
			target->pos = (u32) ((i32) num_positions + i);
		} else {
			target->pos = (u32) i - 1;
		}

		if (temp == '/') {
			index_start = ++cut;
			if (*index_start == '/') {
				cut = ++index_start;
				while (IS_DIGIT(*++cut)) {}
				temp = *cut;
				*cut = 0;
				i = atoi(index_start);
				*cut = temp;
				target->normal = i < 0 ? (u32) ((i32) num_normals + i) : (u32) i - 1;
			} else {
				while (IS_DIGIT(*++cut)) {}
				temp = *cut;
				*cut = 0;
				i = atoi(index_start);
				*cut = temp;
				target->uv = i < 0 ? (u32) ((i32) num_uvs + i) : (u32) i - 1;

				if (*cut == '/') {
					index_start = ++cut;
					while (IS_DIGIT(*++cut)) {}
					temp = *cut;
					*cut = 0;
					i = atoi(index_start);
					*cut = temp;
					target->normal = i < 0 ? (u32) ((i32) num_normals + i) : (u32) i - 1;
				}
			}
		}

		index_start = cut;
		while (*++index_start && IS_WHITESPACE(*index_start)) {}

		num_indices_in_face++;
	}

	return num_indices_in_face;
}

static RawOBJData parse_obj(char *text)
{
	RawOBJData result;

	result.has_normals = false;
	result.has_uvs = false;

	size_t vertex_capacity = OBJMODEL_INITIAL_VERTEX_CAPACITY;
	size_t index_capacity = OBJMODEL_INITIAL_INDEX_CAPACITY;
	size_t face_capacity = OBJMODEL_INITIAL_VERTEX_CAPACITY / 3;

	result.positions = malloc(sizeof(vec3) * vertex_capacity);
	result.uvs = malloc(sizeof(vec2) * vertex_capacity);
	result.normals = malloc(sizeof(vec3) * vertex_capacity);
	result.indices = malloc(sizeof(OBJIndex) * index_capacity);
	result.num_indices_in_face = malloc(sizeof(u32) * face_capacity);

	u32 num_positions = 0;
	u32 num_uvs = 0;
	u32 num_normals = 0;
	result.num_indices = 0;
	result.num_faces = 0;

	char *line_start = text;
	while (line_start) {
		if (*line_start == 'v') {

			if (num_positions == vertex_capacity ||
				num_uvs == vertex_capacity ||
				num_normals == vertex_capacity) {
				vertex_capacity *= 2;
				result.positions = realloc(result.positions, vertex_capacity * sizeof(vec3));
				result.uvs = realloc(result.uvs, vertex_capacity * sizeof(vec2));
				result.normals = realloc(result.normals, vertex_capacity * sizeof(vec3));
			}

			u32 tag_len, num_comps;
			f32 *dest;

			if (*(line_start + 1) == 't') {
				result.has_uvs = true;
				tag_len = 2;
				num_comps = 2;
				dest = (f32 *) (result.uvs + num_uvs++);
			} else if (*(line_start + 1) == 'n') {
				result.has_normals = true;
				tag_len = 2;
				num_comps = 3;
				dest = (f32 *) (result.normals + num_normals++);
			} else {
				tag_len = 1;
				num_comps = 3;
				dest = (f32 *) (result.positions + num_positions++);
			}

			parse_vertex(line_start, tag_len, num_comps, dest);
		} else if (*line_start == 'm') {
		} else if (*line_start == 'f') {

			if (result.num_faces == face_capacity) {
				face_capacity *= 2;
				result.num_indices_in_face = (u32 *) realloc((void *) result.num_indices_in_face, face_capacity * sizeof(u32));
			}

			if (result.num_indices + result.num_indices_in_face[result.num_faces] >= index_capacity) {
				index_capacity *= 2;
				result.indices = (OBJIndex *) realloc((void *) result.indices, index_capacity * sizeof(OBJIndex));
			}

			result.num_indices_in_face[result.num_faces] = parse_face(line_start, result.indices + result.num_indices, result.num_indices, num_positions, num_uvs, num_normals);

			result.num_indices += result.num_indices_in_face[result.num_faces];
			result.num_faces++;
		} else if (*line_start == 'l') {
		}

		while (*line_start++ != '\n') {
			if (*line_start == 0) {
				line_start = NULL;
				break;
			}
		}
	}

	return result;
}

static void calc_normals(IndexedModel model)
{
	for (u32 i = 0; i < model.num_vertices; i++) {
		model.vertices[i].normal = vec3_zero();
	}

	// Assumes full triangulation
	for (u32 i = 0; i < model.num_indices / 3; i++) {
		u32 i0 = model.indices[i * 3 + 0];
		u32 i1 = model.indices[i * 3 + 1];
		u32 i2 = model.indices[i * 3+ 2];

		vec3 d1 = vec3_sub(model.vertices[i1].pos, model.vertices[i0].pos);
		vec3 d2 = vec3_sub(model.vertices[i2].pos, model.vertices[i0].pos);
		vec3 normal = vec3_normalized(vec3_cross(d1, d2));

		model.vertices[i0].normal = vec3_add(model.vertices[i0].normal, normal);
		model.vertices[i1].normal = vec3_add(model.vertices[i1].normal, normal);
		model.vertices[i2].normal = vec3_add(model.vertices[i2].normal, normal);
	}

	for (u32 i = 0; i < model.num_vertices; i++) {
		model.vertices[i].normal = vec3_normalized(model.vertices[i].normal);
	}
}

static IndexedModel create_indexed_model(RawOBJData data)
{
	IndexedModel result;
	result.num_vertices = data.num_indices;
	result.num_indices = 0;
	result.vertices = malloc(sizeof(Vertex) * data.num_indices);

	size_t index_capacity = data.num_indices;
	result.indices = malloc(sizeof(u32) * index_capacity);

	size_t offset = 0;
	for (u32 i = 0; i < data.num_faces; i++) {

		if (result.num_indices == index_capacity) {
			index_capacity *= 2;
			result.indices = (u32 *) realloc((void *) result.indices, index_capacity * sizeof(u32));
		}

		for (u32 k = 0; k < data.num_indices_in_face[i]; k++) {
			OBJIndex index = data.indices[offset + k];
			result.vertices[offset + k].pos = data.positions[index.pos];
			if (data.has_uvs) result.vertices[offset + k].uv = data.uvs[index.uv];
			if (data.has_normals) result.vertices[offset + k].normal = data.normals[index.normal];
		}

		u32 i0 = offset + 0;
		u32 i1 = 0;
		u32 i2 = offset + 1;
		size_t n = 0;
		for (u32 j = 2; j < data.num_indices_in_face[i]; j++) {
			i1 = i2;
			i2 = offset + j;

			result.indices[result.num_indices + 3 * n + 0] = i0;
			result.indices[result.num_indices + 3 * n + 1] = i1;
			result.indices[result.num_indices + 3 * n + 2] = i2;

			n++;
		}

		result.num_indices += 3 * n;
		offset += data.num_indices_in_face[i];
	}

	if (!data.has_normals) {
		calc_normals(result);
	}

	return result;
}

static Mesh create_mesh(IndexedModel model)
{
	Mesh result;

	GLuint vbo;

	GL_CALL(glGenVertexArrays, 1, &result.vao);
	GL_CALL(glBindVertexArray, result.vao);

	GL_CALL(glGenBuffers, 1, &vbo);
	GL_CALL(glGenBuffers, 1, &result.ibo);

	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(Vertex) * model.num_vertices, model.vertices, GL_STATIC_DRAW);

	GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, result.ibo);
	GL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * model.num_indices, model.indices, GL_STATIC_DRAW);

	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glEnableVertexAttribArray, 1);
	GL_CALL(glEnableVertexAttribArray, 2);
	GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) sizeof(vec3));
	GL_CALL(glVertexAttribPointer, 2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) sizeof(vec3) + sizeof(vec2));

	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glDeleteBuffers, 1, &vbo);

	result.num_indices = model.num_indices;

	return result;
}

Mesh obj_load_mesh(const char *path)
{
	char *text = get_file_contents(path);

	RawOBJData raw_data = parse_obj(text);
	IndexedModel model = create_indexed_model(raw_data);
	Mesh result = create_mesh(model);

	free(model.vertices);
	free(model.indices);
	free(raw_data.positions);
	free(raw_data.uvs);
	free(raw_data.normals);
	free(raw_data.indices);
	free(raw_data.num_indices_in_face);
	free(text);

	INFO("Loaded mesh: %s", path);

	return result;
}