#ifndef BIRPLOT_TEXTURE_H
#define BIRPLOT_TEXTURE_H

#include <stdint.h>

// i suppose i will not be using this extension in this project but i am keeping
// the code just in case.
#undef BIRPLOT_BINDLESS_TEXTURES

struct texture {
	uint32_t width, height;
	const char *filename;
	uint32_t texture_name;
#ifdef BIRPLOT_BINDLESS_TEXTURES
	uint64_t texture_handle;
#endif
};

// args must be pointers of struct texture.
void load_textures(uint32_t texture_count, ...);
void bind_texture(const struct texture texture, uint32_t unit);

#endif
