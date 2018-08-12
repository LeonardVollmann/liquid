#pragma once

#include "common.h"
#include "maths.h"
#include "texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GRAPHICS_MAX_WINDOWS 16
#define GRAPHICS_MAX_LAYERS 32

typedef u32 Window;

typedef struct
{
	GLuint vao, ibo;
	u32 num_indices;
} Mesh;

Window graphics_create_window(u32 width, u32 height, const char *title);
void graphics_destroy_window(Window *window);
bool graphics_terminated();
void graphics_begin_frame(Window *window);
void graphics_end_frame(Window *window);

void graphics_draw_triangle(const Transform *transform, mat4 projection, const Texture *texture, vec4 color);
void graphics_draw_rect(const Transform *transform, mat4 projection, const Texture *texture, vec4 color);
void graphics_draw_mesh(Mesh mesh, const Transform *transform, mat4 projection, const Texture *texture, vec4 color);