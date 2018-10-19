#include <stdio.h>

#include "common.h"
#include "maths.h"
#include "graphics.h"
#include "shader.h"
#include "texture.h"
#include "obj_loading.h"

int main(int argc, char const *argv[])
{
	const f32 width = 1280;
	const f32 height = 720;

	Window window = graphics_create_window(width, height, "Window");

	Mesh bunny = obj_load_mesh("res/bunny.obj");
	Mesh monkey = obj_load_mesh("res/monkey.obj");
	Mesh dragon = obj_load_mesh("res/dragon.obj");
	// Mesh rungholt = obj_load_mesh("res/rungholt.obj");

	Texture bricks = texture_load("res/bricks.png");
	Texture bricks2 = texture_load("res/bricks2.png");
	// Texture rungholt_texture = texture_load("res/rungholt.png");

	quat rot = quat_from_axis_angle(vec3_new(0, 0, 1), 3.14f / 4.0f);

	Transform t1 = {vec3_new(0.0f, 0.0f, 0), vec3_new(0.5f, 0.5f, 1), rot};
	Transform t2 = {vec3_new(200.0f, 200.0f, 1.0f), vec3_new(1, 1, 1), quat_null_rotation()};
	Transform t3 = {vec3_new(-1.5, -1, -3), vec3_new(1, 1, 1), quat_from_axis_angle(vec3_new(-1, 0, 0), M_PI/2.0f)};
	Transform t4 = {vec3_new(2.0, -0.8, -3), vec3_new(0.6, 0.6, 0.6), quat_from_axis_angle(vec3_new(-1, 0, 0), M_PI/2.0f)};
	Transform t5 = {vec3_new(0, 0, -2), vec3_new(1, 1, 1), quat_from_axis_angle(vec3_new(-1, 0, 0), M_PI/2.0f)};
	quat q = quat_from_axis_angle(vec3_new(1, 0, 0), M_PI/2);
	t3.rot = quat_mul(t3.rot, q);

	mat4 ortho = mat4_ortho(0, 1280, 720, 0, -1.0f, 100.0f);
	mat4 projection = mat4_perspective(70.0f, width/height, 0.0f, 1000.0f);

	Font font = font_load("res/Courier New.ttf", 32.0f);

	f32 t = 0;
	while (!graphics_terminated())
	{
		t += 0.01f;

		graphics_begin_frame(&window);

		vec4 color1 = {0, 0.1, 0.1, 1};
		vec4 color2 = {1, 0, 1, 1};

		t1.pos.x = cosf(t);
		t1.pos.y = sinf(t);
		t1.pos.z = -2.0f - sinf(2 * t);

		quat rotation_step = quat_from_axis_angle(vec3_new(0, 1, 0), 0.02f);
		t1.rot = quat_normalize(quat_mul(t1.rot, rotation_step));
		t3.rot = quat_mul(t3.rot, rotation_step);
		// t2.rot = t3.rot;
		t4.rot = t3.rot;
		t5.rot = t3.rot;

		// graphics_draw_mesh(dragon, &t5, projection, &bricks, color1);
		// graphics_draw_mesh(bunny, &t3, projection, &bricks, color1);
		// graphics_draw_mesh(monkey, &t4, projection, &bricks, color1);
		// graphics_draw_mesh(rungholt, &t5, projection, &rungholt_texture, vec4_zero());

		DrawMeshCommandData dmcd1 = {dragon, t5, projection, bricks, color1};
		DrawMeshCommandData dmcd2 = {bunny, t3, projection, bricks, color1};
		DrawMeshCommandData dmcd3 = {monkey, t4, projection, bricks, color1};

		DrawCommand dc1 = {DRAW_MESH, &dmcd1};
		DrawCommand dc2 = {DRAW_MESH, &dmcd2};
		DrawCommand dc3 = {DRAW_MESH, &dmcd3};

		graphics_submit_call(&dc1);
		graphics_submit_call(&dc2);
		graphics_submit_call(&dc3);

		graphics_draw_text("Hello World", font, &t2, ortho);

		graphics_sort_and_flush_queue();

		graphics_end_frame(&window);
	}

	return 0;
}