#include "graphics.h"

static struct
{
	bool initialized;
	u32 num_windows;
	GLFWwindow *windows[GRAPHICS_MAX_WINDOWS];
	u32 indices[GRAPHICS_MAX_WINDOWS];
} graphics_info;

Window graphics_create_window(u32 width, u32 height, const char *title)
{
	GLFWwindow *result;

	if (!graphics_info.initialized)
	{
		for (u32 i = 0; i < GRAPHICS_MAX_WINDOWS; i++) {
			graphics_info.indices[i] = i;
		}

		if (!glfwInit())
		{
			ERROR("Failed to initialize GLFW.");
			return -1;
		}
		INFO("Initialized GLFW.");
	}

	if (graphics_info.num_windows == GRAPHICS_MAX_WINDOWS)
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

	if (!graphics_info.initialized)
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

		graphics_info.initialized = true;
	}

	graphics_info.windows[graphics_info.indices[graphics_info.num_windows]] = result;
	graphics_info.num_windows++;

	return graphics_info.num_windows - 1;
}

void graphics_destroy_window(Window *window)
{
	GLFWwindow **temp = &graphics_info.windows[graphics_info.indices[*window]]; 
	glfwDestroyWindow(*temp);
	*temp = graphics_info.windows[graphics_info.indices[graphics_info.num_windows - 1]];
	graphics_info.indices[graphics_info.num_windows - 1] = graphics_info.indices[*window];
	graphics_info.num_windows--;
	INFO("Closed window: %d", *window);
	*window = -1;

	if (graphics_info.num_windows == 0)
	{
		INFO("All windows are closed, terminating GLFW.");
		glfwTerminate();
	}
}

bool graphics_terminated()
{
	return graphics_info.initialized && !graphics_info.num_windows;
}

bool window_should_close(Window *window)
{
	return glfwWindowShouldClose(graphics_info.windows[graphics_info.indices[*window]]);
}

void graphics_begin_frame(Window *window)
{
	if (*window != -1)
	{}
}

void graphics_end_frame(Window *window)
{
	if (*window != -1)
	{
		glfwMakeContextCurrent(graphics_info.windows[graphics_info.indices[*window]]);
		glfwSwapBuffers(graphics_info.windows[graphics_info.indices[*window]]);
		glfwPollEvents();

		if (window_should_close(window)) {
			graphics_destroy_window(window);
		}
	}
}