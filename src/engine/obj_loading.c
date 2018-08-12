#include "obj_loading.h"
#include "common.h"
#include "maths.h"

#define OBJMODEL_INITIAL_VERTEX_CAPACITY 10000
#define OBJMODEL_INITIAL_INDEX_CAPACITY 10000

#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
#define IS_DIGIT(x) ((u32)((x) - '0') < (u32)(10))
#define IS_NEW_LINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))

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
} OBJVertex;

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
	OBJVertex *vertices;
	u32 *indices;
	u32 num_vertices;
	u32 num_indices;
} OBJModel;

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

static char *advance_past_newline(char *ptr)
{
	while (*ptr++ != '\n') {
		if (*ptr == 0) return NULL;
	}
	return ptr;
}

static char *advance_to_space(char *ptr)
{
	while (*++ptr != ' ') {}
	return ptr;
}

static bool is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

static char *extract_vec(char *line_start, u32 tag_len, u32 num_comps, f32 *dest)
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

static u32 parse_obj_face(char *line_start, OBJIndex *face_buffer)
{
	char *index_start = line_start + 2; // Skip "f"
	u32 num_indices_in_face = 0;

	while (*(index_start - 1) != '\n' && *(index_start - 1) != 0) {
		OBJIndex *target = face_buffer + num_indices_in_face;

		char *cut = index_start;
		while (IS_DIGIT(*++cut)) {} // Advance to end of index

		char temp = *cut;
		*cut = 0;
		target->pos = atoi(index_start) - 1;
		*cut = temp;

		if (temp == '/') {
			index_start = ++cut;
			if (*index_start == '/') {
				cut = ++index_start;
				while (IS_DIGIT(*++cut)) {}
				temp = *cut;
				*cut = 0;
				target->normal = atoi(index_start) - 1;
				*cut = temp;
			} else {
				while (IS_DIGIT(*++cut)) {}
				temp = *cut;
				*cut = 0;
				target->uv = atoi(index_start) - 1;
				*cut = temp;

				if (*cut == '/') {
					index_start = ++cut;
					while (IS_DIGIT(*++cut)) {}
					temp = *cut;
					*cut = 0;
					target->normal = atoi(index_start) - 1;
					*cut = temp;
				}
			}
		}

		index_start = cut;
		while (*++index_start && is_whitespace(*index_start)) {}
		num_indices_in_face++;
	}

	return num_indices_in_face;
}

static RawOBJData parse_obj(char *text)
{
	RawOBJData result;

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
		if (*line_start == 'v') { // vertex pos, uv or normal

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

			if (*(line_start + 1) == 't') { // uv
				result.has_uvs = true;
				tag_len = 2;
				num_comps = 2;
				dest = (f32 *) (result.uvs + num_uvs++);
			} else if (*(line_start + 1) == 'n') { // normal
				result.has_normals = true;
				tag_len = 2;
				num_comps = 3;
				dest = (f32 *) (result.normals + num_normals++);
			} else { // pos
				tag_len = 1;
				num_comps = 3;
				dest = (f32 *) (result.positions + num_positions++);
			}

			extract_vec(line_start, tag_len, num_comps, dest);
		} else if (*line_start == 'm') { // Material info
		} else if (*line_start == 'f') {

			static OBJIndex face_buffer[16];

			result.num_indices_in_face[result.num_faces] = parse_obj_face(line_start, face_buffer);

			if (result.num_faces == face_capacity) {
				face_capacity *= 2;
				result.num_indices_in_face = (u32 *) realloc((void *) result.num_indices_in_face, face_capacity * sizeof(u32));
			}

			if (result.num_indices + result.num_indices_in_face[result.num_faces] >= index_capacity) {
				index_capacity *= 2;
				result.indices = (OBJIndex *) realloc((void *) result.indices, index_capacity * sizeof(OBJIndex));
			}

			memcpy(result.indices + result.num_indices, face_buffer, result.num_indices_in_face[result.num_faces] * sizeof(OBJIndex));
			result.num_indices += result.num_indices_in_face[result.num_faces];

			result.num_faces++;
		} else if (*line_start == 'l') { // Polyline
		}

		line_start = advance_past_newline(line_start);
	}

	return result;
}

static OBJModel assemble_model(RawOBJData data)
{
	OBJModel result;
	result.num_vertices = data.num_indices;
	result.num_indices = 0;
	result.vertices = malloc(sizeof(OBJVertex) * data.num_indices);

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

	return result;
}

static Mesh create_mesh(OBJModel model)
{
	Mesh result;

	GLuint vbo;

	GL_CALL(glGenVertexArrays, 1, &result.vao);
	GL_CALL(glBindVertexArray, result.vao);

	GL_CALL(glGenBuffers, 1, &vbo);
	GL_CALL(glGenBuffers, 1, &result.ibo);

	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(OBJVertex) * model.num_vertices, model.vertices, GL_STATIC_DRAW);

	GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, result.ibo);
	GL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * model.num_indices, model.indices, GL_STATIC_DRAW);

	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glEnableVertexAttribArray, 1);
	GL_CALL(glEnableVertexAttribArray, 2);
	GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), NULL);
	GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (const GLvoid *) sizeof(vec3));
	GL_CALL(glVertexAttribPointer, 2, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (const GLvoid *) sizeof(vec3) + sizeof(vec2));

	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glDeleteBuffers, 1, &vbo);

	result.num_indices = model.num_indices;

	return result;
}

Mesh obj_load_mesh(const char *path)
{
	char *text = get_file_contents(path);

	RawOBJData raw_data = parse_obj(text);
	OBJModel model = assemble_model(raw_data);
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