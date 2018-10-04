#include "shader.h"

#define BASIC_VSHADER_SOURCE "														\
	#version 330 core 																\
																					\
	layout(location = 0) in vec3 vertex_pos;										\
	layout(location = 1) in vec2 vertex_uv;											\
	layout(location = 2) in vec3 vertex_normal;										\
																					\
	out vec2 uv;																	\
	out vec3 normal;																\
																					\
	uniform mat4 view_projection;													\
	uniform mat4 transformation;													\
																					\
	void main()																		\
	{																				\
		uv = vertex_uv;																\
		normal = (transformation * vec4(vertex_normal, 0.0)).xyz;					\
		gl_Position =  transformation * view_projection * vec4(vertex_pos, 1.0);	\
	}																				\
"

#define BASIC_FSHADER_SOURCE "													\
	#version 330 core 															\
																				\
	in vec2 uv;																	\
	in vec3 normal;																\
																				\
	out vec4 frag_color;														\
																				\
	uniform sampler2D diffuse;													\
	uniform vec4 color;															\
																				\
	const vec3 light_dir = normalize(vec3(0, -1, 1));							\
																				\
	void main()																	\
	{																			\
		frag_color = dot(-light_dir, normal) * (texture(diffuse, uv) + color);	\
	}																			\
"

static struct
{
	Shader basic;
} default_shaders;

static char *load_source_from_file(const char *path)
{
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		FATAL("Could not open file: %s", path);
	}
	INFO("Loaded file: %s", path);

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);
	char *result = malloc(size + 1);
	fread(result, size, 1, file);
	fclose(file);
	*(result + size) = 0;

	return result;
}

static Shader shader_create(char *vsource, char *fsource, const char *vname, const char *fname)
{
	static char shader_info_log[1024];
	
	i32 success;
	GLuint vshader, fshader;

	vshader = glCreateShader(GL_VERTEX_SHADER);
	GL_CALL(glShaderSource, vshader, 1, (const GLchar *const *)&vsource, NULL);
	GL_CALL(glCompileShader, vshader);
	GL_CALL(glGetShaderiv, vshader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GL_CALL(glGetShaderInfoLog, vshader, 1024, 0, shader_info_log);
	    FATAL("Failed to compile vertex shader %s: %s", vname, shader_info_log);
	};

	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	GL_CALL(glShaderSource, fshader, 1, (const GLchar *const *)&fsource, NULL);
	GL_CALL(glCompileShader, fshader);
	GL_CALL(glGetShaderiv, fshader, GL_COMPILE_STATUS, &success);
	if (!success) {
	    GL_CALL(glGetShaderInfoLog, fshader, 1024, 0, shader_info_log);
	    FATAL("Failed to compile fragment shader %s: %s", fname, shader_info_log);
	};

	Shader result = glCreateProgram();
	GL_CALL(glAttachShader, result, vshader);
	GL_CALL(glAttachShader, result, fshader);
	GL_CALL(glLinkProgram, result);
	GL_CALL(glGetProgramiv, result, GL_LINK_STATUS, &success);
	if (!success) {
		GL_CALL(glGetProgramInfoLog, result, 1024, NULL, shader_info_log);
		FATAL("Failed to link shaders %s and %s: %s", vname, fname, shader_info_log);
	}

	/*
	GL_CALL(glValidateProgram, result);
	GL_CALL(glGetProgramiv, result, GL_VALIDATE_STATUS, &success);
	if (!success) {
		GL_CALL(glGetProgramInfoLog, result, 1024, NULL, shader_info_log);
		FATAL("Failed to validate shaders %s and %s: %s", vname, fname, shader_info_log);
	}
	*/
	
	GL_CALL(glDeleteShader, vshader);
	GL_CALL(glDeleteShader, fshader);

	INFO("Created Shader (vs: %s, fs: %s, program: %d).", vname, fname, result);

	return result;
}

Shader shader_load(const char *vpath, const char *fpath)
{
	char *vsource = load_source_from_file(vpath);
	char *fsource = load_source_from_file(fpath);

	Shader result = shader_create(vsource, fsource, vpath, fpath);

	free(vsource);
	free(fsource);

	return result;
}

void shader_destroy(Shader *shader)
{
	GL_CALL(glDeleteProgram, *shader);
}

void shader_bind(Shader shader)
{
	GL_CALL(glUseProgram, shader);
}

void shader_load_defaults()
{
	default_shaders.basic = shader_create(BASIC_VSHADER_SOURCE, BASIC_FSHADER_SOURCE, "basic_vs", "basic_fs");
	INFO("Loaded default shaders.");
}

void shader_destroy_defaults()
{
	shader_destroy(&default_shaders.basic);
	INFO("Destroyed default shaders.");
}

Shader shader_get_basic()
{
	return default_shaders.basic;
}