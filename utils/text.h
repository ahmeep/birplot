#ifndef BIRPLOT_TEXT_H
#define BIRPLOT_TEXT_H

#include <cglm/types.h>
#include <stdarg.h>

#include "utils/data.h"
#include "utils/texture.h"

struct text_renderer {
	uint32_t characters_len;
	void *characters;
	uint32_t kernings_len;
	void *kernings;
	float line_height;

	struct texture font_texture;
	uint32_t program;
	struct vao vertex_array;
	struct vbo vertex_buffer;

	void *vertices;
	void *cursor;
	uint32_t drawing_chars_len;
};

void create_text_renderer(struct text_renderer *renderer, const char *fontfile,
			  const char *texturefile);
void free_text_renderer(struct text_renderer *renderer);

void draw_text(struct text_renderer *renderer, const wchar_t *text, vec2 position,
	       float scale, vec4 color, vec2 end_of_text);
void draw_textf(struct text_renderer *renderer, vec2 position, float scale,
		vec4 color, vec2 end_of_text, const wchar_t *fmt, ...);
void render_text(struct text_renderer *renderer, mat4 projection);

#endif
