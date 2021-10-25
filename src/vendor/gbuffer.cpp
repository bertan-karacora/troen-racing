#include <gbuffer.hpp>

gbuffer::gbuffer() {
	width = NULL;
	height = NULL;

	fbo = NULL;
	normal_depth_tex = NULL;
	diffuse_specular_tex = NULL;
	emitting_shininess_tex = NULL;
	depth_rbo = NULL;

	// Ping-Pong buffers for bloom
	ping_fbo = NULL;
	ping_tex = NULL;
	pong_fbo = NULL;
	pong_tex = NULL;

	final_fbo = NULL;
	final_tex = NULL;
}

gbuffer::~gbuffer() {
	del();
}

GLvoid gbuffer::init(GLuint w, GLuint h) {
	width = w;
	height = h;
	del();

	glEnable(GL_MULTISAMPLE);

	glCreateFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	normal_depth_tex = create_texture_rgba32f();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normal_depth_tex, 0);

	diffuse_specular_tex = create_texture_rgba32f();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffuse_specular_tex, 0);

	emitting_shininess_tex = create_texture_rgba32f();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, emitting_shininess_tex, 0);

	GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	glCreateRenderbuffers(1, &depth_rbo);
	glNamedRenderbufferStorage(depth_rbo, GL_DEPTH_COMPONENT32, width, height);
	glNamedFramebufferRenderbuffer(fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

	if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Incomplete FBO!");
		std::terminate();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Bloom setup
	glCreateFramebuffers(1, &ping_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ping_fbo);
	ping_tex = create_texture_rgba32f();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ping_tex, 0);

	if (glCheckNamedFramebufferStatus(ping_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Incomplete FBO!");
		std::terminate();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glCreateFramebuffers(1, &pong_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, pong_fbo);
	pong_tex = create_texture_rgba32f();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pong_tex, 0);

	if (glCheckNamedFramebufferStatus(pong_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Incomplete FBO!");
		std::terminate();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCreateFramebuffers(1, &final_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, final_fbo);
	final_tex = create_texture_rgba32f();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, final_tex, 0);

	if (glCheckNamedFramebufferStatus(final_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Incomplete FBO!");
		std::terminate();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint gbuffer::create_texture_rgba32f() {
	GLuint handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return handle;
}

GLvoid gbuffer::del() {
	if (fbo) glDeleteFramebuffers(1, &fbo);
	if (normal_depth_tex)	glDeleteTextures(1, &normal_depth_tex);
	if (diffuse_specular_tex) glDeleteTextures(1, &diffuse_specular_tex);
	if (emitting_shininess_tex)	glDeleteTextures(1, &emitting_shininess_tex);
	if (depth_rbo) glDeleteTextures(1, &depth_rbo);

	if (ping_fbo) glDeleteFramebuffers(1, &ping_fbo);
	if (ping_tex) glDeleteTextures(1, &ping_tex);
	if (pong_fbo) glDeleteFramebuffers(1, &pong_fbo);
	if (pong_tex) glDeleteTextures(1, &pong_tex);
	
	if (final_fbo) glDeleteFramebuffers(1, &final_fbo);
	if (final_tex) glDeleteTextures(1, &final_tex);
}