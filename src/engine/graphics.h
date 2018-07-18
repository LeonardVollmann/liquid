#pragma once

#include "common.h"
#include "maths.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GRAPHICS_MAX_WINDOWS 16
#define GRAPHICS_MAX_LAYERS 32

typedef u32 Window;

Window graphics_create_window(u32 width, u32 height, const char *title);
void graphics_destroy_window(Window *window);
bool graphics_terminated();
void graphics_begin_frame(Window *window);
void graphics_end_frame(Window *window);

typedef struct
{
	mat4 projection;
	mat4 view;
	float z;
	u32 id;
} Layer;

void graphics_add_layer(Layer *layer);

typedef GLuint Shader;
void graphics_draw_triangle(vec3 pos, u32 color);