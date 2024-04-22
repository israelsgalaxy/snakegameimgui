#include "gui.h"
#include "snakegame.h"

constexpr float grid_size = 15;
constexpr float cell_size = gui::WINDOW_WIDTH / grid_size;
constexpr float offset_y = gui::WINDOW_HEIGHT - gui::WINDOW_WIDTH;
constexpr float min_delay_time = 0.1f;

static SnakeGame::Direction snake_direction = SnakeGame::Direction::Halt;

enum class CellType { Food, Grid, SnakeHead, SnakeBody };

void DrawCell(const std::pair<int, int> &cell, const CellType &cell_type) {
  ImVec2 top_left(cell.first * cell_size, (cell.second * cell_size) + offset_y);
  ImVec2 bottom_right((cell.first + 1) * cell_size,
                      ((cell.second + 1) * cell_size) + offset_y);

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

void DrawSnake(const std::pair<int, int> &snake_head,
               const std::deque<std::pair<int, int>> &snake_body) {
  DrawCell(snake_head, CellType::SnakeHead);

  for (const std::pair<int, int> &cell : snake_body) {
    DrawCell(cell, CellType::SnakeBody);
  }
}

void gui::CreateContent() {
  static SnakeGame snake_game(grid_size);
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

  for (int x = 0; x < grid_size; ++x) {
    for (int y = 0; y < grid_size; ++y) {
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
    if (!(snake_direction == SnakeGame::Direction::Halt)) {
      switch (snake_game.MoveSnake(snake_direction)) {
      case SnakeGame::MoveResult::Body:
        game_over = true;
        break;
      case SnakeGame::MoveResult::Grid:
        break;
      case SnakeGame::MoveResult::Food:
        score += 0.2f;
        delay_time =
            (delay_time > min_delay_time) ? delay_time - 0.02f : min_delay_time;
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
      snake_direction = SnakeGame::Direction::Left;
      break;
    case GLFW_KEY_RIGHT:
      snake_direction = SnakeGame::Direction::Right;
      break;
    case GLFW_KEY_DOWN:
      snake_direction = SnakeGame::Direction::Down;
      break;
    case GLFW_KEY_UP:
      snake_direction = SnakeGame::Direction::Up;
      break;
    case GLFW_KEY_SPACE:
      snake_direction = SnakeGame::Direction::Halt;
      break;
    }
  }
}

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