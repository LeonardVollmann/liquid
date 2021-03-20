#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture texture_load(const char *path)
{
	Texture result;

	result.data = stbi_load(path, &result.width, &result.height, &result.comps_per_pixel, 0);
	if (result.data == NULL) {
		FATAL("Failed to load texture: %s", path);
	}

	GL_CALL(glGenTextures, 1, &result.id);
	GL_CALL(glBindTexture, GL_TEXTURE_2D, result.id);
	
	GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, result.width, result.height, 0, GL_RGB, GL_UNSIGNED_BYTE, result.data);

	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	INFO("Loaded texture: %s", path);

	return result;
}

void texture_init(Texture *texture, i32 width, i32 height, GLenum format, GLenum type, u8 *image)
{
	texture->data = image;
	texture->width = width;
	texture->height = height;

	GL_CALL(glGenTextures, 1, &texture->id);
	GL_CALL(glBindTexture, GL_TEXTURE_2D, texture->id);
	
	GL_CALL(glTexImage2D, GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, type, image);

	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void texture_destroy(Texture *texture)
{
	GL_CALL(glDeleteTextures, 1, &texture->id);
	stbi_image_free(texture->data);
	texture->data = NULL;
	texture->width = 0;
	texture->height = 0;
	texture->comps_per_pixel = 0;
}

void texture_bind(const Texture *texture)
{
	GL_CALL(glActiveTexture, GL_TEXTURE0);
	GL_CALL(glBindTexture, GL_TEXTURE_2D, texture->id);
}