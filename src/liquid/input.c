#include "input.h"
#include "graphics.h"

// TODO: multiple windows, multiple inputs

static struct
{
	GLFWwindow *window;

	bool keys[512];
	bool keys_single_detection[512];
	bool mouse_buttons[64];
	bool mouse_buttons_single_detection[64];
	bool cursor_visible;
	bool mouse_locked;
	vec2 cursor_pos;
	vec2 old_cursor_pos;
	vec2 cursorPosDelta;
} input_data;

static void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
	if (action == GLFW_PRESS)
	{
		input_data.keys[key] = true;
		input_data.keys_single_detection[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		input_data.keys[key] = false;
		input_data.keys_single_detection[key] = false;
	}
}

static void cursor_position_callback(GLFWwindow* window, f64 xpos, f64 ypos)
{
	input_data.old_cursor_pos = input_data.cursor_pos;
	input_data.cursor_pos.x = (f32) xpos;
	input_data.cursor_pos.y = (f32) ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		input_data.mouse_buttons[button] = true;
		input_data.mouse_buttons_single_detection[button] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		input_data.mouse_buttons[button] = false;
		input_data.mouse_buttons_single_detection[button] = false;
	}
}

void input_initialize(Window window)
{
	input_data.window = (GLFWwindow *) graphics_get_window_ptr(window);
	glfwSetKeyCallback(input_data.window, key_callback);
	// glfwSetCursorPosCallback(input_data.window, cursor_position_callback);
	glfwSetMouseButtonCallback(input_data.window, mouse_button_callback);
}

void input_update(Window window)
{
	glfwPollEvents();

	f64 xpos, ypos;
	input_data.old_cursor_pos = input_data.cursor_pos;
	glfwGetCursorPos((GLFWwindow *) graphics_get_window_ptr(window), &xpos, &ypos);
	input_data.cursor_pos.x = (f32) xpos;
	input_data.cursor_pos.y = (f32) ypos;
}

bool input_get_key(i32 key)
{
	return input_data.keys[key];
}

bool input_get_mouse_button(i32 mouse_button)
{
	return input_data.mouse_buttons[mouse_button];
}


bool input_get_key_single_detection(i32 key)
{
	bool result = input_data.keys_single_detection[key];
	input_data.keys_single_detection[key] = false;
	return result;
}

bool input_get_mouse_button_single_detection(i32 mouse_button)
{
	bool result = input_data.mouse_buttons_single_detection[mouse_button];
	input_data.mouse_buttons_single_detection[mouse_button] = false;
	return result;
}

// bool input_was_key_pressed();
// bool input_was_key_released();

vec2 input_get_cursor_pos()
{
	return input_data.cursor_pos;
}

vec2 input_get_cursor_delta()
{
	return vec2_sub(input_data.cursor_pos, input_data.old_cursor_pos);
}

void input_set_cursor_pos(vec2 pos)
{
	glfwSetCursorPos(input_data.window, pos.x, pos.y);
	input_data.cursor_pos = pos;
	// input_data.old_cursor_pos = pos;
}