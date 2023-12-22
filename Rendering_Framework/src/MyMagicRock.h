#pragma once

#include "DynamicSceneObject.h"

#include <iostream>
#include <cstring>
#include <vector>
#include "stb_image.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

using namespace std;

class MyMagicRock
{
public:
	MyMagicRock();
	virtual ~MyMagicRock();

	void Init();
	void loadTexture(const string &path, GLuint &handle);
	DynamicSceneObject *sceneObject() const;


public:
	void updateDataBuffer();
	void updateState(const glm::mat4 &viewMat, const glm::vec3 &viewPos);
	void render();
private:
	DynamicSceneObject *m_dynamicSO;
	int m_totalDataByte;
	GLuint albedo_handle;
	GLuint normal_handle;
};

