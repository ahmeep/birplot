#include "text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <cglm/types.h>
#include <cglm/vec2.h>
#include <cglm/vec4.h>

#include "gl.h"

#include "utils/file.h"
#include "utils/shader.h"

struct character {
	uint32_t id;
	float x, y, width, height, xoffset, yoffset, xadvance;
};

struct kerning {
	uint32_t first, second;
	float amount;
};

struct character_vertex {
	vec2 position;
	vec2 texture_coordinates;
	vec4 color;
};

const uint32_t MAX_CHARACTERS = 1024;
const uint32_t MAX_VERTICES = MAX_CHARACTERS * 4;
const uint32_t MAX_INDICES = MAX_CHARACTERS * 6;

void read_fontfile(struct text_renderer *renderer, const char *fontfile)
{

	char token[1000];

	FILE *font_stream = fopen(fontfile, "r");

	renderer->characters_len = 0;
	renderer->kernings_len = 0;

	// TODO: improve this! (maybe make it more abstract?)
	while (!feof(font_stream)) {
		read_until_char(token, ' ', font_stream);

		if (strncmp(token, "chars", 5) == 0) {
			read_until_char(token, '=', font_stream);
			read_until_char(token, '\n', font_stream);
			int count = atoi(token);
			renderer->characters =
				calloc(count, sizeof(struct character));
		} else if (strncmp(token, "char", 4) == 0) {
			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			uint32_t id = atoi(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float x = atof(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float y = atof(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float width = atof(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float height = atof(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float xoffset = atof(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float yoffset = atof(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float xadvance = atof(token);

			((struct character *)renderer
				 ->characters)[renderer->characters_len] =
				(struct character){id,	    x,	     y,
						   width,   height,  xoffset,
						   yoffset, xadvance};
			renderer->characters_len++;

			read_until_char(token, '\n', font_stream);
		} else if (strncmp(token, "common", 6) == 0) {
			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			renderer->line_height = atof(token);

			read_until_char(token, ' ', font_stream);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			renderer->font_texture.width = atoi(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			renderer->font_texture.height = atoi(token);

			read_until_char(token, '\n', font_stream);
		} else if (strncmp(token, "kernings", 8) == 0) {
			read_until_char(token, '=', font_stream);
			read_until_char(token, '\n', font_stream);
			int count = atoi(token);
			renderer->kernings =
				calloc(count, sizeof(struct kerning));
		} else if (strncmp(token, "kerning", 7) == 0) {
			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			uint32_t first = atoi(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			uint32_t second = atoi(token);

			read_until_char(token, '=', font_stream);
			read_until_char(token, ' ', font_stream);
			float amount = atof(token);

			((struct kerning *)
				 renderer->kernings)[renderer->kernings_len] =
				(struct kerning){first, second, amount};
			renderer->kernings_len++;

			read_until_char(token, '\n', font_stream);
		} else {
			read_until_char(token, '\n', font_stream);
		}
	}

	fclose(font_stream);
}

void create_text_renderer(struct text_renderer *renderer, const char *fontfile,
			  const char *texturefile)
{
	renderer->font_texture = (struct texture){
		.width = 0, .height = 0, .filename = texturefile};

	read_fontfile(renderer, fontfile);

	load_shader_file(&(renderer->program), "./assets/shaders/font.vert",
			 "./assets/shaders/font.frag");
	load_textures(1, &(renderer->font_texture));

	renderer->vertices =
		calloc(MAX_VERTICES, sizeof(struct character_vertex));
	renderer->cursor = renderer->vertices;

	uint32_t format[] = {FL_FLOAT, 2, FL_FLOAT, 2, FL_FLOAT, 4};
	renderer->vertex_buffer = generate_vbo(format, 3);
	struct ebo index_buffer = generate_ebo();

	renderer->vertex_array =
		generate_vao(&(renderer->vertex_buffer), 1, index_buffer);
	allocate_vbo(renderer->vertex_buffer,
		     MAX_VERTICES * sizeof(struct character_vertex), NULL,
		     AO_IMMUTABLE | AO_DYNAMIC | AO_READ | AO_WRITE);

	uint32_t indices[MAX_INDICES];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < MAX_INDICES; i += 6) {
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

	allocate_ebo(index_buffer, sizeof(indices), indices, AO_IMMUTABLE);
}

void free_text_renderer(struct text_renderer *renderer)
{
	free(renderer->vertices);
	free(renderer->characters);
	free(renderer->kernings);
}

struct character get_character(struct text_renderer *renderer, uint32_t id)
{
	for (uint32_t i = 0; i < renderer->characters_len; i++) {
		struct character c =
			((struct character *)renderer->characters)[i];
		if (c.id == id) {
			return c;
		}
	}

	return ((struct character *)renderer->characters)[0];
}

float get_kerning(struct text_renderer *renderer, uint32_t first,
		  uint32_t second)
{
	for (uint32_t i = 0; i < renderer->kernings_len; i++) {
		struct kerning k = ((struct kerning *)renderer->kernings)[i];
		if (k.first == first && k.second == second) {
			return k.amount;
		}
	}

	return 0.0f;
}

void draw_text(struct text_renderer *renderer, const wchar_t *text,
	       vec2 position, float scale, vec4 color, vec2 end_of_text)
{
	if (renderer->drawing_chars_len >= MAX_CHARACTERS) {
		if (end_of_text != NULL) {
			glm_vec2_copy(position, end_of_text);
		}
	}

	vec2 cursor;
	glm_vec2_copy(position, cursor);

	uint32_t i = 0;
	uint32_t last_char = 0;

	wchar_t c = text[i];
	while (c != '\0') {
		if (c == '\n') {
			cursor[1] += renderer->line_height * scale;
			cursor[0] = position[0];
		} else {
			struct character ch = get_character(renderer, c);
			float kerning = get_kerning(renderer, last_char, c);
			cursor[0] += kerning * scale;

			float xoffset = ch.xoffset * scale;
			float yoffset = ch.yoffset * scale;
			float width = ch.width * scale;
			float height = ch.height * scale;

			uint32_t tex_width = renderer->font_texture.width;
			uint32_t tex_height = renderer->font_texture.height;

			struct character_vertex *vertex_cursor =
				renderer->cursor;

			vertex_cursor->position[0] = cursor[0] + xoffset;
			vertex_cursor->position[1] = cursor[1] + yoffset;
			vertex_cursor->texture_coordinates[0] =
				ch.x / tex_width;
			vertex_cursor->texture_coordinates[1] =
				ch.y / tex_height;
			glm_vec4_copy(color, vertex_cursor->color);
			vertex_cursor++;

			vertex_cursor->position[0] =
				cursor[0] + width + xoffset;
			vertex_cursor->position[1] = cursor[1] + yoffset;
			vertex_cursor->texture_coordinates[0] =
				(ch.x + ch.width) / tex_width;
			vertex_cursor->texture_coordinates[1] =
				ch.y / tex_height;
			glm_vec4_copy(color, vertex_cursor->color);
			vertex_cursor++;

			vertex_cursor->position[0] =
				cursor[0] + width + xoffset;
			vertex_cursor->position[1] =
				cursor[1] + height + yoffset;
			vertex_cursor->texture_coordinates[0] =
				(ch.x + ch.width) / tex_width;
			vertex_cursor->texture_coordinates[1] =
				(ch.y + ch.height) / tex_height;
			glm_vec4_copy(color, vertex_cursor->color);
			vertex_cursor++;

			vertex_cursor->position[0] = cursor[0] + xoffset;
			vertex_cursor->position[1] =
				cursor[1] + height + yoffset;
			vertex_cursor->texture_coordinates[0] =
				ch.x / tex_width;
			vertex_cursor->texture_coordinates[1] =
				(ch.y + ch.height) / tex_height;
			glm_vec4_copy(color, vertex_cursor->color);
			vertex_cursor++;

			renderer->cursor = vertex_cursor;

			cursor[0] += ch.xadvance * scale;

			renderer->drawing_chars_len++;
			if (renderer->drawing_chars_len >= MAX_CHARACTERS) {
				break;
			}
		}

		last_char = c;
		i++;
		c = text[i];
	}
	if (end_of_text != NULL) {
		glm_vec2_copy(cursor, end_of_text);
	}
}

void draw_textf(struct text_renderer *renderer, vec2 position, float scale,
		vec4 color, vec2 end_of_text, const wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	wchar_t buffer[512];
	vswprintf(buffer, 512, fmt, args);
	va_end(args);
	draw_text(renderer, buffer, position, scale, color, end_of_text);
}

void render_text(struct text_renderer *renderer, mat4 projection)
{
	bind_vao(renderer->vertex_array);

	update_vbo(renderer->vertex_buffer, 0,
		   MAX_VERTICES * sizeof(struct character_vertex),
		   renderer->vertices);

	use_shader(renderer->program);
	shader_set_mat4(renderer->program, "projection", projection);
	bind_texture(renderer->font_texture, 0);

	glDrawElements(GL_TRIANGLES, renderer->drawing_chars_len * 6,
		       GL_UNSIGNED_INT, NULL);

	renderer->drawing_chars_len = 0;
	renderer->cursor = renderer->vertices;
}
