#include "AirplaneSceneObject.h"
#include "../MyCameraManager.h"

AirplaneSceneObject::AirplaneSceneObject() {}

AirplaneSceneObject::~AirplaneSceneObject() {}

void AirplaneSceneObject::setVAO(const GLuint handle) {
	this->m_vaoHandle = handle;
}

void AirplaneSceneObject::setVBOvertex(const GLuint handle) {
	this->m_vertexBufferHandle_vertex = handle;
}

void AirplaneSceneObject::setVBOtexcoord(const GLuint handle) {
	this->m_vertexBufferHandle_texcoord = handle;
}

void AirplaneSceneObject::setVBOnormal(const GLuint handle) {
	this->m_vertexBufferHandle_normal = handle;
}

void AirplaneSceneObject::setIBO(const GLuint handle) {
	this->m_iboHandle = handle;
}

void AirplaneSceneObject::setDrawCount(const int count) {
	this->airplaneDrawCount = count;
}

void AirplaneSceneObject::setModelMat(const glm::mat4 modelMat) {
	this->m_airplaneModelMat = modelMat;
}

void AirplaneSceneObject::update() {
	//cout << this->airplaneDrawCount << "\n";
	glBindVertexArray(this->m_vaoHandle);
	glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, glm::value_ptr(this->m_airplaneModelMat));
	for (int i = 0; i < 1; ++i)
	{
		glBindVertexArray(this->m_vaoHandle);
		glActiveTexture(GL_TEXTURE0);
		//int materialID = this->airplaneShapes[i].materialID;
		//glBindTexture(GL_TEXTURE_2D, this->airplaneMaterails[materialID].diffuse_tex);
		glDrawElements(GL_TRIANGLES, this->airplaneDrawCount, GL_UNSIGNED_INT, 0);
	}
}