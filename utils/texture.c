#include "texture.h"

#include <stdarg.h>

#include "gl.h"
#include "stb_image.h"

void load_textures(uint32_t texture_count, ...)
{
	va_list ptr;
	va_start(ptr, texture_count);
	uint32_t texture_names[texture_count];
	glCreateTextures(GL_TEXTURE_2D, texture_count, texture_names);
	for (uint32_t i = 0; i < texture_count; i++) {
		uint32_t texture_name = texture_names[i];
		struct texture *texture_info = va_arg(ptr, struct texture *);

		int image_width, image_height, n;
		uint8_t *data = stbi_load(texture_info->filename, &image_width,
					  &image_height, &n, 4);
		if ((uint32_t)image_width != texture_info->width
		    || (uint32_t)image_height != texture_info->height) {
			// texture info does not match with the image file. do
			// not load.
			stbi_image_free(data);
			continue;
		}

		texture_info->texture_name = texture_name;

		glTextureParameteri(texture_name, GL_TEXTURE_MIN_FILTER,
				    GL_LINEAR);
		glTextureParameteri(texture_name, GL_TEXTURE_MAG_FILTER,
				    GL_LINEAR);
		glTextureParameteri(texture_name, GL_TEXTURE_WRAP_S,
				    GL_CLAMP_TO_EDGE);
		glTextureParameteri(texture_name, GL_TEXTURE_WRAP_T,
				    GL_CLAMP_TO_EDGE);
		glTextureStorage2D(texture_name, 1, GL_RGBA8,
				   texture_info->width, texture_info->height);
		glTextureSubImage2D(texture_name, 0, 0, 0, texture_info->width,
				    texture_info->height, GL_RGBA,
				    GL_UNSIGNED_BYTE, data);

#ifdef BIRPLOT_BINDLESS_TEXTURES
		uint64_t texture_handle = glGetTextureHandleARB(texture_name);
		texture_info->texture_handle = texture_handle;
#endif

		stbi_image_free(data);
	}
	va_end(ptr);
}

void bind_texture(const struct texture texture, uint32_t unit)
{
	glBindTextureUnit(unit, texture.texture_name);
}
