#pragma once

#include "common.hpp"

struct moto {
	moto(GLFWwindow* window);
	~moto();

	GLvoid input(GLint key, GLint action);
	GLvoid update();

	glm::mat4 modelMat();
	glm::vec3 position();
};
