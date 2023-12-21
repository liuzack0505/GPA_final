#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Rendering_Framework\src\SceneManager.h>

class AirplaneSceneObject
{
public:
	AirplaneSceneObject();
	virtual ~AirplaneSceneObject();

	void update();

public:
	//bool viewFrustumCullingTest(const glm::vec4* frustumPlaneEquations);

public:
	//void initializeChunkGeometry(const float* chunkVertices, const int numChunkVertex, const unsigned int* chunkIndices, const int numIndex);
	//void setChunkBoundingVolume(const int idx, IBoundingVolume* bv);
	//void setChunkTransformMatrix(const int idx, const glm::mat4& m);
	//void setWorldVertexToElevationMapUVMatrix(const glm::mat4& m);
	//void setElevationTextureHandle(const GLuint texHandle);
	//void setNormalTextureHandle(const GLuint texHandle);
	//void setAlbedoTextureHandle(const GLuint texHandle);
	void setVAO(const GLuint handle);
	void setVBOvertex(const GLuint handle);
	void setVBOtexcoord(const GLuint handle);
	void setVBOnormal(const GLuint handle);
	void setIBO(const GLuint handle);
	void setDrawCount(const int count);
	void setModelMat(const glm::mat4 modelMat);

private:
	//const int m_numChunk;
	//glm::mat4* m_chunkModelMats = nullptr;
	//IBoundingVolume** m_chunkBoundingVolumes = nullptr;
	//bool* m_chunkVisibilityFlags = nullptr;
	glm::mat4 m_worldVertexToElevationMapUvMat;

	//int m_numIndex;

	GLuint m_evelationMapHandle;
	GLuint m_normalMapHandle;
	GLuint m_albedoMapHandle;
	//GLuint m_vao;

	GLuint m_vaoHandle = 0u;
	GLuint m_vertexBufferHandle_vertex = 0u;
	GLuint m_vertexBufferHandle_texcoord = 0u;
	GLuint m_vertexBufferHandle_normal = 0u;
	GLuint m_iboHandle = 0u;

	int airplaneDrawCount = 0;

	glm::mat4 m_airplaneModelMat = glm::mat4(1.0);

};

