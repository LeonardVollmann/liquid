#include <stdio.h>

#include "common.h"
#include "maths.h"
#include "graphics.h"
#include "shader.h"

int main(int argc, char const *argv[])
{
	vec2 v2 = {1, 2};

	Window window = graphics_create_window(640, 480, "Window");
	//Window window2 = graphics_create_window(640, 480, "Window2");

	Layer layer;
	mat4_init_to_identity(&layer.projection);
	mat4_init_to_identity(&layer.view);

	while (!graphics_terminated())
	{
		graphics_begin_frame(&window);

		vec3 pos = {0, 0, 0};
		graphics_draw_triangle(pos, 0xff00ff);

		graphics_end_frame(&window);

		// graphics_begin_frame(&window2);
		// graphics_end_frame(&window2);
	}

	return 0;
}