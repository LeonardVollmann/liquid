#pragma once

#include "common.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct
{
	GLuint id;
	u8 *data;
	i32 width;
	i32 height;
	i32 comps_per_pixel;
} Texture;

Texture texture_load(const char *path);
void texture_init(Texture *texture, i32 width, i32 height, GLenum format, GLenum type, u8 *image);
void texture_destroy(Texture *texture);
void texture_bind(const Texture *texture);