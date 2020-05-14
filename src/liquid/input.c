#include "input.h"
#include "graphics.h"

// TODO: multiple windows, multiple inputs

static struct
{
	GLFWwindow *window;

	bool keys[512];

	bool mouseButtons[64];
	bool cursor_visible;
	bool mouse_locked;
	vec2 cursor_pos;
	vec2 old_cursor_pos;
	vec2 cursorPosDelta;
} input_data;

static void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
	if (action == GLFW_PRESS) {
		input_data.keys[key] = true;
	} else if (action == GLFW_RELEASE) {
		input_data.keys[key] = false;
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	input_data.old_cursor_pos = input_data.cursor_pos;
	input_data.cursor_pos.x = (float) xpos;
	input_data.cursor_pos.y = (float) ypos;
}

void input_initialize(void *window)
{
	input_data.window = (GLFWwindow *) window;
	glfwSetKeyCallback(input_data.window, key_callback);
	glfwSetCursorPosCallback(input_data.window, cursor_position_callback);
}

void input_update()
{
	glfwPollEvents();
}

bool input_get_key(i32 key) {
	return input_data.keys[key];
}

// bool input_was_key_pressed();
// bool input_was_key_released();

vec2 input_get_cursor_pos() {
	return input_data.cursor_pos;
}

vec2 input_get_cursor_delta() {
	return vec2_sub(input_data.cursor_pos, input_data.old_cursor_pos);
}