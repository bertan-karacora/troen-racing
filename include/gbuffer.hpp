#pragma once

#include "common.hpp"

class gbuffer {
	public:
		gbuffer();
		~gbuffer();

		GLvoid init(GLuint width, GLuint height);

		GLuint fbo;
		GLuint normal_depth_tex;
		GLuint diffuse_specular_tex;
		GLuint emitting_shininess_tex;
		GLuint depth_rbo;

		// Ping-Pong buffers for bloom
		GLuint ping_fbo;
		GLuint ping_tex;
		GLuint pong_fbo;
		GLuint pong_tex;

		GLuint final_fbo;
		GLuint final_tex;
	private:
		GLuint width;
		GLuint height;

		GLuint create_texture_rgba32f();
		GLvoid del();
};