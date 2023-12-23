#pragma once

#include "camera/MyCamera.h"
#include "SceneManager.h"
#include "MyPoissonSample.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glad/glad.h>
#include <iostream>
#include <cstring>
#include <vector>
#include "stb_image.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

using namespace std;
using namespace glm;

typedef struct {
	int drawCount;
	int offset;
} Model;

typedef struct {
	unsigned int count;
	unsigned int instanceCount;
	unsigned int firstIndex;
	unsigned int baseVertex;
	unsigned int baseInstance;
} DrawCommand;

typedef struct {
	glm::vec4 position;
} Sample;

typedef struct {
	glm::vec4 position;
	glm::vec4 extra_data;
} rawSample;

const int IMG_SIZE = 1024 * 1024 * 4;


class MyFoliages {
public:
	explicit MyFoliages();
	virtual ~MyFoliages();

public:
	virtual void init();
	void computeShader(const glm::mat4& viewMat, const glm::mat4& projMat);
	void resetShader();
	void render();

	bool initShader();
	void initBuffer();
	void loadSpacialSample();
	void initRawDataBuffer();
	void initSSBO();
	void initDrawCommands();

	Model* loadTextureModels(const string& model_path, const int model_num);
	unsigned char* loadTextureImage(const string& texture_path);

private:
	string grass_path;
	string bush_path1;
	string bush_path2;

	Model* Grass_model;
	Model* Bush_1_model;
	Model* Bush_2_model;
	Model* Building1;
	Model* Building2;
	glm::mat4 modelMat;

	vector<GLfloat> positionData;
	vector<GLfloat> normalData;
	vector<GLfloat> texcoordData;
	vector<unsigned int> indexData;

	MyPoissonSample* grass_sample;
	MyPoissonSample* bush_1_sample;
	MyPoissonSample* bush_2_sample;
	MyPoissonSample* building_1_sample;
	MyPoissonSample* building_2_sample;
	int GRASS_MAX_INSTANCE;
	int BUSH1_MAX_INSTANCE;
	int BUSH2_MAX_INSTANCE;
	int BUILDING1_MAX_INSTANCE;
	int BUILDING2_MAX_INSTANCE;//total num of sample

	rawSample* rawData;
	GLfloat* testData;
	glm::mat4* rotationData;

	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	GLuint rawDataSSBO;
	GLuint validDataSSBO;
	GLuint rotationDataSSBO;

	DrawCommand drawCommands[5];
	GLuint commandSSBO;

	unsigned char* Texture_data;
	GLuint texture3D;

	//shader
	ShaderProgram* resetShaderProgram;
	ShaderProgram* computeShaderProgram;
};




