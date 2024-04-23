#ifndef GUI_H
#define GUI_H
#include <string_view>

#include <glfw3.h>
#include <imgui.h>

#include "snakegame.h"

namespace gui {
inline constexpr int WINDOW_HEIGHT = 450;
inline constexpr int WINDOW_WIDTH = 420;
inline constexpr float GRID_SIZE = 20;
inline constexpr float CELL_SIZE = WINDOW_WIDTH / GRID_SIZE;
inline constexpr float OFFSET_Y = WINDOW_HEIGHT - WINDOW_WIDTH;
inline constexpr float MIN_DELAY_TIME = 0.1f;

inline GLFWwindow *window = nullptr;
inline SnakeGame::Direction user_direction = SnakeGame::Direction::Halt;

enum class CellType;

void CreateGlfwWindow(std::string_view windowName);
void DestroyGlfwWindow();

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods);

void CreateImgui();
void DestroyImgui();

void PreRender();
void CreateContent();
void Render();

void DrawCell(const std::pair<int, int> &cell, const CellType &cell_type);
void DrawSnake(const std::pair<int, int> &snake_head,
               const std::deque<std::pair<int, int>> &snake_body);
} // namespace gui
#endif