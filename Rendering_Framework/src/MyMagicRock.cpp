#include "MyMagicRock.h"

#include <glm\mat4x4.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

MyMagicRock::MyMagicRock()
{	
}


MyMagicRock::~MyMagicRock()
{
}

void MyMagicRock::Init() {
	vector<GLfloat> positionData;
	vector<GLfloat> normalData;
	vector<GLfloat> texcoordData;
	vector<unsigned int> indexData;

	// load magic model
	string path = "assets/MagicRock/magicRock.obj";

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	printf("Load Models Success ! Shapes size %d Material size %d\n", scene->mNumMeshes, scene->mNumMaterials);

	//load first mesh's infomation
	aiMesh* mesh = scene->mMeshes[0];
	cout << "model has " << mesh->mNumVertices << " vertices" << endl;

	this->m_dynamicSO = new DynamicSceneObject(mesh->mNumVertices, mesh->mNumFaces * 3, true, true, false);

	float* databuffer = this->m_dynamicSO->dataBuffer();
	
	const unsigned int normalOffset = mesh->mNumVertices * 3;
	const unsigned int uvOffset = mesh->mNumVertices * 3 * 2;

	for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
	{
		databuffer[v * 9] = mesh->mVertices[v][0];
		databuffer[v * 9 + 1] = mesh->mVertices[v][1];
		databuffer[v * 9 + 2] = mesh->mVertices[v][2];
		databuffer[v * 9 + 3] = mesh->mNormals[v][0];
		databuffer[v * 9 + 4] = mesh->mNormals[v][1];
		databuffer[v * 9 + 5] = mesh->mNormals[v][2];
		databuffer[v * 9 + 6] = mesh->mTextureCoords[0][v][0];
		databuffer[v * 9 + 7] = mesh->mTextureCoords[0][v][1];
		databuffer[v * 9 + 8] = 0;
	}

	unsigned int* indexbuffer = this->m_dynamicSO->indexBuffer();

	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indexbuffer[f * 3] = mesh->mFaces[f].mIndices[0];
		indexbuffer[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indexbuffer[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	this->m_dynamicSO->updateDataBuffer(0, mesh->mNumVertices * 36);
	this->m_dynamicSO->updateIndexBuffer(0, mesh->mNumFaces * 12);

	loadTexture("assets/MagicRock/StylMagicRocks_AlbedoTransparency.png", this->albedo_handle);
	loadTexture("assets/MagicRock/StylMagicRocks_NormalOpenGL.png", this->normal_handle);

	

	this->m_dynamicSO->setPrimitive(GL_TRIANGLES);
	this->m_dynamicSO->setModelMat(glm::translate(glm::vec3(25.92, 18.27, 11.75)));
	this->m_dynamicSO->setPixelFunctionId(1);
}

void MyMagicRock::loadTexture(const string &path, GLuint &handle) {

	int width;
	int height;
	int n;
	unsigned char* texture_data;
	stbi_set_flip_vertically_on_load(true); // verticalmirror image data
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &n, 4);
	int data_size = width * height * 4;
	if (data != NULL)
	{
		texture_data = new unsigned char[data_size];
		memcpy(data, data, data_size * sizeof(unsigned char));
		stbi_image_free(data);

		//create texture
		glGenTextures(1, &handle);
		glBindTexture(GL_TEXTURE_2D, handle);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		cout << path << " texture load successfully" << endl;
	}
	else {
		cout << "failed to load albedo texture" << endl;
	}
}

void MyMagicRock::updateDataBuffer() {
	this->m_dynamicSO->updateDataBuffer(0, this->m_totalDataByte);
}

DynamicSceneObject *MyMagicRock::sceneObject() const {
	return this->m_dynamicSO;
}

void MyMagicRock::updateState(const glm::mat4 &viewMat, const glm::vec3 &viewPos) {
	glm::mat4 tMat = glm::translate(viewPos);
	glm::mat4 viewT = glm::transpose(viewMat);
	glm::vec4 forward = -1.0f * glm::vec4(viewT[2].x, viewT[2].y, viewT[2].z, 0.0);
	glm::vec4 x = -1.0f * glm::vec4(viewT[0].x, viewT[0].y, viewT[0].z, 0.0);
	glm::vec4 y = glm::vec4(viewT[1].x, viewT[1].y, viewT[1].z, 0.0);

	glm::mat4 rMat;
	rMat[0] = x;
	rMat[1] = y;
	rMat[2] = forward;
	rMat[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);

	this->m_dynamicSO->setModelMat(tMat * rMat);
}

void MyMagicRock::render() {
	glActiveTexture(SceneManager::Instance()->m_normalTexUnit);
	glBindTexture(GL_TEXTURE_2D, this->normal_handle);
	glActiveTexture(SceneManager::Instance()->m_albedoTexUnit);
	glBindTexture(GL_TEXTURE_2D, this->albedo_handle);
	glActiveTexture(SceneManager::Instance()->m_elevationTexUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	this->m_dynamicSO->update();
}

