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

	plot_shader = 0;
	load_shader_file(&plot_shader, "assets/shaders/plot.vert",
			 "assets/shaders/plot.frag");
	if (plot_shader == 0) {
		printf("an error occurred while loading plot shaders!\n");
		return 1;
	}
	watch_file("assets/shaders/plot.vert", shader_file_updated_callback);
	watch_file("assets/shaders/plot.frag", shader_file_updated_callback);

	cartesian_shader = 0;
	load_shader_file(&cartesian_shader, "assets/shaders/plot.vert",
			 "assets/shaders/cartesian.frag");
	watch_file("assets/shaders/cartesian.frag",
		   shader_file_updated_callback);

	struct text_renderer helvetica_text = {};
	create_text_renderer(&helvetica_text, "assets/fonts/helvetica.txt",
			     "assets/fonts/helvetica.png");

	// glfwSwapInterval(0);
	double update_notice = 0;

	while (!glfwWindowShouldClose(window)) {
		if (shaders_should_update) {
			reload_shader_file(&plot_shader,
					   "assets/shaders/plot.vert",
					   "assets/shaders/plot.frag");
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
		glDrawArrays(GL_TRIANGLES, 0, 3);

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
