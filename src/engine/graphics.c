#include "graphics.h"
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
//	GLuint vbo;
//	GLuint ibo;
	GLuint vao;
	GLenum mode;
} Model;

static struct
{
	bool initialized;
	u32 num_windows;
	GLFWwindow *windows[GRAPHICS_MAX_WINDOWS];
	u32 indices[GRAPHICS_MAX_WINDOWS];

	Model primitive_triangle;
	Model primitive_rect;
} graphics_data;

static void init_primitives()
{
	static const GLfloat triangle_vertices[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f
	};

	static const GLfloat rect_vertices[] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	GLuint vbo;

	graphics_data.primitive_triangle.mode = GL_TRIANGLES;
	GL_CALL(glGenVertexArrays, 1, &graphics_data.primitive_triangle.vao);
	GL_CALL(glBindVertexArray, graphics_data.primitive_triangle.vao);
	GL_CALL(glGenBuffers, 1, &vbo);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GL_CALL(glBindVertexArray, 0);
	GL_CALL(glDeleteBuffers, 1, &vbo);

	graphics_data.primitive_rect.mode = GL_TRIANGLE_STRIP;
	GL_CALL(glGenVertexArrays, 1, &graphics_data.primitive_rect.vao);
	GL_CALL(glBindVertexArray, graphics_data.primitive_rect.vao);
	GL_CALL(glGenBuffers, 1, &vbo);
	GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
	GL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);
	GL_CALL(glEnableVertexAttribArray, 0);
	GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
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

	result = glfwCreateWindow(640, 480, title, NULL, NULL);
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

		graphics_data.initialized = true;
	}

	graphics_data.windows[graphics_data.indices[graphics_data.num_windows]] = result;
	graphics_data.num_windows++;

	init_primitives();

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
		GL_CALL(glClear, GL_COLOR_BUFFER_BIT);
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

void graphics_add_layer(Layer *layer)
{
	//layer->id = graphics_data.num_layers++;
}

void graphics_draw_triangle(vec3 pos, u32 color)
{
	shader_bind(shader_get_basic());

	const mat4 transformation = mat4_identity();
	const mat4 view_projection = mat4_identity();
	GLint loc_transformation = glGetUniformLocation(shader_get_basic(), "view_projection");
	GLint loc_view_projection = glGetUniformLocation(shader_get_basic(), "transformation");
	GLint loc_color = glGetUniformLocation(shader_get_basic(), "color");

	GL_CALL(glUniformMatrix4fv, loc_transformation, 1, GL_FALSE, transformation.M);
	GL_CALL(glUniformMatrix4fv, loc_view_projection, 1, GL_FALSE, view_projection.M);
	GL_CALL(glUniform4f, loc_color, 1, 0, 1, 1);

	GL_CALL(glBindVertexArray, graphics_data.primitive_triangle.vao);
	GL_CALL(glDrawArrays, graphics_data.primitive_triangle.mode, 0, 3);
}