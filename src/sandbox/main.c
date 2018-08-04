#include <stdio.h>

#include "common.h"
#include "maths.h"
#include "graphics.h"
#include "shader.h"
#include "texture.h"

int main(int argc, char const *argv[])
{
	vec2 v2 = {1, 2};

	Window window = graphics_create_window(640, 480, "Window");
	//Window window2 = graphics_create_window(640, 480, "Window2");

	Texture bricks = texture_load("res/bricks.png");
	Texture bricks2 = texture_load("res/bricks2.png");

	while (!graphics_terminated())
	{
		graphics_begin_frame(&window);

		vec3 pos = {0, 0, 0};
		vec2 scale = {1, 1};
		vec4 color1 = {0, 0.1, 0.1, 1};
		vec4 color2 = {0.1, 0, 0.1, 1};
		graphics_draw_rect(pos, &bricks, color1);
		graphics_draw_triangle(pos, &bricks2, color2);

		//graphics_draw_text(pos, "Hello World");

		graphics_end_frame(&window);

		// graphics_begin_frame(&window2);
		// graphics_end_frame(&window2);
	}

	return 0;
}