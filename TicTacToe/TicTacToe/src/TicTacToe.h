#pragma once
#include <vector>

#include "Shader.h"
#include "ImGuiLayer.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW/glfw3.h>

const int BOARD_SIZE = 3;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

enum GameState { MAIN_MENU, GAME_PLAY };
enum Player { NONE, PLAYER_X, PLAYER_O };
enum AIMode { EASY, HARD };

class TicTacToe
{
public:
	TicTacToe() : board(BOARD_SIZE, std::vector<Player>(BOARD_SIZE, NONE)), currentPlayer(PLAYER_X), aiMode(AIMode::EASY), scoreX(0), scoreO(0),
		m_Shader("shaders/color.vs", "shaders/color.fs"), m_TextureShader("shaders/texture.vs", "shaders/texture.fs") {
		std::srand(static_cast<unsigned int>(std::time(0)));

		//load texture
		glGenTextures(1, &m_Texture);
		glBindTexture(GL_TEXTURE_2D, m_Texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load("../Checkerboard.png", &width, &height, &channels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	void DrawBackground();
	void DrawBoard();
	void HandleClick(int x, int y);
	void AiMove();
	bool CheckWin(Player player);
	void RenderScores();
	void RenderUI();
	void RestartGame();
	void Reset();
public:
	ImGuiLayer ImGuilayer;
private:
	std::vector<std::vector<Player>> board;
	GameState state;
	Player currentPlayer;
	AIMode aiMode;
	int scoreX, scoreO;
	bool chooseAI = true;

	void SwitchPlayer();
	bool IsBoardFull();
	void DrawX(int row, int rol);
	void DrawO(int row, int col);
	void DrawGrid();

	//algorithm for hard AI
	int minimax(bool isMaximizing);
	int evaluate();

	//UI Render
	void DrawMainMenu();
	//void DrawGameOverScreen();

	Shader m_Shader;
	Shader m_TextureShader;
	unsigned int m_Texture;
};

