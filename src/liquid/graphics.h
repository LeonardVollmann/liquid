#pragma once

#include "common.h"
#include "maths.h"
#include "texture.h"

#include "stb/stb_truetype.h"

#define GRAPHICS_MAX_WINDOWS 16
#define GRAPHICS_MAX_LAYERS 32

typedef u32 Window;

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
	bool initialized;
	u32 num_windows;
	GLFWwindow* windows[GRAPHICS_MAX_WINDOWS];
	u32 indices[GRAPHICS_MAX_WINDOWS];

	GLuint primitive_triangle_vao;
	GLuint primitive_rect_vao;

	size_t queue_capacity;
	size_t queue_size;
	DrawCommand *queue;
} GraphicsData;

typedef struct
{
	Transform transform;
	mat4 projection;
} Camera;

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

Window graphics_create_window(GraphicsData *graphics_data, u32 width, u32 height, const char *title);
void graphics_destroy_window(GraphicsData *graphics_data, Window *window);
void *graphics_get_window_ptr(GraphicsData *graphics_data, Window window);
bool graphics_terminated(GraphicsData *graphics_data);
void graphics_begin_frame(GraphicsData *graphics_data, Window *window);
void graphics_end_frame(GraphicsData *graphics_data, Window *window);

void graphics_hide_cursor(GraphicsData *graphics_data, Window window);
void graphics_disable_cursor(GraphicsData *graphics_data, Window window);
void graphics_show_cursor(GraphicsData *graphics_data, Window window);

void graphics_submit_call(GraphicsData *graphics_data, DrawCommand *cmd);
void graphics_sort_and_flush_queue(GraphicsData *graphics_data);

void graphics_draw_triangle(GraphicsData *graphics_data, const Transform *transform, mat4 view_projection, const Texture *texture, vec4 color);
void graphics_draw_rect(GraphicsData *graphics_data, const Transform *transform, mat4 view_projection, const Texture *texture, vec4 color);
void graphics_draw_mesh(GraphicsData *graphics_data, Mesh mesh, const Transform *transform, mat4 view_projection, const Texture *texture, vec4 color);
void graphics_draw_text(GraphicsData *graphics_data, const char *text, Font font, Transform *transform, mat4 view_projection);

Font font_load(const char *path, f32 size);

mat4 camera_view_projection(const Camera *camera);