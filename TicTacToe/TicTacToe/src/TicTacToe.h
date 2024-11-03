#pragma once
#include <vector>

#include "Shader.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

const int BOARD_SIZE = 3;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

enum Player { NONE, PLAYER_X, PLAYER_O };

class TicTacToe
{
public:
	TicTacToe() : board(BOARD_SIZE, std::vector<Player>(BOARD_SIZE, NONE)), currentPlayer(PLAYER_X), scoreX(0), scoreO(0),
		m_Shader("shaders/color.vs", "shaders/color.fs"), m_TextureShader("shaders/texture.vs", "shaders/texture.fs") {}

	void DrawBackground();
	void DrawBoard();
	void HandleClick(int x, int y);
	void AiMove();
	bool CheckWin(Player player);
	void RenderScores();
	void Reset();

private:
	std::vector<std::vector<Player>> board;
	Player currentPlayer;
	int scoreX, scoreO;

	void SwitchPlayer();
	bool IsBoardFull();
	void DrawX(int row, int rol);
	void DrawO(int row, int col);
	void DrawGrid();

	//algorithm for AI
	int minimax(bool isMaximizing);
	int evaluate();

	Shader m_Shader;
	Shader m_TextureShader;
};

