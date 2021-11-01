#include <moto.hpp>

const GLint FPS = 30;

const GLfloat max_speed = 300.0;
const GLfloat max_angle = 15.0;
const GLfloat reversed_max_speed = max_speed * 0.5;

const GLfloat forward_acceleration = max_speed * 0.6 / FPS;
const GLdouble reversed_acceleration = reversed_max_speed * 0.6 / FPS;
const GLdouble rotation_speed = max_angle * 2.0 / FPS;

const GLdouble brake_factor = 0.3 / FPS;
const GLdouble straight_factor = 1.0 / FPS;

const GLfloat move_speed = 2.0;
const GLfloat slide_speed = 0.3;

struct moto_state
{
	bool w_down;
	bool a_down;
	bool s_down;
	bool d_down;

	glm::mat4 model_mat;
	glm::vec3 translate;
	GLdouble speed;
	GLfloat y_rotation_angle;
	GLfloat scale;
};

static moto_state *state;

GLvoid moto::update()
{
	state->translate += glm::vec3(slide_speed * state->y_rotation_angle / max_angle, move_speed * state->speed / max_speed, 0.0);
	if (state->translate.x >= 9.5)
		state->translate.x = 9.5;
	if (state->translate.x <= -9.5)
		state->translate.x = -9.5;
	if (state->translate.y >= 200.0)
		state->translate.y -= 200.0;

	state->speed -= state->speed * brake_factor;
	state->y_rotation_angle -= state->y_rotation_angle * straight_factor;

	if (state->w_down && state->speed < max_speed)
		state->speed += forward_acceleration;
	if (state->s_down && state->speed > -reversed_max_speed)
		state->speed -= reversed_acceleration;
	if (state->a_down && state->y_rotation_angle < max_angle)
		state->y_rotation_angle -= rotation_speed;
	if (state->d_down && state->y_rotation_angle > -max_angle)
		state->y_rotation_angle += rotation_speed;

	glm::mat4 translation_mat = glm::translate(glm::mat4(1.0), state->translate);
	glm::translate(translation_mat, glm::vec3(0.0, -1.0, 0.0));
	glm::mat4 rotation_mat = glm::rotate(glm::mat4(1.0), glm::radians(state->y_rotation_angle), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(state->scale));

	state->model_mat = translation_mat * rotation_mat * scale_mat;
}

GLvoid moto::input(GLint key, GLint action)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			state->w_down = true;
			break;
		case GLFW_KEY_S:
			state->s_down = true;
			break;
		case GLFW_KEY_A:
			state->a_down = true;
			break;
		case GLFW_KEY_D:
			state->d_down = true;
			break;
		}
	}
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			state->w_down = false;
			break;
		case GLFW_KEY_S:
			state->s_down = false;
			break;
		case GLFW_KEY_A:
			state->a_down = false;
			break;
		case GLFW_KEY_D:
			state->d_down = false;
			break;
		}
	}
}

moto::moto(GLFWwindow *window)
{
	state = new moto_state({});

	state->w_down = false;
	state->a_down = false;
	state->s_down = false;
	state->d_down = false;

	state->model_mat = glm::mat4(1.0);
	state->translate = glm::vec3(0.0, -90.0, 0.0);
	state->speed = 0.0;
	state->y_rotation_angle = 0.0;
	state->scale = 0.2;
	update();
}

moto::~moto()
{
	delete[] state;
}

glm::mat4 moto::modelMat()
{
	return state->model_mat;
}

glm::vec3 moto::position()
{
	return state->translate;
}
