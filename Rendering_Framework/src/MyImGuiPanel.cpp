#include "MyImGuiPanel.h"



MyImGuiPanel::MyImGuiPanel()
{
	this->m_avgFPS = 0.0;
	this->m_avgFrameTime = 0.0;
}


MyImGuiPanel::~MyImGuiPanel()
{
}

void MyImGuiPanel::update() {
	// performance information
	const std::string FPS_STR = "FPS: " + std::to_string(this->m_avgFPS);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FPS_STR.c_str());
	const std::string FT_STR = "Frame: " + std::to_string(this->m_avgFrameTime);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FT_STR.c_str());

	if (ImGui::Button("Teleport 0")) {
		this->m_button0Pressed = true;
	}
	else {
		this->m_button0Pressed = false;
	}

	if (ImGui::Button("Teleport 1")) {
		this->m_button1Pressed = true;
	}
	else {
		this->m_button1Pressed = false;
	}

	if (ImGui::Button("Teleport 2")) {
		this->m_button2Pressed = true;
	}
	else {
		this->m_button2Pressed = false;
	}

	ImGui::Checkbox("Normal Mapping", &m_normalMappingCheckBox);

	if (ImGui::Button("World Vertex")) {
		this->m_worldVertex = true;
	}
	else {
		this->m_worldVertex = false;
	}

	if (ImGui::Button("World Normal")) {
		this->m_worldNormal = true;
	}
	else {
		this->m_worldNormal = false;
	}

	if (ImGui::Button("Diffuse")) {
		this->m_diffuse = true;
	}
	else {
		this->m_diffuse = false;
	}

	if (ImGui::Button("Ambient")) {
		this->m_ambient = true;
	}
	else {
		this->m_ambient = false;
	}

	if (ImGui::Button("Specular")) {
		this->m_specular = true;
	}
	else {
		this->m_specular = false;
	}

	if (ImGui::Button("Original")) {
		this->m_original = true;
	}
	else {
		this->m_original = false;
	}
}

bool MyImGuiPanel::isButton0Pressed() {
	return this->m_button0Pressed;
}

bool MyImGuiPanel::isButton1Pressed() {
	return this->m_button1Pressed;
}

bool MyImGuiPanel::isButton2Pressed() {
	return this->m_button2Pressed;
}

bool MyImGuiPanel::isNormalMapping() {
	return this->m_normalMappingCheckBox;
}

void MyImGuiPanel::setAvgFPS(const double avgFPS){
	this->m_avgFPS = avgFPS;
}
void MyImGuiPanel::setAvgFrameTime(const double avgFrameTime){
	this->m_avgFrameTime = avgFrameTime;
}

bool MyImGuiPanel::isWorldVertex() {
	return this->m_worldVertex;
}

bool MyImGuiPanel::isWorldNormal() {
	return this->m_worldNormal;
}

bool MyImGuiPanel::isDiffuse() {
	return this->m_diffuse;
}

bool MyImGuiPanel::isAmbient() {
	return this->m_ambient;
}

bool MyImGuiPanel::isSpecular() {
	return this->m_specular;
}

bool MyImGuiPanel::isOriginal() {
	return this->m_original;
}