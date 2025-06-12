#define GLAD_GL_IMPLEMENTATION
#include "gl.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cglm/cglm.h>

#include "utils/data.h"
#include "utils/shader.h"
#include "utils/text.h"
#include "utils/file.h"

int width = 800, height = 600;
GLFWwindow *window;
mat4 projection;
double dt, last;

uint32_t plot_shader;
uint32_t cartesian_shader;
int shaders_should_update;
const char *updated_file;

uint32_t INSTANCE_COUNT = 1;

int FPS_ENABLED = 1;
int DRAW_COORDINATE_SYSTEM = 1;

void shader_file_updated_callback(const char *updated)
{
	updated_file = updated;
	shaders_should_update = 1;
}


void key_callback(GLFWwindow *window, int key, int scancode, int action,
		  int mods)
{
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		FPS_ENABLED *= -1;
	}
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		DRAW_COORDINATE_SYSTEM *= -1;
	}
}

void load_plot_shader();

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, "birplot", NULL, NULL);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// a triangle big enough that covers entire screen.
	float vertices[] = {-1.0f, -1.0f, 4.0f, -1.0f, -1.0f, 4.0f};

	struct vbo vertex_buffer = generate_vbo((uint32_t[]){FL_FLOAT, 2}, 1);
	struct ebo dummy_index_buffer = generate_ebo();
	struct vao vertex_array =
		generate_vao(&vertex_buffer, 1, dummy_index_buffer);

	allocate_vbo(vertex_buffer, sizeof(vertices), vertices, AO_IMMUTABLE);

	load_plot_shader();
	watch_file("assets/shaders/plot.frag", shader_file_updated_callback);
	watch_file("birplot.glsl", shader_file_updated_callback);
	if (plot_shader == 0) {
		printf("an error occurred while loading plot shaders!\n");
		return 1;
	}

	cartesian_shader = 0;
	load_shader_file(&cartesian_shader, "assets/shaders/plot.vert",
			 "assets/shaders/cartesian.frag");

	struct text_renderer helvetica_text = {};
	create_text_renderer(&helvetica_text, "assets/fonts/helvetica.txt",
			     "assets/fonts/helvetica.png");

	// glfwSwapInterval(0);
	double update_notice = 0;

	while (!glfwWindowShouldClose(window)) {
		if (shaders_should_update) {
			load_plot_shader();
			reload_shader_file(&cartesian_shader,
					   "assets/shaders/plot.vert",
					   "assets/shaders/cartesian.frag");
			shaders_should_update = 0;
			update_notice = 3.0;
		}
		double time = glfwGetTime();
		dt = time - last;
		last = time;

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, projection);

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		bind_vao(vertex_array);

		if (DRAW_COORDINATE_SYSTEM == 1) {
			use_shader(cartesian_shader);
			shader_set_int(cartesian_shader, "width", width);
			shader_set_int(cartesian_shader, "height", height);
			shader_set_float(cartesian_shader, "time", time);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		use_shader(plot_shader);
		shader_set_int(plot_shader, "width", width);
		shader_set_int(plot_shader, "height", height);
		shader_set_float(plot_shader, "time", time);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, INSTANCE_COUNT);

		vec2 end = {0.0f, 0.0f};
		if (FPS_ENABLED == 1) {
			draw_textf(&helvetica_text, (vec2){10.0f, 10.0f}, 0.30f,
				   (vec4){1.0f, 1.0f, 1.0f, 1.0f}, end,
				   L"FPS: %f", 1 / dt);
		}

		if (update_notice > 0) {
			update_notice -= dt;
			draw_textf(&helvetica_text,
				   (vec2){end[0] + 10.0f, 10.0f}, 0.30f,
				   (vec4){1.0f, 0.0f, 0.0f, 1.0f}, end,
				   L"shader file %s updated", updated_file);
		}
		render_text(&helvetica_text, projection);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	free_text_renderer(&helvetica_text);
	glfwDestroyWindow(window);
	glfwTerminate();
}


void load_plot_shader()
{
	char *vertex_shader_code, *fragment_shader_code, *user_code;
	size_t fragment_shader_code_size, user_code_size;
	read_file("assets/shaders/plot.vert", &vertex_shader_code, NULL);
	read_file("assets/shaders/plot.frag", &fragment_shader_code,
		  &fragment_shader_code_size);
	read_file("birplot.glsl", &user_code, &user_code_size);

	size_t written = 0;
	size_t total_size = fragment_shader_code_size + user_code_size;
	char *merged_code = calloc(total_size + 1, sizeof(char));

	char *fragment_shader_cursor = fragment_shader_code;
	char *line =
		strtok_r(fragment_shader_code, "\n", &fragment_shader_cursor);
	while (line != NULL) {
		size_t line_size = strlen(line);
		if (strcmp(line, "#USERDEFINEDHERE") == 0) {
			break;
		}

		memcpy(merged_code + written, line, line_size);
		written += line_size;
		merged_code[written] = '\n';
		written += 1;

		line = strtok_r(NULL, "\n", &fragment_shader_cursor);
	}

	line = strtok(user_code, "\n");
	while (line != NULL) {
		size_t line_size = strlen(line);
		if (line_size > 31
		    && strncmp(line, "#define BIRPLOT_INSTANCE_COUNT ", 31)
			       == 0) {
			INSTANCE_COUNT = atoi(line + 31);
		}

		memcpy(merged_code + written, line, line_size);
		written += line_size;
		merged_code[written] = '\n';
		written += 1;

		line = strtok(NULL, "\n");
	}

	memcpy(merged_code + written, fragment_shader_cursor,
	       strlen(fragment_shader_cursor));

	if (plot_shader != 0) {
		reload_shader_memory(&plot_shader, vertex_shader_code,
				     merged_code);
	} else {
		load_shader_memory(&plot_shader, vertex_shader_code,
				   merged_code);
	}


	free(merged_code);
	free(user_code);
	free(vertex_shader_code);
	free(fragment_shader_code);
}
