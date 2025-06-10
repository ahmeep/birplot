#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#include "gl.h"

#include "utils/file.h"

int check_shader_compilation(uint32_t shader, const char *shader_type)
{
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		char log[len];
		glGetShaderInfoLog(shader, len, &len, log);

		fprintf(stderr,
			"An error occurred while compiling %s shader: \n%s\n",
			shader_type, log);

		glDeleteShader(shader);
		return 1;
	}
	return 0;
}

int check_program_link(uint32_t program)
{
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		char log[len];
		glGetProgramInfoLog(program, len, &len, log);

		fprintf(stderr,
			"An error occurred while linking shader program: \n%s\n",
			log);

		glDeleteProgram(program);
		return 1;
	}
	return 0;
}

void load_shader_memory(uint32_t *program, const char *vertex_shader_code,
			const char *fragment_shader_code)
{
	uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
	glCompileShader(vertex_shader);

	if (check_shader_compilation(vertex_shader, "vertex shader") != 0) {
		return;
	}

	uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
	glCompileShader(fragment_shader);

	if (check_shader_compilation(fragment_shader, "fragment shader") != 0) {
		// also delete the vertex shader!
		glDeleteShader(vertex_shader);
		return;
	}

	uint32_t created_program = glCreateProgram();
	glAttachShader(created_program, vertex_shader);
	glAttachShader(created_program, fragment_shader);
	glLinkProgram(created_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	if (check_program_link(created_program) != 0) {
		return;
	}
	*program = created_program;
}

void load_shader_file(uint32_t *program, const char *vertex_shader_file,
		      const char *fragment_shader_file)
{
	char *vertex_shader_code, *fragment_shader_code;
	read_file(vertex_shader_file, &vertex_shader_code, NULL);
	read_file(fragment_shader_file, &fragment_shader_code, NULL);
	load_shader_memory(program, vertex_shader_code, fragment_shader_code);
	free(vertex_shader_code);
	free(fragment_shader_code);
}

void reload_shader_memory(uint32_t *program, const char *vertex_shader_code,
			  const char *fragment_shader_code)
{
	uint32_t new_program;
	load_shader_memory(&new_program, vertex_shader_code,
			   fragment_shader_code);

	if (new_program != 0) {
		glDeleteProgram(*program);
		*program = new_program;
	} else {
		*program = 0;
	}
}

void reload_shader_file(uint32_t *program, const char *vertex_shader_file,
			const char *fragment_shader_file)
{
	char *vertex_shader_code, *fragment_shader_code;
	read_file(vertex_shader_file, &vertex_shader_code, NULL);
	read_file(fragment_shader_file, &fragment_shader_code, NULL);
	reload_shader_memory(program, vertex_shader_code, fragment_shader_code);
	free(vertex_shader_code);
	free(fragment_shader_code);
}

void use_shader(uint32_t program)
{
	glUseProgram(program);
}

void shader_set_int(uint32_t program, const char *name, int i)
{
	glUniform1i(glGetUniformLocation(program, name), i);
}

void shader_set_float(uint32_t program, const char *name, float f)
{
	glUniform1f(glGetUniformLocation(program, name), f);
}

void shader_set_vec2(uint32_t program, const char *name, vec2 v)
{
	glUniform2fv(glGetUniformLocation(program, name), 1, v);
}

void shader_set_vec3(uint32_t program, const char *name, vec3 v)
{
	glUniform3fv(glGetUniformLocation(program, name), 1, v);
}

void shader_set_vec4(uint32_t program, const char *name, vec4 v)
{
	glUniform4fv(glGetUniformLocation(program, name), 1, v);
}

void shader_set_mat4(uint32_t program, const char *name, mat4 m)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE,
			   (float *)m);
}
