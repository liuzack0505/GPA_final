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

	glEnable(GL_DEPTH_TEST);

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
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, this->texture[i], 0);
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


	// vao
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	// Draw Buffers
	const int NUM_ATTACHMENTS = GBO_NUM_TEXTURES - 1;
	GLenum attachments[NUM_ATTACHMENTS] = {};
	for (int i = 0; i < NUM_ATTACHMENTS; i++) {
		attachments[i] = GL_COLOR_ATTACHMENT1 + i;
	}
	glDrawBuffers(NUM_ATTACHMENTS, attachments);

	auto gboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (gboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "GB::ERROR: " << gboStatus << std::endl;
	}

	// Unbind the framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	Shader* vshader = new Shader(GL_VERTEX_SHADER);
	vshader->createShaderFromFile("src/shader/oglVertexShader2.glsl");
	cout << vshader->shaderInfoLog() << "\n";

	Shader* fshader = new Shader(GL_FRAGMENT_SHADER);
	fshader->createShaderFromFile("src/shader/oglFragmentShader2.glsl");
	cout << fshader->shaderInfoLog() << "\n";

	//create reset shader
	this->deferShader = new ShaderProgram();
	this->deferShader->init();
	this->deferShader->attachShader(vshader);
	this->deferShader->attachShader(fshader);
	this->deferShader->checkStatus();
	if (this->deferShader->status() != ShaderProgramStatus::READY) {
		cout << "defer shader error\n";
	}
	this->deferShader->linkProgram();

	vshader->releaseShader();
	fshader->releaseShader();

	delete vshader;
	delete fshader;
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
	glEnable(GL_DEPTH_TEST);
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
	glReadBuffer(GL_COLOR_ATTACHMENT1 + static_cast<int>(texType));
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

void GBO::use() {
	this->deferShader->useProgram();
}

void GBO::bindTexture() {
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < GBO_NUM_TEXTURES; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, this->texture[i]);
		//glUniform1i(uniforms.deferred.position_map, i);
	}
}

void GBO::bindDrawBuffer() {
	GLenum attachments[5] = {};
	for (int i = 0; i < 5; i++) {
		attachments[i] = GL_COLOR_ATTACHMENT1 + i;
	}
	glDrawBuffers(5, attachments);
}