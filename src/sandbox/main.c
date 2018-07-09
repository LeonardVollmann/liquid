#include <stdio.h>

#include "common.h"
#include "maths.h"
#include "graphics.h"

int main(int argc, char const *argv[])
{
	vec2 v2 = {1, 2};
	printf("Hello World\n");

	Window window = graphics_create_window(640, 480, "Window");
	Window window2 = graphics_create_window(640, 480, "Window2");
	while (!graphics_terminated())
	{
		graphics_begin_frame(&window);

		graphics_end_frame(&window);
		graphics_begin_frame(&window2);

		graphics_end_frame(&window2);
	}

	return 0;
}