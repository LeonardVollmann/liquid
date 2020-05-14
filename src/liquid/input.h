#pragma once

#include "maths.h"

void input_initialize(void *window);
void input_update();

bool input_get_key(i32 key);

// bool input_was_key_pressed();
// bool input_was_key_released();

vec2 input_get_cursor_pos();
vec2 input_get_mouse_delta();