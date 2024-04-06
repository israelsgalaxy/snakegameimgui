#include <string>
#include <string_view>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>

#include "gui.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void gui::CreateGlfwWindow(std::string_view windowName) {
  if (!glfwInit())
    std::exit(1);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                            std::string(windowName).c_str(), nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    std::exit(1);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  GLFWimage images[1];
  images[0].pixels =
      stbi_load("./assets/icon.jpg", &images[0].width, &images[0].height, 0, 4);
  glfwSetWindowIcon(window, 1, images);
  stbi_image_free(images[0].pixels);

  glfwSetKeyCallback(window, gui::KeyCallback);
}

void gui::DestroyGlfwWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void gui::CreateImgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = NULL;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void gui::DestroyImgui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void gui::PreRender() {
  glfwPollEvents();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void gui::Render() {
  ImGui::Render();
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window);
}