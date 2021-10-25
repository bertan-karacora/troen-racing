#pragma once

#include "common.hpp"

struct camera {
	camera(GLFWwindow* window);
	virtual ~camera();

	GLvoid update(glm::vec3 target);

	glm::mat4 viewMat() const;
	glm::vec3 position() const;
};
