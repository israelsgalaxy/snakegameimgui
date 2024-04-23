#include "gui.h"

int main() {
  gui::CreateGlfwWindow("Snake");
  gui::CreateImgui();

  while (!glfwWindowShouldClose(gui::window)) {
    gui::PreRender();
    gui::CreateContent();
    gui::Render();
  }

  gui::DestroyImgui();
  gui::DestroyGlfwWindow();

  return 0;
}