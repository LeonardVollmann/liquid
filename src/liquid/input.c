#include "input.h"
#include "graphics.h"

void input_initialize(InputData *input_data, GraphicsData *graphics_data, Window window)
{
	input_data->window = (GLFWwindow *) graphics_get_window_ptr(graphics_data, window);
}

void input_update(InputData *input_data, Window window)
{
	glfwPollEvents();

	input_data->old_cursor_pos = input_data->cursor_pos;
	input_data->cursor_pos = input_get_cursor_pos(input_data);
}

bool input_get_key(InputData *input_data, i32 key)
{
	return glfwGetKey(input_data->window, key) == GLFW_PRESS;
}

bool input_get_mouse_button(InputData *input_data, i32 mouse_button)
{
	return glfwGetMouseButton(input_data->window, mouse_button) == GLFW_PRESS; 
}


// bool input_get_key_single_detection(InputData *input_data, i32 key)
// {
// 	bool result = input_data->keys_single_detection[key];
// 	input_data->keys_single_detection[key] = false;
// 	return result;
// }

// bool input_get_mouse_button_single_detection(InputData *input_data, i32 mouse_button)
// {
// 	bool result = input_data->mouse_buttons_single_detection[mouse_button];
// 	input_data->mouse_buttons_single_detection[mouse_button] = false;
// 	return result;
// }

// bool input_was_key_pressed();
// bool input_was_key_released();

vec2 input_get_cursor_pos(InputData *input_data)
{
	f64 x, y;
	glfwGetCursorPos(input_data->window, &x, &y);
	vec2 result = { (f32) x, (f32) y };
	return result;
}

vec2 input_get_cursor_delta(InputData *input_data)
{
	return vec2_sub(input_data->cursor_pos, input_data->old_cursor_pos);
}

void input_set_cursor_pos(InputData *input_data, vec2 pos)
{
	glfwSetCursorPos(input_data->window, pos.x, pos.y);
	input_data->cursor_pos = pos;
}