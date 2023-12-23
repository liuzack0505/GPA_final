#pragma once

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <string>

class MyImGuiPanel
{
public:
	MyImGuiPanel();
	virtual ~MyImGuiPanel();

public:
	void update();
	void setAvgFPS(const double avgFPS);
	void setAvgFrameTime(const double avgFrameTime);

	bool isButton0Pressed();
	bool isButton1Pressed();
	bool isButton2Pressed();

	bool isNormalMapping();

private:
	double m_avgFPS;
	double m_avgFrameTime;

	bool m_button0Pressed = false;
	bool m_button1Pressed = false;
	bool m_button2Pressed = false;

	bool m_normalMappingCheckBox = false;
};

