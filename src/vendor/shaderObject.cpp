#include "shaderObject.hpp"

#include "shader.hpp"


GLvoid basicShaderObject::init(GLchar* vertex_shader_name, GLchar* fragment_shader_name) {
	vertex_shader = vertex_shader_name;
	fragment_shader = fragment_shader_name;
	GLuint vert_sh = compileShader(vertex_shader, GL_VERTEX_SHADER);
	GLuint frag_sh = compileShader(fragment_shader, GL_FRAGMENT_SHADER);
	shader_program = linkProgram(vert_sh, frag_sh);
	glDeleteShader(frag_sh);
	glDeleteShader(vert_sh);

	glUseProgram(shader_program);
}

GLvoid basicShaderObject::use() {
	glUseProgram(shader_program);
}


GLvoid geometryShaderObject::init(GLchar* vertex_shader_name, GLchar* fragment_shader_name) {
	vertex_shader = vertex_shader_name;
	fragment_shader = fragment_shader_name;
	GLuint vert_sh = compileShader(vertex_shader, GL_VERTEX_SHADER);
	GLuint frag_sh = compileShader(fragment_shader, GL_FRAGMENT_SHADER);
	shader_program = linkProgram(vert_sh, frag_sh);
	glDeleteShader(frag_sh);
	glDeleteShader(vert_sh);

	glUseProgram(shader_program);

	model_mat_loc = glGetUniformLocation(shader_program, "model_mat");
	view_mat_loc = glGetUniformLocation(shader_program, "view_mat");
	proj_mat_loc = glGetUniformLocation(shader_program, "proj_mat");
}

GLvoid geometryShaderObject::use(glm::mat4 view_matrix, glm::mat4 proj_matrix, glm::mat4 model_matrix) {
	glUseProgram(shader_program);

	glUniformMatrix4fv(view_mat_loc, 1, GL_FALSE, &view_matrix[0][0]);
	glUniformMatrix4fv(proj_mat_loc, 1, GL_FALSE, &proj_matrix[0][0]);
	glUniformMatrix4fv(model_mat_loc, 1, GL_FALSE, &model_matrix[0][0]);
}


GLvoid composeShaderObject::init(GLchar* vertex_shader_name, GLchar* fragment_shader_name) {
	vertex_shader = vertex_shader_name;
	fragment_shader = fragment_shader_name;
	GLuint vert_sh = compileShader(vertex_shader, GL_VERTEX_SHADER);
	GLuint frag_sh = compileShader(fragment_shader, GL_FRAGMENT_SHADER);
	compose_shader_program = linkProgram(vert_sh, frag_sh);
	glDeleteShader(frag_sh);
	glDeleteShader(vert_sh);

	glUseProgram(compose_shader_program);

	normal_depth_tex_loc = glGetUniformLocation(compose_shader_program, "normal_depth_tex");
	diffuse_specular_tex_loc = glGetUniformLocation(compose_shader_program, "diffuse_specular_tex");
	emitting_shininess_tex_loc = glGetUniformLocation(compose_shader_program, "emitting_shininess_tex");
	bloom_tex_loc = glGetUniformLocation(compose_shader_program, "bloom_tex");

	global_light_pos_loc = glGetUniformLocation(compose_shader_program, "light_pos");
	proj_mat_loc = glGetUniformLocation(compose_shader_program, "proj_mat");
	view_mat_loc = glGetUniformLocation(compose_shader_program, "view_mat");
	inv_view_mat_loc = glGetUniformLocation(compose_shader_program, "inv_view_mat");
	half_size_near_loc = glGetUniformLocation(compose_shader_program, "half_size_near");
}

GLvoid composeShaderObject::use(GLuint normal_depth_tex, GLuint diffuse_specular_tex, GLuint emitting_shininess_tex, GLuint bloom_tex, glm::vec3 global_light_pos, glm::mat4 proj_mat, glm::mat4 view_mat, glm::vec2 half_size_near) {
	glUseProgram(compose_shader_program);

	glBindTextureUnit(0, normal_depth_tex);
	glUniform1i(normal_depth_tex_loc, 0);
	glBindTextureUnit(1, diffuse_specular_tex);
	glUniform1i(diffuse_specular_tex_loc, 1);
	glBindTextureUnit(2, emitting_shininess_tex);
	glUniform1i(emitting_shininess_tex_loc, 2);
	glBindTextureUnit(3, bloom_tex);
	glUniform1i(bloom_tex_loc, 3);

	glUniform3fv(global_light_pos_loc, 1, &global_light_pos[0]);
	glUniformMatrix4fv(view_mat_loc, 1, GL_FALSE, &view_mat[0][0]);
	glm::mat4 inv_view_mat = glm::inverse(view_mat);
	glUniformMatrix4fv(inv_view_mat_loc, 1, GL_FALSE, &inv_view_mat[0][0]);
	glUniformMatrix4fv(proj_mat_loc, 1, GL_FALSE, &proj_mat[0][0]);
	glUniform2fv(half_size_near_loc, 1, &half_size_near[0]);
}

GLvoid finalShaderObject::init(GLchar* vertex_shader_name, GLchar* fragment_shader_name) {
	vertex_shader = vertex_shader_name;
	fragment_shader = fragment_shader_name;
	GLuint vert_sh = compileShader(vertex_shader, GL_VERTEX_SHADER);
	GLuint frag_sh = compileShader(fragment_shader, GL_FRAGMENT_SHADER);
	final_shader_program = linkProgram(vert_sh, frag_sh);
	glDeleteShader(frag_sh);
	glDeleteShader(vert_sh);

	glUseProgram(final_shader_program);

	normal_depth_tex_loc = glGetUniformLocation(final_shader_program, "normal_depth_tex");
	diffuse_specular_tex_loc = glGetUniformLocation(final_shader_program, "diffuse_specular_tex");
	emitting_shininess_tex_loc = glGetUniformLocation(final_shader_program, "emitting_shininess_tex");
	final_tex_loc = glGetUniformLocation(final_shader_program, "final_tex");

	proj_mat_loc = glGetUniformLocation(final_shader_program, "proj_mat");
	view_mat_loc = glGetUniformLocation(final_shader_program, "view_mat");
	inv_view_mat_loc = glGetUniformLocation(final_shader_program, "inv_view_mat");
	half_size_near_loc = glGetUniformLocation(final_shader_program, "half_size_near");
}

GLvoid finalShaderObject::use(GLuint normal_depth_tex, GLuint diffuse_specular_tex, GLuint emitting_shininess_tex, GLuint final_tex, glm::mat4 proj_mat, glm::mat4 view_mat, glm::vec2 half_size_near) {
	glUseProgram(final_shader_program);

	glBindTextureUnit(0, normal_depth_tex);
	glUniform1i(normal_depth_tex_loc, 0);
	glBindTextureUnit(1, diffuse_specular_tex);
	glUniform1i(diffuse_specular_tex_loc, 1);
	glBindTextureUnit(2, emitting_shininess_tex);
	glUniform1i(emitting_shininess_tex_loc, 2);
	glBindTextureUnit(3, final_tex);
	glUniform1i(final_tex_loc, 3);

	glUniformMatrix4fv(view_mat_loc, 1, GL_FALSE, &view_mat[0][0]);
	glm::mat4 inv_view_mat = glm::inverse(view_mat);
	glUniformMatrix4fv(inv_view_mat_loc, 1, GL_FALSE, &inv_view_mat[0][0]);
	glUniformMatrix4fv(proj_mat_loc, 1, GL_FALSE, &proj_mat[0][0]);
	glUniform2fv(half_size_near_loc, 1, &half_size_near[0]);
}