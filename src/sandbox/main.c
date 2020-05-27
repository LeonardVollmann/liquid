#include <stdio.h>

#include "common.h"
#include "maths.h"
#include "graphics.h"
#include "shader.h"
#include "texture.h"
#include "obj_loading.h"
#include "input.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>	

int main(int argc, char const *argv[])
{

	const f32 width = 1280;
	const f32 height = 720;

	Window window = graphics_create_window(width, height, "Window");
	input_initialize(window);

	Mesh bunny = obj_load_mesh("res/bunny.obj");
	Mesh monkey = obj_load_mesh("res/monkey.obj");
	Mesh dragon = obj_load_mesh("res/dragon.obj");
	// Mesh rungholt = obj_load_mesh("res/rungholt.obj");

	Texture bricks = texture_load("res/bricks.png");
	Texture bricks2 = texture_load("res/bricks2.png");
	// Texture rungholt_texture = texture_load("res/rungholt.png");

	quat rot = quat_from_axis_angle(vec3_new(0, 0, 1), 3.14f / 4.0f);

	Transform t1 = {vec3_zero(), vec3_zero(), quat_null_rotation()};
	Transform t2 = {vec3_new(200.0f, 200.0f, 1.0f), vec3_new(1, 1, 1), quat_null_rotation()};
	Transform t3 = {vec3_new(-1.5, -1, -3), vec3_new(1, 1, 1), quat_from_axis_angle(vec3_new(-1, 0, 0), M_PI/2.0f)};
	Transform t4 = {vec3_new(2.0, -0.8, -3), vec3_new(0.6, 0.6, 0.6), quat_from_axis_angle(vec3_new(-1, 0, 0), M_PI/2.0f)};
	Transform t5 = {vec3_new(0, 0, -2), vec3_new(1, 1, 1), quat_from_axis_angle(vec3_new(-1, 0, 0), M_PI/2.0f)};
	Transform t6 = {vec3_new(200.0f, 250.0f, 1.0f), vec3_new(1, 1, 1), quat_null_rotation()};
	quat q = quat_from_axis_angle(vec3_new(1, 0, 0), M_PI/2);
	t3.rot = quat_mul(t3.rot, q);

	mat4 ortho = mat4_ortho(0, 1280, 720, 0, -1.0f, 100.0f);
	mat4 projection = mat4_perspective(70.0f, width/height, 0.0f, 1000.0f);

	Camera camera = {t1, projection};

	Font font = font_load("res/CourierNew.ttf", 32.0f);

	bool mouse_control = false;
	f32 turn_speed = 0.005f;
	vec2 angles = vec2_zero();

	f32 t = 0;
	while (!graphics_terminated())
	{
		t += 0.01f;

		input_update(window);
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

		float speed = 0.1f;
		vec2 move_amount = vec2_zero();
		if (input_get_key(KEY_W))
		{
			move_amount.y -= speed;
		}
		if (input_get_key(KEY_S))
		{
			move_amount.y += speed;
		}
		if (input_get_key(KEY_A))
		{
			move_amount.x -= speed;
		}
		if (input_get_key(KEY_D)) 
		{
			move_amount.x += speed;
		}
		transform_translate(&camera.transform, vec3_scalar_mul(quat_get_right(camera.transform.rot), move_amount.x));
		transform_translate(&camera.transform, vec3_scalar_mul(quat_get_forward(camera.transform.rot), move_amount.y));

		if (input_get_mouse_button_single_detection(MOUSE_BUTTON_LEFT)) {
			mouse_control = true;
			graphics_disable_cursor(window);
		}
		if (input_get_key_single_detection(KEY_ESCAPE)) {
			mouse_control = false;
			graphics_show_cursor(window);
			vec2 center_window = {width/2, height/2};
			input_set_cursor_pos(center_window);
		}

		if (mouse_control) {
			vec2 angle_delta = vec2_scalar_mul(input_get_cursor_delta(), turn_speed);
			angles = vec2_add(angles, angle_delta);
			
			quat rotation = quat_from_euler_angles(0, angles.x, 0);
			quat rotation2 = quat_from_euler_angles(0, 0, -angles.y);

			quat rot1 = quat_from_axis_angle(vec3_new(0, 1, 0), -angles.x);
			quat rot2 = quat_from_axis_angle(vec3_new(1, 0, 0), -angles.y);

			camera.transform.rot = quat_normalize(quat_mul(rot1, rot2));
		}

		graphics_draw_mesh(dragon, &t5, camera_view_projection(&camera), &bricks, color1);
		graphics_draw_mesh(bunny, &t3, camera_view_projection(&camera), &bricks, color1);
		graphics_draw_mesh(monkey, &t4, camera_view_projection(&camera), &bricks, color1);
		graphics_draw_text("Hello World", font, &t2, ortho);
		graphics_draw_text("It's me, Leonard", font, &t6, ortho);

		graphics_sort_and_flush_queue();

		graphics_end_frame(&window);
	}

	return 0;
}