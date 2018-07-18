#pragma once

#include "common.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef GLuint Shader;

Shader shader_load(const char *vpath, const char *fpath);
void shader_destroy(Shader *shader);
void shader_bind(Shader shader);

void shader_load_defaults();
void shader_destroy_defaults();

Shader shader_get_basic();