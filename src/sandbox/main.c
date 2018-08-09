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

	Texture bricks = texture_load("res/bricks.png");
	Texture bricks2 = texture_load("res/bricks2.png");

	quat rot = quat_from_axis_angle(vec3_new(0, 0, 1), 3.14f / 4.0f);

	Transform t1 = {vec3_new(0.0f, 0.0f, 0), vec3_new(0.5f, 0.5f, 1), rot};
	Transform t2 = {vec3_new(-0.5f, -0.5f, 0), vec3_new(0.5f, 0.5f, 1), quat_null_rotation()};
	Transform t3 = {vec3_new(0, -1, -2), vec3_new(1, 1, 1), quat_from_axis_angle(vec3_new(-1, 0, 0), M_PI/2.0f)};
	quat q = quat_from_axis_angle(vec3_new(1, 0, 0), M_PI/2);
	t3.rot = quat_mul(t3.rot, q);

	// mat4 projection = mat4_ortho(-1, 1, -1, 1, 0.0f, 100);
	mat4 projection = mat4_perspective(70.0f, width/height, 0.01f, 1000.0f);

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

		// graphics_draw_rect(&t1, projection, &bricks, color1);
		graphics_draw_mesh(bunny, &t3, projection, &bricks, color1);
		// graphics_draw_triangle(&t2, projection, &bricks2, color2);

		graphics_end_frame(&window);
	}

	return 0;
}