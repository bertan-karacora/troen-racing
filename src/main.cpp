#include "common.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "buffer.hpp"
#include "camera.hpp"
#include "moto.hpp"
#include "gbuffer.hpp"
#include "shaderObject.hpp"
#include <fstream>
#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib")

const GLint WINDOW_WIDTH = 1920;
const GLint WINDOW_HEIGHT = 1080;
const GLint FPS = 30;

const GLfloat FOV = 90.0;
const GLfloat NEAR_VALUE = 0.1;
const GLfloat FAR_VALUE = 100.0;

glm::mat4 proj_mat;
glm::vec2 half_size_near;
GLFWwindow* window;
gbuffer gbuf;
irrklang::ISoundEngine* engine;
moto player_moto(window);

GLvoid resizeCallback(GLFWwindow*, GLint width, GLint height) {
	glViewport(0, 0, width, height);
	proj_mat = glm::perspective(FOV, GLfloat(width) / GLfloat(height), NEAR_VALUE, FAR_VALUE);
	gbuf.init(width, height);

	GLfloat aspect = GLfloat(width) / GLfloat(height);
	half_size_near.x = aspect * half_size_near.y;
}

GLvoid keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_M && action == GLFW_PRESS) engine->play2D("20210716_210942_Tron_Impro.mp3", true);
	player_moto.input(key, action);
}

GLuint makeFullscreenQuad() {
	GLfloat vertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f
	};

	GLuint indices[] = {
		0, 1, 2, 2, 3, 0
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO = makeBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	GLuint IBO = makeBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	return VAO;
}


GLint main(GLint, GLchar* argv[]) {
	// INIT
	window = initOpenGL(WINDOW_WIDTH, WINDOW_HEIGHT, argv[0]);
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	camera cam(window);
	gbuf.init(WINDOW_WIDTH, WINDOW_HEIGHT);
	proj_mat = glm::perspective(FOV, GLfloat(WINDOW_WIDTH) / GLfloat(WINDOW_HEIGHT), NEAR_VALUE, FAR_VALUE);
	half_size_near = glm::vec2(std::tan(0.5 * FOV), std::tan(0.5 * FOV));
	GLfloat aspect = GLfloat(WINDOW_WIDTH) / GLfloat(WINDOW_HEIGHT);
	half_size_near.x = aspect * half_size_near.y;
	engine = irrklang::createIrrKlangDevice();
	glm::vec3 global_light_pos;

	basicShaderObject blur_shader;
	blur_shader.init("blur.vert", "blur.frag");

	finalShaderObject final_shader;
	final_shader.init("final.vert", "final.frag");

	// Fullscreen quad
	GLuint quad = makeFullscreenQuad();
	composeShaderObject compose_shader;
	compose_shader.init("compose.vert", "compose.frag");


	// OBJECTS
	std::vector<geometry> objects;

	// Grid
	geometry grid = loadMesh("grid.obj", true);
	geometryShaderObject grid_shader;
	grid_shader.init("grid.vert", "grid.frag");
	objects.push_back(grid);

	// Bikes
	std::vector<geometry> moto = loadScene("moto.obj", true);
	geometryShaderObject moto_shader;
	moto_shader.init("moto.vert", "moto.frag");
	objects.insert(objects.end(), moto.begin(), moto.end());

	GLdouble last_time = glfwGetTime();
	GLdouble delta_time = 0.0;
	GLint current_FPS = 0;
	GLint tick = 0;

	FILE* output_image;
	std::vector<GLubyte> pixels(WINDOW_WIDTH * WINDOW_HEIGHT * 3);

	// RENDER LOOP
	while (glfwWindowShouldClose(window) == false) {
		// GEOMETRY PASS
		glBindFramebuffer(GL_FRAMEBUFFER, gbuf.fbo);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		player_moto.update();
		cam.update(player_moto.position() + glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 moto_mat = player_moto.modelMat();
		glm::mat4 view_mat = cam.viewMat();

		// Grid
		grid_shader.use(view_mat, proj_mat, grid.transform);
		grid.bind();
		glDrawElements(GL_TRIANGLES, grid.vertex_count, GL_UNSIGNED_INT, (GLvoid*)0);

		grid_shader.use(view_mat, proj_mat, glm::translate(grid.transform, glm::vec3(0.0, 200.0, 0.0)));
		grid.bind();
		glDrawElements(GL_TRIANGLES, grid.vertex_count, GL_UNSIGNED_INT, (GLvoid*)0);

		// Moto
		for (GLuint i = 0; i < moto.size(); ++i) {
			moto_shader.use(view_mat, proj_mat, moto_mat * moto[i].transform);
			glUniform1ui(glGetUniformLocation(moto_shader.shader_program, "material"), i);
			glUniform1ui(glGetUniformLocation(moto_shader.shader_program, "is_blue"), 1);
			moto[i].bind();
			glDrawElements(GL_TRIANGLES, moto[i].vertex_count, GL_UNSIGNED_INT, (GLvoid*)0);
		}
		moto_mat = glm::translate(glm::mat4(1.0), glm::vec3(-4.0, -2.0, 0.0)) * moto_mat;
		for (GLuint i = 0; i < moto.size(); ++i) {
			moto_shader.use(view_mat, proj_mat, moto_mat * moto[i].transform);
			glUniform1ui(glGetUniformLocation(moto_shader.shader_program, "material"), i);
			glUniform1ui(glGetUniformLocation(moto_shader.shader_program, "is_blue"), 0);
			moto[i].bind();
			glDrawElements(GL_TRIANGLES, moto[i].vertex_count, GL_UNSIGNED_INT, (GLvoid*)0);
		}

		// HDR BLOOM PASS
		glDisable(GL_DEPTH_TEST);
		bool first_iteration = true;
		blur_shader.use();
		for (GLuint i = 0; i < 5; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, gbuf.ping_fbo);
			glUniform1ui(glGetUniformLocation(blur_shader.shader_program, "horizontal"), 1);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? gbuf.emitting_shininess_tex : gbuf.pong_tex);
			glBindVertexArray(quad);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);
			if (first_iteration) first_iteration = false;

			glBindFramebuffer(GL_FRAMEBUFFER, gbuf.pong_fbo);
			glUniform1ui(glGetUniformLocation(blur_shader.shader_program, "horizontal"), 0);
			glBindTexture(GL_TEXTURE_2D, gbuf.ping_tex);
			glBindVertexArray(quad);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);
		}

		// LIGHTING PASS
		glBindFramebuffer(GL_FRAMEBUFFER, gbuf.final_fbo);
		glBindVertexArray(quad);

		for (GLint i = 0; i <= 0; i++) {
			global_light_pos = glm::vec3(i * 2.0, floor(player_moto.position().y / 100.0) * 100.0 + 50.0, 10.0);
			compose_shader.use(gbuf.normal_depth_tex, gbuf.diffuse_specular_tex, gbuf.emitting_shininess_tex, gbuf.pong_tex, global_light_pos, proj_mat, view_mat, half_size_near);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*) 0);
		}

		// SSR + POST PROCESSING PASS
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		final_shader.use(gbuf.normal_depth_tex, gbuf.diffuse_specular_tex, gbuf.emitting_shininess_tex, gbuf.final_tex, proj_mat, view_mat, half_size_near);
		glBindVertexArray(quad);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);

		//glGetTexImage(gbuf.record_tex, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
		std::string filename = "C:\\Users\\Bertan\\Desktop\\Frames\\" + std::to_string(tick) + ".ppm";
		std::ofstream out(filename, std::ios::binary);
		out << "P6 " << WINDOW_WIDTH << " " << WINDOW_HEIGHT << " 255\n";
		out.close();
		out.open(filename, std::ios::app | std::ios::binary | std::ios::out);
		out.write((const char*)pixels.data(), WINDOW_WIDTH * WINDOW_HEIGHT * 3);

		glfwSwapBuffers(window);
		glfwPollEvents();

		while (glfwGetTime() < last_time + 1.0 / FPS) {} // Wait
		delta_time = glfwGetTime() - last_time;
		current_FPS = int(1.0 / delta_time);
		last_time = glfwGetTime();
		printf("FPS: %i \n", current_FPS);
		tick++;
	}

	glDeleteFramebuffers(1, &gbuf.fbo);
	for (auto& obj : objects) obj.destroy();
	engine->drop();
	glfwTerminate();
}