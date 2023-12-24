#include "gbo.h"

GBO::GBO(int w, int h) {
	this->gbo = 0;
	this->width = w;
	this->height = h;
};

GBO::~GBO() {
	this->release();
}

void GBO::init() {
	this->init(this->width, this->height);
}

void GBO::init(int w, int h) {
	this->width = w;
	this->height = h;

	// FrameBuffer
	glGenFramebuffers(1, &this->gbo);
	glBindFramebuffer(GL_FRAMEBUFFER, this->gbo);

	// create textures
	glGenTextures(GBO_NUM_TEXTURES, this->texture);

	for (int i = 0; i < GBO_NUM_TEXTURES; i++) {
		glBindTexture(GL_TEXTURE_2D, this->texture[i]);

		if (texType[i] != "DEPTH") {
			if (texType[i] == "COORDS") {
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, w, h);
			}
			else if (texType[i] == "COLORS") {
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, this->texture[i], 0);
		}
		else {
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, w, h);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->texture[i], 0);
		}
	}

	// Draw Buffers
	const int NUM_ATTACHMENTS = GBO_NUM_TEXTURES - 1;
	GLenum attachments[NUM_ATTACHMENTS] = {};
	for (int i = 0; i < NUM_ATTACHMENTS; i++) {
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glDrawBuffers(NUM_ATTACHMENTS, attachments);

	auto gboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (gboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "GB::ERROR: " << gboStatus << std::endl;
	}

	// Unbind the framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBO::resize(int new_w, int new_h) {
	if (width == new_w && height == new_h)
		return;

	// Delete old framebuffer and texture
	release();

	// Reinitialize with the new size
	init(new_w, new_h);
}

void GBO::bindWrite() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->gbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBO::bindRead() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gbo);
}

GLuint GBO::getTexture(GBOTextureType texType) {
	return this->texture[static_cast<int>(texType)];
}

void GBO::setReadBuffer(GBOTextureType texType) {
	glReadBuffer(GL_COLOR_ATTACHMENT0 + static_cast<int>(texType));
}

void GBO::release() {
	if (this->texture) {
		for (int i = 0; i < GBO_NUM_TEXTURES; i++) {
			glDeleteTextures(1, &this->texture[i]);
			this->texture[i] = 0;
		}
	}

	if (this->gbo) {
		glDeleteFramebuffers(1, &this->gbo);
		this->gbo = 0;
	}
}