#include "MyAirplane.h"



MyAirplane::MyAirplane() {}

MyAirplane::~MyAirplane() {}

void MyAirplane::init() {
	const char* path = "assets/airplane.obj";
	const aiScene* scene = nullptr;
	scene = this->airplaneImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	if (!scene) {
		cout << "Failed to load the airplane.\n";
		return;
	}
	else {
		cout << "Load the airplane successfully.\n";
	}

	// load airplane's materials
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		Material Material;
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{
			string base = "textures/";
			string fileName = string(texturePath.C_Str());
			string pathName = base + fileName;
			TextureData td = loadImage(pathName.c_str());
			if (td.data != NULL) {
				cout << "Successfully load texture " << i << "\n";
			}
			else {
				cout << "Failed to load texture " << i << "\n";
			}
			glGenTextures(1, &Material.diffuse_tex);
			glBindTexture(GL_TEXTURE_2D, Material.diffuse_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, td.width, td.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, td.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			cout << "There is no diffuse texture.\n";
		}
		this->airplaneMaterails.push_back(Material);

		
	}

	// load airplane's geometries
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];
		Shape shape;
		glGenVertexArrays(1, &shape.vao);
		glBindVertexArray(shape.vao);

		glGenBuffers(1, &shape.vbo_position);
		glGenBuffers(1, &shape.vbo_normal);
		glGenBuffers(1, &shape.vbo_texcoord);
		vector<GLfloat> vbo_p, vbo_n, vbo_t;
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			GLfloat x = mesh->mVertices[v][0];
			GLfloat y = mesh->mVertices[v][1];
			GLfloat z = mesh->mVertices[v][2];

			GLfloat nx = mesh->mNormals[v][0];
			GLfloat ny = mesh->mNormals[v][1];
			GLfloat nz = mesh->mNormals[v][2];

			GLfloat u0 = mesh->mTextureCoords[0][v][0];
			GLfloat u1 = mesh->mTextureCoords[0][v][1];

			vbo_p.push_back(x);
			vbo_p.push_back(y);
			vbo_p.push_back(z);

			vbo_n.push_back(nx);
			vbo_n.push_back(ny);
			vbo_n.push_back(nz);

			vbo_t.push_back(u0);
			vbo_t.push_back(u1);
			//vbo_t.push_back(0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
		glBufferData(GL_ARRAY_BUFFER, vbo_p.size() * sizeof(GLfloat), &vbo_p.at(0), GL_STATIC_DRAW);
		this->m_vertexBufferHandle_vertex_ = shape.vbo_position;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, vbo_t.size() * sizeof(GLfloat), &vbo_t.at(0), GL_STATIC_DRAW);
		this->m_vertexBufferHandle_texcoord_ = shape.vbo_texcoord;
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, vbo_n.size() * sizeof(GLfloat), &vbo_n.at(0), GL_STATIC_DRAW);
		this->m_vertexBufferHandle_normal_ = shape.vbo_normal;
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		this->m_vaoHandle_ = shape.vao;

		// create 1 ibo to hold data
		glGenBuffers(1, &shape.ibo);
		vector<unsigned int> ibo_;
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			unsigned int index1 = mesh->mFaces[f].mIndices[0];
			unsigned int index2 = mesh->mFaces[f].mIndices[1];
			unsigned int index3 = mesh->mFaces[f].mIndices[2];

			ibo_.push_back(index1);
			ibo_.push_back(index2);
			ibo_.push_back(index3);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo_.size() * sizeof(unsigned int), &ibo_.at(0), GL_STATIC_DRAW);
		this->m_iboHandle_ = shape.ibo;

		shape.materialID = mesh->mMaterialIndex;
		shape.drawCount = mesh->mNumFaces * 3;
		this->airplaneShapes.push_back(shape);

		this->airplaneDrawCount_ = shape.drawCount;
	}
	//cout << "airplane shape size: " << this->airplaneShapes.size() << "\n";
	this->setupAirplaneSceneObject();
}

void MyAirplane::setupAirplaneSceneObject() {
	this->m_airplaneSO = new AirplaneSceneObject();
	this->m_airplaneSO->setVAO(this->m_vaoHandle_);
	this->m_airplaneSO->setVBOvertex(this->m_vertexBufferHandle_vertex_);
	this->m_airplaneSO->setVBOtexcoord(this->m_vertexBufferHandle_texcoord_);
	this->m_airplaneSO->setVBOnormal(this->m_vertexBufferHandle_normal_);
	this->m_airplaneSO->setIBO(this->m_iboHandle_);
	this->m_airplaneSO->setDrawCount(this->airplaneDrawCount_);
}

AirplaneSceneObject* MyAirplane::sceneObject() {
	return this->m_airplaneSO;
}

void MyAirplane::render() {
	glBindVertexArray(this->m_vaoHandle_);
	//glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
	for (int i = 0; i < this->airplaneShapes.size(); ++i)
	{
		glBindVertexArray(this->m_vaoHandle_);
		glActiveTexture(GL_TEXTURE0);
		int materialID = this->airplaneShapes[i].materialID;
		glBindTexture(GL_TEXTURE_2D, this->airplaneMaterails[materialID].diffuse_tex);
		glDrawElements(GL_TRIANGLES, this->airplaneShapes[i].drawCount, GL_UNSIGNED_INT, 0);
	}
}

TextureData MyAirplane::loadImage(string path) {
	TextureData texture;
	const char* Path = path.c_str();
	int n;
	stbi_set_flip_vertically_on_load(true); // verticalmirror image data
	stbi_uc* data = stbi_load(Path, &texture.width, &texture.height, &n, 4);
	int data_size = texture.width * texture.height * 4;
	if (data != NULL)
	{
		texture.data = new unsigned char[data_size];
		memcpy(texture.data, data, data_size * sizeof(unsigned char));
		stbi_image_free(data);
	}
	else {
		cout << "failed to load " << path << endl;
	}
	return texture;
}