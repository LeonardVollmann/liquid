#pragma once

#include "common.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GRAPHICS_MAX_WINDOWS 16

static bool s_initialized = false;

typedef u32 Window;

Window graphics_create_window(u32 width, u32 height, const char *title);
void graphics_destroy_window(Window *window);

bool graphics_terminated();

bool window_should_close(Window *window);

void graphics_begin_frame(Window *window);
void graphics_end_frame(Window *window);