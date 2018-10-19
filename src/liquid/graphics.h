#pragma once

#include "common.h"
#include "maths.h"
#include "texture.h"

#include "stb/stb_truetype.h"

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

typedef struct
{
	stbtt_bakedchar char_data[96];
	Texture texture;
} Font;

enum DrawCommandType
{
	DRAW_TRIANGLE,
	DRAW_RECT,
	DRAW_MESH,
	DRAW_TEXT
};

typedef struct
{
	enum DrawCommandType type;
	void *data;
} DrawCommand;

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

Window graphics_create_window(u32 width, u32 height, const char *title);
void graphics_destroy_window(Window *window);
bool graphics_terminated();
void graphics_begin_frame(Window *window);
void graphics_end_frame(Window *window);

void graphics_submit_call(DrawCommand *cmd);
void graphics_sort_and_flush_queue();

void graphics_draw_triangle(const Transform *transform, mat4 projection, const Texture *texture, vec4 color);
void graphics_draw_rect(const Transform *transform, mat4 projection, const Texture *texture, vec4 color);
void graphics_draw_mesh(Mesh mesh, const Transform *transform, mat4 projection, const Texture *texture, vec4 color);
void graphics_draw_text(const char *text, Font font, Transform *transform, mat4 projection);

Font font_load(const char *path, f32 size);