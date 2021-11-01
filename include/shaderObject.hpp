#pragma once

#include "common.hpp"

struct basicShaderObject
{
	GLvoid init(GLchar *vertex_shader_name, GLchar *fragment_shader_name);
	GLvoid use();

	GLchar *vertex_shader;
	GLchar *fragment_shader;

	GLuint shader_program;
};

struct geometryShaderObject
{
	GLvoid init(GLchar *vertex_shader_name, GLchar *fragment_shader_name);
	GLvoid use(glm::mat4 view_matrix, glm::mat4 proj_matrix, glm::mat4 model_matrix);

	GLchar *vertex_shader;
	GLchar *fragment_shader;

	GLuint shader_program;

	GLuint view_mat_loc;
	GLuint proj_mat_loc;
	GLuint model_mat_loc;
};

struct composeShaderObject
{
	GLvoid init(GLchar *vertex_shader_name, GLchar *fragment_shader_name);
	GLvoid use(GLuint normal_depth_tex, GLuint diffuse_specular_tex, GLuint emitting_shininess_tex, GLuint bloom_tex, glm::vec3 global_light_pos, glm::mat4 proj_mat, glm::mat4 view_mat, glm::vec2 half_size_near);

	GLchar *vertex_shader;
	GLchar *fragment_shader;

	GLuint compose_shader_program;

	GLuint normal_depth_tex_loc;
	GLuint diffuse_specular_tex_loc;
	GLuint emitting_shininess_tex_loc;
	GLuint bloom_tex_loc;

	GLuint global_light_pos_loc;
	GLuint proj_mat_loc;
	GLuint view_mat_loc;
	GLuint inv_view_mat_loc;
	GLuint half_size_near_loc;
};

struct finalShaderObject
{
	GLvoid init(GLchar *vertex_shader_name, GLchar *fragment_shader_name);
	GLvoid use(GLuint normal_depth_tex, GLuint diffuse_specular_tex, GLuint emitting_shininess_tex, GLuint final_tex, glm::mat4 proj_mat, glm::mat4 view_mat, glm::vec2 half_size_near);

	GLchar *vertex_shader;
	GLchar *fragment_shader;

	GLuint final_shader_program;

	GLuint normal_depth_tex_loc;
	GLuint diffuse_specular_tex_loc;
	GLuint emitting_shininess_tex_loc;
	GLuint final_tex_loc;

	GLuint proj_mat_loc;
	GLuint view_mat_loc;
	GLuint inv_view_mat_loc;
	GLuint half_size_near_loc;
};