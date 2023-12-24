#pragma once

#include <glm\mat4x4.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glad\glad.h>
#include <string>
#include <iostream>
#include "Shader.h"

using namespace std;

enum GBOTextureType {
	GBO_TEXTURE_TYPE_VERTEX = 0,
	GBO_TEXTURE_TYPE_NORMAL = 1,
	GBO_TEXTURE_TYPE_DIFFUSE = 2,
	GBO_TEXTURE_TYPE_AMBIENT = 3,
	GBO_TEXTURE_TYPE_SPECULAR = 4,
	GBO_TEXTURE_TYPE_DEPTH = 5,
	GBO_NUM_TEXTURES = 6
};

class GBO {
private:
	GLuint gbo;
	GLuint texture[GBO_NUM_TEXTURES];
	std::string texType[GBO_NUM_TEXTURES] = {
		"COORDS", "COORDS", "COLORS", "COLORS", "COLORS", "DEPTH"
	};
	int width;
	int height;

public:
	GBO(int w, int h);
	~GBO();

	void init();
	void init(int w, int h);
	void resize(int new_w, int new_h);
	void bindWrite();
	void bindRead();
	GLuint getTexture(GBOTextureType texType);
	void setReadBuffer(GBOTextureType texType);
	static void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
	}

	void use();
	void bindTexture();

	GLuint vao;

	void bindDrawBuffer();

	ShaderProgram* deferShader = nullptr;
private:
	void release();



	
};