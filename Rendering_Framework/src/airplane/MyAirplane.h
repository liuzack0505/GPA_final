#pragma once

#include "AirplaneSceneObject.h"

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
#include <vector>

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

public:
	void init();
	void render();
	TextureData loadImage(string path);
	void setupAirplaneSceneObject();

public:
	//void updateState(const glm::mat4& viewMat, const glm::vec3& viewOrg, const glm::mat4& projMat, const glm::vec4* frustumPlaneEquations);

public:
	//TerrainSceneObject* sceneObject();
	AirplaneSceneObject* sceneObject();
public:
	//const glm::mat4 worldVtoElevationUVMat() const;
	//const MyTerrainData* terrainData() const;

private:
	//TerrainSceneObject* m_terrainSO = nullptr;
	//MyTerrainData* m_terrainData = nullptr;


	//glm::mat4 m_worldVtoElevationUVMat;

	vector<Material> airplaneMaterails;
	vector<Shape> airplaneShapes;
	Assimp::Importer airplaneImporter;

	GLuint m_vaoHandle_ = 0u;
	GLuint m_vertexBufferHandle_vertex_ = 0u;
	GLuint m_vertexBufferHandle_texcoord_ = 0u;
	GLuint m_vertexBufferHandle_normal_ = 0u;
	GLuint m_iboHandle_ = 0u;
	int airplaneDrawCount_ = 0;

	AirplaneSceneObject* m_airplaneSO = nullptr;
	
};

