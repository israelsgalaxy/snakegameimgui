#include <deque>
#include <random>

#include "gui.h"

enum CellType
{
	Food,
	Grid,
	SnakeHead,
	SnakeBody
};

enum Direction
{
	Left,
	Right,
	Up,
	Down,
	Halt
};

constexpr float grid_size = 15;
constexpr float cell_size = gui::WINDOW_WIDTH / grid_size;
constexpr float offset_y = gui::WINDOW_HEIGHT - gui::WINDOW_WIDTH;

constexpr float min_delay_time = 0.1f;

static std::random_device rd{};
static std::seed_seq ss{ rd() };
static std::mt19937 mt{ ss };
static std::uniform_int_distribution u_dist{ 0, static_cast<int>(grid_size - 1) };

static Direction snake_direction = Direction::Halt;

bool EqualCells(const ImVec2& cell_a, const ImVec2& cell_b)
{
	return (cell_a.x == cell_b.x) && (cell_a.y == cell_b.y);
}

void Reset(ImVec2& snake_head, std::deque<ImVec2>& snake_body, ImVec2& food_cell, float& delay_time, float& score)
{
	std::seed_seq ss{ rd() };
	mt.seed(ss);

	delay_time = 0.5f;
	score = 0.0f;
	snake_head = ImVec2(static_cast<int>(grid_size) / 2, static_cast<int>(grid_size) / 2);
	snake_body = std::deque<ImVec2>();
	food_cell = ImVec2(u_dist(mt), u_dist(mt));
	snake_direction = Direction::Halt;
}

void DrawCell(const ImVec2& cell, const CellType& cell_type)
{
	const ImVec2& top_left = ImVec2(cell.x * cell_size, (cell.y * cell_size) + offset_y);
	const ImVec2& bottom_right = ImVec2((cell.x + 1) * cell_size, ((cell.y + 1) * cell_size) + offset_y);

	ImDrawList* draw = ImGui::GetBackgroundDrawList();

	ImU32 color_int {};

	switch (cell_type)
	{
	case CellType::Food:
		color_int = ImColor(ImVec4(0.18f, 0.67f, 0.17f, 1.0f));
		break;
	case CellType::Grid:
		color_int = ImColor(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		break;
	case CellType::SnakeHead:
		color_int = ImColor(ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 1.f));
		break;
	case CellType::SnakeBody:
		color_int = ImColor(ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 0.5f));
		break;
	}

	if (cell_type == CellType::Grid)
	{
		draw->AddRect(top_left, bottom_right, color_int);
	}
	else {
		draw->AddRectFilled(top_left, bottom_right, color_int);
	}
}

void DrawSnake(const ImVec2& snake_head, const std::deque<ImVec2>& snake_body)
{
	DrawCell(snake_head, CellType::SnakeHead);

	for (const ImVec2& cell : snake_body)
	{
		DrawCell(cell, CellType::SnakeBody);
	}
}

void UpdateSnake(ImVec2& snake_head, std::deque<ImVec2>& snake_body, ImVec2& food_cell, float& delay_time, float& score)
{
	snake_body.push_front(snake_head);

	switch (snake_direction)
	{
	case Left:
		snake_head.x = (snake_head.x == 0) ? grid_size - 1 : snake_head.x - 1;
		break;
	case Right:
		snake_head.x = (snake_head.x == (grid_size - 1)) ? 0 : snake_head.x + 1;
		break;
	case Up:
		snake_head.y = (snake_head.y == 0) ? grid_size - 1 : snake_head.y - 1;
		break;
	case Down:
		snake_head.y = (snake_head.y == (grid_size - 1)) ? 0 : snake_head.y + 1;
		break;
	}

	if (EqualCells(snake_head, food_cell))
	{
		food_cell = ImVec2(u_dist(mt), u_dist(mt));
		score += 0.2f;
		delay_time = (delay_time > min_delay_time) ? delay_time - 0.02f : min_delay_time;
	}
	else
	{
		snake_body.pop_back();
	}
}

void gui::CreateContent()
{
	static bool game_over = false;
	static float elapsed_time = 0.0f; // time elapsed since last redraw of snake
	static float delay_time = 0.5f; // minimum time that should elapse before redraw of snake
	static float score = 0.0f;

	static ImVec2 snake_head = ImVec2(static_cast<int>(grid_size) / 2, static_cast<int>(grid_size) / 2);
	static std::deque<ImVec2> snake_body = std::deque<ImVec2>();
	static ImVec2 food_cell = ImVec2(u_dist(mt), u_dist(mt));

	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT) });
	ImGui::Begin("Snake", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

	ImGui::Text("Score: %.1f", score);
	ImGui::SameLine();
	ImGui::Text(";");
	ImGui::SameLine();
	ImGui::Text("Speed: %.2f", -1 * delay_time);

	for (int x = 0; x < grid_size; ++x)
	{
		for (int y = 0; y < grid_size; ++y)
		{
			const ImVec2& cell = ImVec2(x, y);
			DrawCell(cell, CellType::Grid);
		}
	}

	DrawSnake(snake_head, snake_body);
	DrawCell(food_cell, CellType::Food);

	if (game_over)
	{
		ImGui::OpenPopup("Game Over");

		const ImVec2& center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Game Over", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			if (ImGui::Button("Restart")) {
				Reset(snake_head, snake_body, food_cell, delay_time, score);
				game_over = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		
		ImGui::End();
		return;
	}

	for (const ImVec2& cell : snake_body)
	{
		if (EqualCells(snake_head, cell))
		{
			game_over = true;
			ImGui::End();
			return;
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	elapsed_time += io.DeltaTime;

	if (elapsed_time >= delay_time)
	{
		if (!(snake_direction == Direction::Halt))
		{
			UpdateSnake(snake_head, snake_body, food_cell, delay_time, score);
		}

		elapsed_time = 0;
	}

	ImGui::End();
}

void gui::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	// Handle keyboard input if not already captured
	if (!io.WantCaptureKeyboard && action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT:
			snake_direction = Direction::Left;
			break;
		case GLFW_KEY_RIGHT:
			snake_direction = Direction::Right;
			break;
		case GLFW_KEY_DOWN:
			snake_direction = Direction::Down;
			break;
		case GLFW_KEY_UP:
			snake_direction = Direction::Up;
			break;
		case GLFW_KEY_SPACE:
			snake_direction = Direction::Halt;
			break;
		}
	}
}

int main()
{
	gui::CreateGlfwWindow("Snake");
	gui::CreateImgui();

	while (!glfwWindowShouldClose(gui::window))
	{
		gui::PreRender();
		gui::CreateContent();
		gui::Render();
	}

	gui::DestroyImgui();
	gui::DestroyGlfwWindow();

	return 0;
}