#include "TicTacToe.h"

void TicTacToe::DrawBackground()
{
	m_TextureShader.use();
	
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	m_TextureShader.setInt("u_Texture", 0);

	//set up data
	float vertices[] = {
		1.0f,  1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
	   -1.0f, -1.0f, 0.0f, 0.0f,
	   -1.0f,  1.0f, 0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void TicTacToe::DrawBoard()
{
	if (state == GameState::GAME_PLAY)
	{
		m_Shader.use();
		DrawGrid();

		for (int row = 0; row < BOARD_SIZE; row++) {
			for (int col = 0; col < BOARD_SIZE; col++) {
				if (board[row][col] == PLAYER_X) {
					DrawX(row, col);
				}
				else if (board[row][col] == PLAYER_O) {
					DrawO(row, col);
				}
			}
		}
	}
}

void TicTacToe::HandleClick(int x, int y)
{
	if (state == GameState::GAME_PLAY)
	{
		int row = y / (WINDOW_HEIGHT / BOARD_SIZE);
		int col = x / (WINDOW_WIDTH / BOARD_SIZE);

		if (board[row][col] == NONE) {
			board[row][col] = currentPlayer;
			if (CheckWin(currentPlayer)) {
				std::cout << "Player" << (currentPlayer == PLAYER_X ? "X" : "O") << " wins!" << std::endl;
				if (currentPlayer == PLAYER_X) {
					DrawX(row, col);
					scoreX++;
				}
				else {
					DrawO(row, col);
					scoreO++;
				}
				Reset();
			}
			else if (IsBoardFull()) {
				std::cout << "Ends in a draw!" << std::endl;
				Reset();
			}
			else {
				SwitchPlayer();
				if (chooseAI) {
					AiMove();
					if (CheckWin(currentPlayer)) {
						std::cout << "Player O wins!" << std::endl;
						++scoreO;
						Reset();
					}
					else if (IsBoardFull()) {
						std::cout << "It's a draw!" << std::endl;
						Reset();
					}
					else SwitchPlayer();
				}
			}
		}
	}
}
/*
if (CheckWin(currentPlayer)) {
					std::cout << "Player O wins!" << std::endl;
					++scoreO;
					Reset();
				}
				else if (IsBoardFull()) {
					std::cout << "It's a draw!" << std::endl;
					Reset();
				}
				else SwitchPlayer();
*/
void TicTacToe::AiMove()
{
	if (currentPlayer == PLAYER_O) {
		if (aiMode == AIMode::EASY)
		{
			std::vector<std::pair<int, int>> availableMoves;
			for (int row = 0; row < BOARD_SIZE; ++row) {
				for (int col = 0; col < BOARD_SIZE; ++col) {
					if (board[row][col] == NONE) {
						availableMoves.emplace_back(row, col);
					}
				}
			}

			if (!availableMoves.empty()) {
				auto move = availableMoves[std::rand() % availableMoves.size()];
				board[move.first][move.second] = currentPlayer;
			}
		}
		else if (aiMode == AIMode::HARD)
		{
			int bestVal = -1000;
			int bestRow = -1, bestCol = -1;

			for (int row = 0; row < BOARD_SIZE; row++) {
				for (int col = 0; col < BOARD_SIZE; col++) {
					if (board[row][col] == NONE) {
						board[row][col] = PLAYER_O;
						int moveVal = minimax(false);
						board[row][col] = NONE;
						if (moveVal > bestVal) {
							bestRow = row;
							bestCol = col;
							bestVal = moveVal;
						}
					}
				}
			}

			board[bestRow][bestCol] = currentPlayer;
		}
	}
}

bool TicTacToe::CheckWin(Player player)
{
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
		if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
	}
	if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return true;
	if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return true;

	return false;
}

void TicTacToe::RenderScores()
{
	ImGui::Begin("Scores", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SetWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);

	ImGui::Text("Player X: %d", scoreX);
	ImGui::Text("Player O: %d", scoreO);

	if (ImGui::Button("Return MainMenu", ImVec2(200, 50))) {
		RestartGame();
	}
	ImGui::End();
}

void TicTacToe::RenderUI()
{
	switch (state) {
		case GameState::MAIN_MENU:
			DrawMainMenu();
			break;
		case GameState::GAME_PLAY:
			RenderScores();
			break;
	}
}

void TicTacToe::RestartGame()
{
	Reset();
	scoreX = 0;
	scoreO = 0;
	state = GameState::MAIN_MENU;
}

void TicTacToe::Reset()
{
	board = std::vector<std::vector<Player>>(BOARD_SIZE, std::vector<Player>(BOARD_SIZE, NONE));
	currentPlayer = PLAYER_X;
}

void TicTacToe::SwitchPlayer()
{
	currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
}

bool TicTacToe::IsBoardFull()
{
	for (const auto& row : board) {
		for (const auto& cell : row)
			if (cell == NONE) return false;
	}
	return true;
}

void TicTacToe::DrawX(int row, int col)
{
	float vertices[2 * 4] = {
		-0.4f, -0.4f, 0.4f,  0.4f,
		-0.4f,  0.4f, 0.4f, -0.4f
	};

	glm::mat4 tranform = glm::translate(glm::mat4(1.0f), glm::vec3((col - 1) * 0.7f, (1 - row) * 0.7f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, 0.6f, 1.0f));
	
	m_Shader.setMat4("u_Transform", tranform);
	m_Shader.setFloat3("u_Color", glm::vec3(0.8f, 0.2f, 0.3f));

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void TicTacToe::DrawO(int row, int col)
{
	const int segments = 100;
	float radius = 0.4f;
	std::vector<glm::vec2> vertices;

	for (int i = 0; i < segments; i++) {
		float theta = 2.0f * 3.1415926f * float(i) / float(segments);
		vertices.emplace_back(radius * cosf(theta), radius * sinf(theta));
	}

	glm::mat4 tranform = glm::translate(glm::mat4(1.0f), glm::vec3((col - 1) * 0.7f, (1 - row) * 0.7f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.6f, 0.6f, 1.0f));

	m_Shader.setMat4("u_Transform", tranform);
	m_Shader.setFloat3("u_Color", glm::vec3(0.2f, 0.3f, 0.8f));

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINE_LOOP, 0, segments);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void TicTacToe::DrawGrid()
{
	float vertices[4 * 4] = {
		-0.33f, -1.0f, -0.33f,  1.0f,
		 0.33f, -1.0f,  0.33f,  1.0f,
		-1.0f, -0.33f,  1.0f, -0.33f,
		-1.0f,  0.33f,  1.0f,  0.33f
	};

	m_Shader.setMat4("u_Transform", glm::mat4(1.0f));
	m_Shader.setFloat3("u_Color", glm::vec3(0.8f, 0.8f, 0.8f));

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 8);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

int TicTacToe::minimax(bool isMaximizing)
{
	int score = evaluate();
	if (score == 10) return score;
	if (score == -10) return score;
	if (IsBoardFull()) return 0;

	if (isMaximizing) {
		int bestScore = -1000;
		for (int row = 0; row < BOARD_SIZE; row++) {
			for (int col = 0; col < BOARD_SIZE; col++) {
				if (board[row][col] == NONE) {
					board[row][col] = PLAYER_O;
					bestScore = std::max(bestScore, minimax(false));
					board[row][col] = NONE;
				}
			}
		}
		return bestScore;
	}
	else {
		int bestScore = 1000;
		for (int row = 0; row < BOARD_SIZE; row++) {
			for (int col = 0; col < BOARD_SIZE; col++) {
				if (board[row][col] == NONE) {
					board[row][col] = PLAYER_X;
					bestScore = std::min(bestScore, minimax(true));
					board[row][col] = NONE;
				}
			}
		}
		return bestScore;
	}
}

int TicTacToe::evaluate()
{
	for (int row = 0; row < BOARD_SIZE; row++) {
		if (board[row][0] == board[row][1] && board[row][1] == board[row][2]) {
			if (board[row][0] == PLAYER_O) return 10;
			else if (board[row][0] == PLAYER_X) return -10;
		}
	}
	for (int col = 0; col < BOARD_SIZE; col++) {
		if (board[0][col] == board[1][col] && board[1][col] == board[2][col]) {
			if (board[0][col] == PLAYER_O) return 10;
			else if (board[0][col] == PLAYER_X) return -10;
		}
	}
	if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
		if (board[0][0] == PLAYER_O) return 10;
		else if (board[0][0] == PLAYER_X) return -10;
	}
	if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
		if (board[0][2] == PLAYER_O) return 10;
		else if (board[0][2] == PLAYER_X) return -10;
	}
	return 0;
}

void TicTacToe::DrawMainMenu()
{
	ImGui::Begin("Tic Tac Toe Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Welcome to Tic Tac Toe!");
	ImGui::Spacing();

	if (ImGui::Button("Start Two-Player Game", ImVec2(200, 50))) {
		chooseAI = false;
		RestartGame();
		state = GameState::GAME_PLAY;
	}

	if (ImGui::Button("Play vs AI", ImVec2(200, 50))) {
		state = GameState::MAIN_MENU;
		chooseAI = true;
	}

	if (state == GameState::MAIN_MENU && ImGui::CollapsingHeader("Select AI Difficulty")) {
		if (ImGui::RadioButton("Easy", aiMode == AIMode::EASY)) aiMode = AIMode::EASY;
		if (ImGui::RadioButton("Hard", aiMode == AIMode::HARD)) aiMode = AIMode::HARD;

		if (ImGui::Button("Start AI Game", ImVec2(200, 50))) {
			RestartGame();
			state = GameState::GAME_PLAY;
		}
	}

	ImGui::End();
}
/*
void TicTacToe::DrawGameOverScreen()
{
	ImGui::Begin("Game Over", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::SetWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	if (CheckWin(currentPlayer)) {
		ImGui::Text("Player %c Wins!", currentPlayer == Player::PLAYER_X ? 'X' : 'O');
	}
	else {
		ImGui::Text("Ends in a Draw!");
	}

	if (ImGui::Button("Return", ImVec2(150, 50))) {
		Reset();
		state = GameState::MAIN_MENU;
	}

	ImGui::End();
}
*/
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
	glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		TicTacToe* game = static_cast<TicTacToe*>(glfwGetWindowUserPointer(window));
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int x = static_cast<int>(xpos);
		int y = static_cast<int>(ypos);
		game->HandleClick(x, y);
	}
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	//Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TicTacToe", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Setup game
	TicTacToe game;
	glfwSetWindowUserPointer(window, &game);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	//Setup ImGui
	game.ImGuilayer.OnAttach();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");
	//Main loop
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		game.ImGuilayer.Begin();

		game.DrawBackground();
		game.DrawBoard();
		game.RenderUI();

		game.ImGuilayer.End();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	game.ImGuilayer.OnDetach();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}