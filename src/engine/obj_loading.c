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
	vec3 *positions;
	vec2 *uvs;
	vec3 *normals;

	OBJIndex *indices;

	// u32 num_positions;
	// u32 num_uvs;
	// u32 num_normals;
	u32 num_indices;

	bool has_uvs;
	bool has_normals;

	// u32 indices_per_face;

	// size_t vertex_capacity;
	// size_t index_capacity;
} RawOBJData;

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

static char *advance_to(char *ptr, char c)
{
	while (*++ptr != c) {}
	return ptr;
}

static char *advance_to_slash_or_space(char *ptr)
{
	while (*++ptr != '/' && *ptr != ' ') {}
	return ptr;
}

static char *advance_to_slash_or_whitespace(char *ptr)
{
	while (*++ptr != '/' && *ptr != ' ' && *ptr != '\n') {}
	return ptr;
}

static char *advance_to_whitespace_or_zero(char *ptr)
{
	while (*++ptr != ' ' && *ptr != '\n' && *ptr != 0) {}
	return ptr;
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

static bool is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

static RawOBJData extract_raw_data(char *text)
{

	// Monkey: 7958 positions, 9907 uvs, 7872 faces -> 31488 indices
	// Bunny: 72027 positions, 72018 normals, 72201 uvs, 144046 triangles

	RawOBJData result;

	size_t vertex_capacity = OBJMODEL_INITIAL_VERTEX_CAPACITY;
	size_t index_capacity = OBJMODEL_INITIAL_INDEX_CAPACITY;

	result.positions = malloc(sizeof(vec3) * vertex_capacity);
	result.uvs = malloc(sizeof(vec2) * vertex_capacity);
	result.normals = malloc(sizeof(vec3) * vertex_capacity);
	result.indices = malloc(sizeof(OBJIndex) * index_capacity);

	u32 num_positions = 0;
	u32 num_uvs = 0;
	u32 num_normals = 0;
	result.num_indices = 0;

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
			u32 num_indices_in_face = 0;

			char *index_start = line_start + 2; // Skip "f"
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

			if (result.num_indices + num_indices_in_face >= index_capacity) {
				index_capacity *= 2;
				result.indices = (OBJIndex *) realloc((void *) result.indices, index_capacity * sizeof(OBJIndex));
			}

			memcpy(result.indices + result.num_indices, face_buffer, num_indices_in_face * sizeof(OBJIndex));
			result.num_indices += num_indices_in_face;

		} else if (*line_start == 'l') { // Polyline
		}

		line_start = advance_past_newline(line_start);
	}

	return result;
}

typedef struct
{
	vec3 pos;
	vec2 uv;
	vec3 normal;
} OBJVertex;

typedef struct
{
	OBJVertex *vertices;
	u32 num_vertices;
} OBJModel;

// @TODO: Triangulation
static OBJModel assemble_model(RawOBJData raw_data)
{
	OBJModel result;
	result.num_vertices = raw_data.num_indices;
	result.vertices = malloc(sizeof(OBJVertex) * raw_data.num_indices);

	for (u32 i = 0; i < raw_data.num_indices; i++) {

		OBJIndex index = raw_data.indices[i];
		OBJVertex v;

		v.pos = raw_data.positions[index.pos];

		if (raw_data.has_uvs) {
			v.uv = raw_data.uvs[index.uv];
		}

		if (raw_data.has_normals) {
			v.normal = raw_data.normals[index.normal];
		}

		result.vertices[i] = v;
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
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(OBJVertex) * model.num_vertices, model.vertices, GL_STATIC_DRAW);

	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glEnableVertexAttribArray, 1);
	GL_CALL(glEnableVertexAttribArray, 2);
	GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), NULL);
	GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (const GLvoid *) sizeof(vec3));
	GL_CALL(glVertexAttribPointer, 2, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (const GLvoid *) sizeof(vec3) + sizeof(vec2));

	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glDeleteBuffers, 1, &vbo);

	result.num_vertices = model.num_vertices;

	return result;
}

Mesh obj_load_mesh(const char *path)
{
	char *text = get_file_contents(path);

	RawOBJData raw_data = extract_raw_data(text);
	OBJModel model = assemble_model(raw_data);
	Mesh result = create_mesh(model);

	free(model.vertices);
	free(raw_data.indices);
	free(raw_data.positions);
	free(raw_data.uvs);
	free(raw_data.normals);
	free(text);

	INFO("Loaded mesh: %s", path);

	return result;
}