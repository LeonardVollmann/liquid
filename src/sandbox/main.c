#include <stdio.h>

#include "common.h"
#include "maths.h"
#include "graphics.h"

int main(int argc, char const *argv[])
{
	vec2 v2 = {1, 2};
	printf("Hello World\n");

	GLFWwindow *window = graphics_init(640, 480, "Window");
	while (!graphics_should_terminate(window))
	{
		graphics_begin_frame(window);

		graphics_end_frame(window);
	}
	graphics_terminate(window);

	return 0;
}