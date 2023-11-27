#pragma once
#include <string_view>

#include <glfw3.h>
#include <imgui.h>

namespace gui
{
	inline constexpr int WINDOW_HEIGHT = 450;
	inline constexpr int WINDOW_WIDTH = 420;
	inline GLFWwindow* window = nullptr;

	void CreateGlfwWindow(std::string_view windowName);
	void DestroyGlfwWindow();

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void CreateImgui();
	void DestroyImgui();

	void PreRender();
	void CreateContent();
	void Render();
}