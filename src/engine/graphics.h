#pragma once

#include "common.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static bool s_initialized = false;

GLFWwindow *graphics_init(u32 width, u32 height, const char *title);
void graphics_terminate(GLFWwindow *window);

bool graphics_should_terminate(GLFWwindow *window);

void graphics_begin_frame(GLFWwindow *window);
void graphics_end_frame(GLFWwindow *window);