#ifndef BIRPLOT_SHADER_H
#define BIRPLOT_SHADER_H

#include <cglm/types.h>
#include <stdint.h>

void load_shader_memory(uint32_t *program, const char *vertex_shader_code,
			const char *fragment_shader_code);
void load_shader_file(uint32_t *program, const char *vertex_shader_file,
		      const char *fragment_shader_file);
void reload_shader_memory(uint32_t *program, const char *vertex_shader_code,
			const char *fragment_shader_code);
void reload_shader_file(uint32_t *program, const char *vertex_shader_file,
			const char *fragment_shader_file);

void use_shader(uint32_t program);

void shader_set_int(uint32_t program, const char *name, int i);
void shader_set_float(uint32_t program, const char *name, float f);
void shader_set_vec2(uint32_t program, const char *name, vec2 v);
void shader_set_vec3(uint32_t program, const char *name, vec3 v);
void shader_set_vec4(uint32_t program, const char *name, vec4 v);
void shader_set_mat4(uint32_t program, const char *name, mat4 m);

#endif
