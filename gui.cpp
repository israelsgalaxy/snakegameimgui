#include <string>
#include <string_view>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "gui.h"

enum class gui::CellType { Food, Grid, SnakeHead, SnakeBody };

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

void gui::DrawCell(const std::pair<int, int> &cell, const CellType &cell_type) {
  ImVec2 top_left(cell.first * CELL_SIZE, (cell.second * CELL_SIZE) + OFFSET_Y);
  ImVec2 bottom_right((cell.first + 1) * CELL_SIZE,
                      ((cell.second + 1) * CELL_SIZE) + OFFSET_Y);

  ImDrawList *draw = ImGui::GetBackgroundDrawList();

  ImU32 color_int;

  switch (cell_type) {
  case CellType::Food:
    color_int = ImColor(ImVec4(0.18f, 0.67f, 0.17f, 1.0f));
    break;
  case CellType::Grid:
    color_int = ImColor(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    break;
  case CellType::SnakeHead:
    color_int =
        ImColor(ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 1.f));
    break;
  case CellType::SnakeBody:
    color_int =
        ImColor(ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 0.5f));
    break;
  }

  if (cell_type == CellType::Grid) {
    draw->AddRect(top_left, bottom_right, color_int);
  } else {
    draw->AddRectFilled(top_left, bottom_right, color_int);
  }
}

void gui::DrawSnake(const std::pair<int, int> &snake_head,
                    const std::deque<std::pair<int, int>> &snake_body) {
  DrawCell(snake_head, CellType::SnakeHead);

  for (const std::pair<int, int> &cell : snake_body) {
    DrawCell(cell, CellType::SnakeBody);
  }
}

void gui::CreateContent() {
  static SnakeGame snake_game(GRID_SIZE);
  static bool game_over = false;
  static float elapsed_time = 0.0f; // time elapsed since last redraw of snake
  static float delay_time =
      0.5f; // minimum time that should elapse before redraw of snake
  static float score = 0.0f;

  ImGui::SetNextWindowPos({0, 0});
  ImGui::SetNextWindowSize(
      {static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});
  ImGui::Begin("Snake", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoTitleBar);

  ImGui::Text("Score: %.1f", score);
  ImGui::SameLine();
  ImGui::Text(";");
  ImGui::SameLine();
  ImGui::Text("Speed: %.2f", -1 * delay_time);

  for (int x = 0; x < GRID_SIZE; ++x) {
    for (int y = 0; y < GRID_SIZE; ++y) {
      DrawCell({x, y}, CellType::Grid);
    }
  }

  DrawSnake(snake_game.GetHeadPosition(), snake_game.GetBodyPositions());
  DrawCell(snake_game.GetFoodPosition(), CellType::Food);

  if (game_over) {
    ImGui::OpenPopup("Game Over");

    const ImVec2 &center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Game Over", NULL,
                               ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoMove)) {
      if (ImGui::Button("Restart")) {
        delay_time = 0.5f;
        score = 0.0f;
        user_direction = SnakeGame::Direction::Halt;
        snake_game.Reset();
        game_over = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    ImGui::End();
    return;
  }

  ImGuiIO &io = ImGui::GetIO();
  elapsed_time += io.DeltaTime;

  if (elapsed_time >= delay_time) {
    if (!(user_direction == SnakeGame::Direction::Halt)) {
      switch (snake_game.MoveSnake(user_direction)) {
      case SnakeGame::MoveResult::Body:
        game_over = true;
        break;
      case SnakeGame::MoveResult::Grid:
        break;
      case SnakeGame::MoveResult::Food:
        score += 0.2f;
        delay_time =
            (delay_time > MIN_DELAY_TIME) ? delay_time - 0.02f : MIN_DELAY_TIME;
        break;
      };
    }

    elapsed_time = 0;
  }

  ImGui::End();
}

void gui::KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
  ImGuiIO &io = ImGui::GetIO();
  // Handle keyboard input if not already captured
  if (!io.WantCaptureKeyboard && action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_LEFT:
      user_direction = SnakeGame::Direction::Left;
      break;
    case GLFW_KEY_RIGHT:
      user_direction = SnakeGame::Direction::Right;
      break;
    case GLFW_KEY_DOWN:
      user_direction = SnakeGame::Direction::Down;
      break;
    case GLFW_KEY_UP:
      user_direction = SnakeGame::Direction::Up;
      break;
    case GLFW_KEY_SPACE:
      user_direction = SnakeGame::Direction::Halt;
      break;
    }
  }
}