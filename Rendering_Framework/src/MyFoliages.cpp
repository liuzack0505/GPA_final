#include "MyFoliages.h"


MyFoliages::MyFoliages() {};
MyFoliages::~MyFoliages() {};

void MyFoliages::init() {
	Texture_data = new unsigned char[IMG_SIZE * 5];

	Grass_model = loadTextureModels("assets/grassB.obj", 0);
	Grass_model->offset = 0;
	modelMat = glm::translate(glm::vec3(0.0, 0.0, 0.0));
	Bush_1_model = loadTextureModels("assets/bush01.obj", 1);
	Bush_1_model->offset = Grass_model->drawCount;
	Bush_2_model = loadTextureModels("assets/bush05.obj", 2);
	Bush_2_model->offset = Bush_1_model->offset + Bush_1_model->drawCount;
	Building1 = loadTextureModels("assets/Medieval_Building_LowPoly/medieval_building_lowpoly_1.obj", 3);
	Building1->offset = Bush_2_model->offset + Building1->drawCount;
	Building2 = loadTextureModels("assets/Medieval_Building_LowPoly/medieval_building_lowpoly_2.obj", 4);
	Building2->offset = Building1->offset + Building2->drawCount;
			
	bool isShader = initShader();
	if (!isShader) {
		cout << "init shader failed" << endl;
	}
	initBuffer();

	cout << "load foliages model successfully" << endl;
	//generate texture
	glGenTextures(1, &texture3D);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture3D);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, 1024, 1024, 5);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 1024, 1024, 5, GL_RGBA,
		GL_UNSIGNED_BYTE, Texture_data);

	//glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		cout << "OpenGL error: " << error << endl;
	}
	else {
		cout << "ok\n";
	}
	cout << "Generate 3D texture successfully" << endl;

	loadSpacialSample();
	initRawDataBuffer();
	initSSBO();
	initDrawCommands();

};

void MyFoliages::computeShader(const glm::mat4& viewMat, const glm::mat4& projMat) {
	computeShaderProgram->useProgram();
	glm::mat4 projViewMat = glm::transpose(projMat * viewMat);
	//glm::mat4 projViewMat = projMat * viewMat;
	glUniform1i(0, GRASS_MAX_INSTANCE);
	glUniform1i(1, BUSH1_MAX_INSTANCE);
	glUniform1i(2, BUSH2_MAX_INSTANCE);
	glUniform1i(3, BUILDING1_MAX_INSTANCE);
	glUniform1i(4, BUILDING2_MAX_INSTANCE);
	glUniform4fv(5, 1, glm::value_ptr(projViewMat[0] + projViewMat[3]));
	glUniform4fv(6, 1, glm::value_ptr(projViewMat[1] + projViewMat[3]));
	glUniform4fv(7, 1, glm::value_ptr(projViewMat[2] + projViewMat[3]));
	glUniform4fv(8, 1, glm::value_ptr(projViewMat[3] - projViewMat[0]));
	glUniform4fv(9, 1, glm::value_ptr(projViewMat[3] - projViewMat[1]));
	glUniform4fv(10, 1, glm::value_ptr(projViewMat[3] - projViewMat[2]));
	glDispatchCompute((BUILDING2_MAX_INSTANCE / 1024) + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void MyFoliages::resetShader() {
	resetShaderProgram->useProgram();
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void MyFoliages::render() {
	glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, 1);
	glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, 3);


	glBindVertexArray(vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D_ARRAY, texture3D);

	//render grass
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandSSBO);
	glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, glm::value_ptr(modelMat));
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 5, 0);

			
}
bool MyFoliages::initShader() {
	Shader* rshader = new Shader(GL_COMPUTE_SHADER);
	rshader->createShaderFromFile("src/shader/resetShader.glsl");
	cout << rshader->shaderInfoLog() << "\n";

	Shader* cshader = new Shader(GL_COMPUTE_SHADER);
	cshader->createShaderFromFile("src/shader/computeShader.glsl");
	cout << cshader->shaderInfoLog() << "\n";
	
	//create reset shader
	resetShaderProgram = new ShaderProgram();
	resetShaderProgram->init();
	resetShaderProgram->attachShader(rshader);
	resetShaderProgram->checkStatus();
	if (resetShaderProgram->status() != ShaderProgramStatus::READY) {
		return false;
	}
	resetShaderProgram->linkProgram();

	rshader->releaseShader();

	delete rshader;

	//create compute shader
	computeShaderProgram = new ShaderProgram();
	computeShaderProgram->init();
	computeShaderProgram->attachShader(cshader);
	computeShaderProgram->checkStatus();
	if (computeShaderProgram->status() != ShaderProgramStatus::READY) {
		return false;
	}
	computeShaderProgram->linkProgram();

	cshader->releaseShader();

	delete cshader;
}

void MyFoliages::initBuffer() {
	//set the material index
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// create 3 vbos to hold data
	glGenBuffers(1, &vbo_position);
	glGenBuffers(1, &vbo_normal);
	glGenBuffers(1, &vbo_texcoord);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, positionData.size() * sizeof(GLfloat), &positionData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	positionData.clear();
	positionData.shrink_to_fit();

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(GLfloat), &normalData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	normalData.clear();
	normalData.shrink_to_fit();

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
	glBufferData(GL_ARRAY_BUFFER, texcoordData.size() * sizeof(GLfloat), &texcoordData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	texcoordData.clear();
	texcoordData.shrink_to_fit();

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), &indexData[0], GL_STATIC_DRAW);

	indexData.clear();
	indexData.shrink_to_fit();
	//lift bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyFoliages::loadSpacialSample() {
	grass_sample = MyPoissonSample::fromFile("assets/poissonPoints_621043_after.ppd2");
	bush_1_sample = MyPoissonSample::fromFile("assets/poissonPoints_1010.ppd2");
	bush_2_sample = MyPoissonSample::fromFile("assets/poissonPoints_2797.ppd2");
	building_1_sample = MyPoissonSample::fromFile("assets/cityLots_sub_1.ppd2");
	building_2_sample = MyPoissonSample::fromFile("assets/cityLots_sub_0.ppd2");
	

	if (!grass_sample || !bush_1_sample || !bush_2_sample) {
		cout << "import binary file failed" << endl;
	}
	else {
		cout << "import binary file successfully" << endl;
		GRASS_MAX_INSTANCE = grass_sample->m_numSample;
		BUSH1_MAX_INSTANCE = GRASS_MAX_INSTANCE + bush_1_sample->m_numSample;
		BUSH2_MAX_INSTANCE = BUSH1_MAX_INSTANCE + bush_2_sample->m_numSample;
		BUILDING1_MAX_INSTANCE = BUSH2_MAX_INSTANCE + building_1_sample->m_numSample;
		BUILDING2_MAX_INSTANCE = BUILDING1_MAX_INSTANCE + building_2_sample->m_numSample;
	}

	return;
}

void MyFoliages::initRawDataBuffer() {
	rawData = new rawSample[BUILDING2_MAX_INSTANCE];
	rotationData = new glm::mat4[BUILDING2_MAX_INSTANCE];
			
	int i = 0;
	vec3 rad;

	for (int idx = 0; idx < grass_sample->m_numSample; idx++){
		float* dst = &grass_sample->m_positions[idx * 3];
		rawData[i].position = glm::vec4(dst[0], dst[1], dst[2], 0);
		rawData[i].extra_data = glm::vec4(0, 0.66, 1.4, 0);
		//load rotation
		dst = &grass_sample->m_radians[idx * 3];
		rad = vec3(dst[0], dst[1], dst[2]);
		glm::quat q = glm::quat(rad);
		rotationData[i] = glm::toMat4(q);
		i++;
	}

	for (int idx = 0; idx < bush_1_sample->m_numSample; idx++) {
		const float* dst = &bush_1_sample->m_positions[idx * 3];
		rawData[i].position = glm::vec4(dst[0], dst[1], dst[2], 0);
		rawData[i].extra_data = glm::vec4(1, 2.55, 3.4, 0);
		dst = &bush_1_sample->m_radians[idx * 3];
		rad = vec3(dst[0], dst[1], dst[2]);
		glm::quat q = glm::quat(rad);
		rotationData[i] = glm::toMat4(q);
		i++;
	}

	for (int idx = 0; idx < bush_2_sample->m_numSample; idx++) {
		const float* dst = &bush_2_sample->m_positions[idx * 3];
		rawData[i].position = glm::vec4(dst[0], dst[1], dst[2], 0);
		rawData[i].extra_data = glm::vec4(2, 1.76, 2.6, 0);
		dst = &bush_2_sample->m_radians[idx * 3];
		rad = vec3(dst[0], dst[1], dst[2]);
		glm::quat q = glm::quat(rad);
		rotationData[i] = glm::toMat4(q);
		i++;
	}

	for (int idx = 0; idx < building_1_sample->m_numSample; idx++) {
		const float* dst = &building_1_sample->m_positions[idx * 3];
		rawData[i].position = glm::vec4(dst[0], dst[1], dst[2], 0);
		rawData[i].extra_data = glm::vec4(3, 4.57, 10.2, 0);
		dst = &building_1_sample->m_radians[idx * 3];
		rad = vec3(dst[0], dst[1], dst[2]);
		glm::quat q = glm::quat(rad);
		rotationData[i] = glm::toMat4(q);
		i++;
	}

	for (int idx = 0; idx < building_2_sample->m_numSample; idx++) {
		const float* dst = &building_2_sample->m_positions[idx * 3];
		rawData[i].position = glm::vec4(dst[0], dst[1], dst[2], 0);
		rawData[i].extra_data = glm::vec4(4, 4.57, 8.5, 0);
		dst = &building_2_sample->m_radians[idx * 3];
		rad = vec3(dst[0], dst[1], dst[2]);
		glm::quat q = glm::quat(rad);
		rotationData[i] = glm::toMat4(q);
		i++;
	}

}

void MyFoliages::initSSBO() {

	glBindVertexArray(vao);
	//create SSBO for raw data
	glGenBuffers(1, &rawDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawDataSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, BUILDING2_MAX_INSTANCE * sizeof(rawSample), rawData, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rawDataSSBO);

	delete rawData;
	//create SSBO for valid data
	glGenBuffers(1, &validDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, validDataSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, BUILDING2_MAX_INSTANCE * sizeof(Sample), nullptr, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, validDataSSBO);
	//create SSbo for rotation data

	//create SSBO for valid data
	glGenBuffers(1, &rotationDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, rotationDataSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, BUILDING2_MAX_INSTANCE * sizeof(glm::mat4), rotationData, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, rotationDataSSBO);

	delete rotationData;

	glBindBuffer(GL_ARRAY_BUFFER, validDataSSBO);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	glBindVertexArray(0);
}

void MyFoliages::initDrawCommands() {
	//grass draw command
	drawCommands[0].count = Grass_model->drawCount;
	drawCommands[0].instanceCount = 0;
	drawCommands[0].firstIndex = 0;
	drawCommands[0].baseVertex = 0;
	drawCommands[0].baseInstance = 0;

	//bush 1 draw command
	drawCommands[1].count = Bush_1_model->drawCount;
	drawCommands[1].instanceCount = 0;
	drawCommands[1].firstIndex = Bush_1_model->offset;
	drawCommands[1].baseVertex = Bush_1_model->offset;
	drawCommands[1].baseInstance = GRASS_MAX_INSTANCE;

	//bush 2 draw command
	drawCommands[2].count = Bush_2_model->drawCount;
	drawCommands[2].instanceCount = 0;
	drawCommands[2].firstIndex = Bush_2_model->offset;
	drawCommands[2].baseVertex = Bush_2_model->offset;
	drawCommands[2].baseInstance = BUSH1_MAX_INSTANCE;

	//building 1 draw command
	drawCommands[3].count = Building1->drawCount * 2;
	drawCommands[3].instanceCount = 0;
	drawCommands[3].firstIndex = Building1->offset;
	drawCommands[3].baseVertex = Building1->offset;
	drawCommands[3].baseInstance = BUSH2_MAX_INSTANCE;

	//building 2 draw command
	drawCommands[4].count = Building2->drawCount * 2;
	drawCommands[4].instanceCount = 0;
	drawCommands[4].firstIndex = Building2->offset;
	drawCommands[4].baseVertex = Building2->offset;
	drawCommands[4].baseInstance = BUILDING1_MAX_INSTANCE;

	glGenBuffers(1, &commandSSBO);

	//load grass command data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, commandSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(DrawCommand) * 5, &drawCommands[0], GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, commandSSBO);

};
		
Model* MyFoliages::loadTextureModels(const string& model_path, const int model_num) {

	Model* model = new Model();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model_path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if (scene != nullptr) {
		printf("Load Models Success ! Shapes size %d Material size %d\n", scene->mNumMeshes, scene->mNumMaterials);
	}

	unsigned char* data;
	// load width, height and data from texturePath.C_Str()
	string textureName = model_path.substr(0, model_path.length() - 4) + ".png";
	data = loadTextureImage(textureName);
	if (data != nullptr) {
		cout << "model " << model_num << "'s texture load successfully" << endl;
		memcpy(Texture_data + IMG_SIZE * model_num, data, IMG_SIZE);
	}
	else {
		cout << "model " << model_num << "'s texture load failed" << endl;
	}

	//load Mesh's infomation, there is only one mesh in our model
			
	aiMesh* mesh = scene->mMeshes[0];
	cout << "model has " << mesh->mNumVertices << " vertices" << endl;
	for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
	{
		positionData.push_back(mesh->mVertices[v][0]);
		positionData.push_back(mesh->mVertices[v][1]);
		positionData.push_back(mesh->mVertices[v][2]);
		normalData.push_back(mesh->mNormals[v][0]);
		normalData.push_back(mesh->mNormals[v][1]);
		normalData.push_back(mesh->mNormals[v][2]);
		texcoordData.push_back(mesh->mTextureCoords[0][v][0]);
		texcoordData.push_back(mesh->mTextureCoords[0][v][1]);
		texcoordData.push_back(model_num);
	}
			
	// create 1 ibo to hold data
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indexData.push_back(mesh->mFaces[f].mIndices[0]);
		indexData.push_back(mesh->mFaces[f].mIndices[1]);
		indexData.push_back(mesh->mFaces[f].mIndices[2]);
	}
				
	model->drawCount = mesh->mNumFaces * 3;

	return model;
}

unsigned char* MyFoliages::loadTextureImage(const string& texture_path) {
	int width;
	int height;
	int n;
	unsigned char* texture_data = nullptr;
	const char* Path = texture_path.c_str();
			
	stbi_uc* data = stbi_load(Path, &width, &height, &n, 4);
	int data_size = width * height * 4;
	if (data != NULL)
	{
		texture_data = new unsigned char[data_size];
		memcpy(texture_data, data, data_size * sizeof(unsigned char));
		stbi_image_free(data);
	}
	else {
		cout << "failed to load " << texture_path << endl;
	}
	return texture_data;
}