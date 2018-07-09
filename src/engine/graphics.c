#include "graphics.h"

GLFWwindow *graphics_init(u32 width, u32 height, const char *title)
{
	GLFWwindow *result;

	if (!s_initialized)
	{
		if (!glfwInit())
		{
			ERROR("Failed to initialize GLFW.");
			return NULL;
		}
		INFO("Initialized GLFW.");
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
		return NULL;
	}

	glfwMakeContextCurrent(result);

	if (!s_initialized)
	{
		glewExperimental = GL_TRUE;
		i32 error = glewInit();
		if (error)
		{
			FATAL("Failed to initialize OpenGL (error code: %d).", error);
			return NULL;
		}
		INFO("Initialized OpenGL.");
		INFO("Graphics Card Vendor: %s", glGetString(GL_VENDOR));
		INFO("Graphics Card: %s", glGetString(GL_RENDERER));
		INFO("OpenGL version: %s", glGetString(GL_VERSION));
		INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

		s_initialized = true;
	}

	return (void *)result;
}

void graphics_terminate(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	INFO("Closed window: %p", window);
}

bool graphics_should_terminate(GLFWwindow *window)
{
	return glfwWindowShouldClose(window);
}

void graphics_begin_frame(GLFWwindow *window)
{}

void graphics_end_frame(GLFWwindow *window)
{
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
	glfwPollEvents();
}