#pragma once

#include "../DynamicSceneObject.h"
#include "../MyCameraManager.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtx\transform.hpp>
#include <glad\glad.h>

#include <stb_image.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <iostream>
#include <cstring>
#include <vector>
#include "stb_image.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

typedef struct {
	int width;
	int height;
	unsigned char* data;
} TextureData;

typedef struct {
	GLuint diffuse_tex;
} Material;

typedef struct {
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	int drawCount;
	int materialID;
} Shape;

using namespace std;

class MyAirplane
{
public:
	MyAirplane();
	virtual ~MyAirplane();

	void Init(INANOA::MyCameraManager* mcm);
	void loadTexture(const string& path, GLuint& handle);
	DynamicSceneObject* sceneObject() const;

public:
	//void updateState(const glm::mat4& viewMat, const glm::vec3& viewOrg, const glm::mat4& projMat, const glm::vec4* frustumPlaneEquations);

public:
	void updateDataBuffer();
	void updateState(const glm::mat4& viewMat, const glm::vec3& viewPos);
	void render();
	void getModelMatFromCameraManager(INANOA::MyCameraManager* mcm);

private:
	DynamicSceneObject* m_dynamicSO;
	int m_totalDataByte;
	GLuint albedo_handle;
	GLuint normal_handle;
	INANOA::MyCameraManager* MCM = nullptr;
	glm::mat4 modelMat;
};

