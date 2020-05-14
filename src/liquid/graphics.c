#include "graphics.h"
#include "shader.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>	

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	Transform transform;
	mat4 projection;
	Texture texture;
	vec4 color;
} DrawTriangleCommandData;

typedef struct
{
	Transform transform;
	mat4 projection;
	Texture texture;
	vec4 color;
} DrawRectCommandData;

typedef struct
{
	Mesh mesh;
	Transform transform;
	mat4 projection;
	Texture texture;
	vec4 color;
} DrawMeshCommandData;

typedef struct
{
	Transform transform;
	mat4 projection;
	Font font;
} DrawTextCommandData;

static struct
{
	bool initialized;
	u32 num_windows;
	GLFWwindow *windows[GRAPHICS_MAX_WINDOWS];
	u32 indices[GRAPHICS_MAX_WINDOWS];

	GLuint primitive_triangle_vao;
	GLuint primitive_rect_vao;

	size_t queue_capacity;
	size_t queue_size;
	DrawCommand *queue;
} graphics_data;

static void init_primitives()
{
	static const GLfloat triangle_vertices[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f
	};

	static const GLfloat triangle_uvs[] = {
		0.0f, 0.0f,
		0.5f, 1.0f,
		1.0f, 0.0f
	};

	static const GLfloat rect_vertices[] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};

	static const GLfloat rect_uvs[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	GLuint vbo;

	GL_CALL(glGenVertexArrays, 1, &graphics_data.primitive_triangle_vao);
	GL_CALL(glBindVertexArray, graphics_data.primitive_triangle_vao);
	GL_CALL(glGenBuffers, 1, &vbo);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(triangle_vertices) + sizeof(triangle_uvs), triangle_vertices, GL_STATIC_DRAW);
	GL_CALL(glBufferSubData, GL_ARRAY_BUFFER, sizeof(triangle_vertices), sizeof(triangle_uvs), triangle_uvs);
	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glEnableVertexAttribArray, 1);
	GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) sizeof(triangle_vertices));
	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glDeleteBuffers, 1, &vbo);

	GL_CALL(glGenVertexArrays, 1, &graphics_data.primitive_rect_vao);
	GL_CALL(glBindVertexArray, graphics_data.primitive_rect_vao);
	GL_CALL(glGenBuffers, 1, &vbo);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(rect_vertices) + sizeof(rect_uvs), rect_vertices, GL_STATIC_DRAW);
	GL_CALL(glBufferSubData, GL_ARRAY_BUFFER, sizeof(rect_vertices), sizeof(rect_uvs), rect_uvs);
	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glEnableVertexAttribArray, 1);
	GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) sizeof(rect_vertices));
	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glDeleteBuffers, 1, &vbo);
}

Window graphics_create_window(u32 width, u32 height, const char *title)
{
	GLFWwindow *result;

	if (!graphics_data.initialized)
	{
		for (u32 i = 0; i < GRAPHICS_MAX_WINDOWS; i++) {
			graphics_data.indices[i] = i;
		}

		if (!glfwInit())
		{
			ERROR("Failed to initialize GLFW.");
			return -1;
		}
		INFO("Initialized GLFW.");
	}

	if (graphics_data.num_windows == GRAPHICS_MAX_WINDOWS)
	{
		ERROR("Maximum number of windows surpassed.");
		return -1;
	}

	// @TODO: adapt this to other machines
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	result = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!result)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(result);

	if (!graphics_data.initialized)
	{
		glewExperimental = GL_TRUE;
		i32 error = glewInit();
		if (error)
		{
			FATAL("Failed to initialize OpenGL (error code: %d).", error);
			return -1;
		}
		INFO("Initialized OpenGL.");
		INFO("Graphics Card Vendor: %s", glGetString(GL_VENDOR));
		INFO("Graphics Card: %s", glGetString(GL_RENDERER));
		INFO("OpenGL version: %s", glGetString(GL_VERSION));
		INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

		shader_load_defaults();
		init_primitives();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_CLAMP);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		graphics_data.initialized = true;
	}

	graphics_data.windows[graphics_data.indices[graphics_data.num_windows]] = result;
	graphics_data.num_windows++;

	// TEMP
	graphics_data.queue = malloc(1024 * sizeof(DrawCommand));
	graphics_data.queue_capacity = 1024;

	INFO("Created window. Title: %s, width: %d, height: %d", title, width, height);

	return graphics_data.num_windows - 1;
}

void graphics_destroy_window(Window *window)
{
	GLFWwindow **temp = &graphics_data.windows[graphics_data.indices[*window]]; 
	glfwDestroyWindow(*temp);
	*temp = graphics_data.windows[graphics_data.indices[graphics_data.num_windows - 1]];
	graphics_data.indices[graphics_data.num_windows - 1] = graphics_data.indices[*window];
	graphics_data.num_windows--;
	INFO("Closed window: %d", *window);
	*window = -1;

	if (graphics_data.num_windows == 0)
	{
		INFO("All windows are closed.");
		shader_destroy_defaults();
		glfwTerminate();
		INFO("Terminated GLFW.");
	}
}

void *graphics_get_window_pointer(Window window) {
	return graphics_data.windows[window];
}

bool graphics_terminated()
{
	return graphics_data.initialized && !graphics_data.num_windows;
}

bool window_should_close(Window *window)
{
	return glfwWindowShouldClose(graphics_data.windows[graphics_data.indices[*window]]);
}

void graphics_begin_frame(Window *window)
{
	if (*window != -1)
	{
		glfwMakeContextCurrent(graphics_data.windows[graphics_data.indices[*window]]);
		GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void graphics_end_frame(Window *window)
{
	if (*window != -1)
	{
		glfwMakeContextCurrent(graphics_data.windows[graphics_data.indices[*window]]);
		glfwSwapBuffers(graphics_data.windows[graphics_data.indices[*window]]);
		glfwPollEvents();

		if (window_should_close(window)) {
			graphics_destroy_window(window);
		}
	}
}

void graphics_submit_call(DrawCommand *cmd)
{
	// @TODO: implement queue type
	graphics_data.queue[graphics_data.queue_size] = *cmd;
	graphics_data.queue_size += 1;
}

static void exexute_draw_command(DrawCommand cmd)
{
	if (cmd.type == DRAW_TRIANGLE) {
		DrawTriangleCommandData *data = (DrawTriangleCommandData *) cmd.data;
		graphics_draw_triangle(&data->transform, data->projection, &data->texture, data->color);
	} else if (cmd.type == DRAW_RECT) {
		DrawRectCommandData *data = (DrawRectCommandData *) cmd.data;
		graphics_draw_rect(&data->transform, data->projection, &data->texture, data->color);
	} else if (cmd.type == DRAW_MESH) {
		DrawMeshCommandData *data = (DrawMeshCommandData *) cmd.data;
		graphics_draw_mesh(data->mesh, &data->transform, data->projection, &data->texture, data->color);
	} else {
		FATAL("Unknown draw command type: %d", cmd.type);
	}
}

void graphics_sort_and_flush_queue()
{
	// @TODO: key generation and sorting

	// Flushing
	for (u32 i = 0; i < graphics_data.queue_size; i++) {
		exexute_draw_command(graphics_data.queue[i]);
	}

	graphics_data.queue_size = 0;
}

void graphics_draw_triangle(const Transform *transform, mat4 projection, const Texture *texture, vec4 color)
{
	shader_bind(shader_get_basic());
	texture_bind(texture);

	GLint loc_transformation = glGetUniformLocation(shader_get_basic(), "view_projection");
	GLint loc_view_projection = glGetUniformLocation(shader_get_basic(), "transformation");
	GLint loc_color = glGetUniformLocation(shader_get_basic(), "color");
	GLint loc_diffuse = glGetUniformLocation(shader_get_basic(), "diffuse");

	GL_CALL(glUniformMatrix4fv, loc_transformation, 1, GL_FALSE, mat4_transformation(transform).M);
	GL_CALL(glUniformMatrix4fv, loc_view_projection, 1, GL_FALSE, projection.M);
	GL_CALL(glUniform4f, loc_color, color.r, color.g, color.b, color.a);
	GL_CALL(glUniform1i, loc_diffuse, 0);

	GL_CALL(glBindVertexArray, graphics_data.primitive_triangle_vao);
	GL_CALL(glDrawArrays, GL_TRIANGLES, 0, 3);
}

void graphics_draw_rect(const Transform *transform, mat4 projection, const Texture *texture, vec4 color)
{
	shader_bind(shader_get_basic());
	texture_bind(texture);

	GLint loc_transformation = glGetUniformLocation(shader_get_basic(), "view_projection");
	GLint loc_view_projection = glGetUniformLocation(shader_get_basic(), "transformation");
	GLint loc_color = glGetUniformLocation(shader_get_basic(), "color");
	GLint loc_diffuse = glGetUniformLocation(shader_get_basic(), "diffuse");

	GL_CALL(glUniformMatrix4fv, loc_transformation, 1, GL_FALSE, mat4_transformation(transform).M);
	GL_CALL(glUniformMatrix4fv, loc_view_projection, 1, GL_FALSE, projection.M);
	GL_CALL(glUniform4f, loc_color, color.r, color.g, color.b, color.a);
	GL_CALL(glUniform1i, loc_diffuse, 0);

	GL_CALL(glBindVertexArray, graphics_data.primitive_rect_vao);
	GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);
}

void graphics_draw_mesh(Mesh mesh, const Transform *transform, mat4 projection, const Texture *texture, vec4 color)
{
	shader_bind(shader_get_basic());
	texture_bind(texture);

	GLint loc_transformation = glGetUniformLocation(shader_get_basic(), "transformation");
	GLint loc_view_projection = glGetUniformLocation(shader_get_basic(), "view_projection");
	GLint loc_color = glGetUniformLocation(shader_get_basic(), "color");
	GLint loc_diffuse = glGetUniformLocation(shader_get_basic(), "diffuse");

	GL_CALL(glUniformMatrix4fv, loc_transformation, 1, GL_FALSE, mat4_transformation(transform).M);
	GL_CALL(glUniformMatrix4fv, loc_view_projection, 1, GL_FALSE, projection.M);
	GL_CALL(glUniform4f, loc_color, color.r, color.g, color.b, color.a);
	GL_CALL(glUniform1i, loc_diffuse, 0);

	GL_CALL(glBindVertexArray, mesh.vao);
	GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	GL_CALL(glDrawElements, GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, NULL);
}

static char *get_file_contents(const char *path) // @TODO: centralize this function, it also is in obj_loading
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

void graphics_draw_text(const char *text, Font font, Transform *transform, mat4 projection)
{
	shader_bind(shader_get_text());
	texture_bind(&font.texture);

	GLint loc_transformation = glGetUniformLocation(shader_get_text(), "transformation");
	GLint loc_view_projection = glGetUniformLocation(shader_get_text(), "view_projection");
	GLint loc_color = glGetUniformLocation(shader_get_text(), "color");
	GLint loc_diffuse = glGetUniformLocation(shader_get_text(), "diffuse");

	vec4 color;
	GL_CALL(glUniformMatrix4fv, loc_transformation, 1, GL_FALSE, mat4_transformation(transform).M);
	GL_CALL(glUniformMatrix4fv, loc_view_projection, 1, GL_FALSE, projection.M);
	GL_CALL(glUniform4f, loc_color, color.r, color.g, color.b, color.a);
	GL_CALL(glUniform1i, loc_diffuse, 0);

	vec2 positions[8 * 256]; // @TODO: static allocation
	vec2 *uvs = positions + (4 * 256);

	f32 x = 0.0f;
	f32 y = 0.0f;
	u32 i = 0;
	while (text[i]) {
		if (text[i] >= 32 /*&& text[i] < 128*/) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(font.char_data, 512, 512, text[i] - 32, &x, &y, &q, 1);

			positions[4 * i + 0].x = q.x0; positions[4 * i + 0].y = q.y1;
			positions[4 * i + 1].x = q.x1; positions[4 * i + 1].y = q.y1;
			positions[4 * i + 3].x = q.x1; positions[4 * i + 3].y = q.y0;
			positions[4 * i + 2].x = q.x0; positions[4 * i + 2].y = q.y0;

			uvs[4 * i + 0].x = q.s0; uvs[4 * i + 0].y = q.t1;
			uvs[4 * i + 1].x = q.s1; uvs[4 * i + 1].y = q.t1;
			uvs[4 * i + 3].x = q.s1; uvs[4 * i + 3].y = q.t0;
			uvs[4 * i + 2].x = q.s0; uvs[4 * i + 2].y = q.t0;
		}

		i++;
	}

	GLuint vao, vbo;
	GL_CALL(glGenVertexArrays, 1, &vao);
	GL_CALL(glBindVertexArray, vao);
	GL_CALL(glGenBuffers, 1, &vbo);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	// GL_CALL(glBufferSubData, GL_ARRAY_BUFFER, sizeof(positions), sizeof(uvs), uvs);
	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glEnableVertexAttribArray, 1);
	GL_CALL(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	GL_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *) (sizeof(vec2) * 4 * 256));
	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glDeleteBuffers, 1, &vbo);

	GL_CALL(glBindVertexArray, vao);
	GL_CALL(glDrawArrays, GL_TRIANGLE_STRIP, 0, i * 4);

	GL_CALL(glDeleteVertexArrays, 1, &vao);
}

Font font_load(const char *path, f32 size)
{
	static u8 temp_bitmap[512 * 512]; // @TODO: Remove this static allocation

	char *ttf_buffer = get_file_contents(path);

	Font result;
	stbtt_BakeFontBitmap((const unsigned char *) ttf_buffer, 0, size, temp_bitmap, 512, 512, 32, 96, result.char_data);
	texture_init(&result.texture, 512, 512, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);

	free(ttf_buffer);

	return result;
}

void font_destroy(Font *font)
{
	// @TODO: Implement
}