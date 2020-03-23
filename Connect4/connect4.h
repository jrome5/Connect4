#pragma once
#include <string>
#include <vector>

namespace connect4
{
	const int COLS = 7;
	const int ROWS = 6;
	typedef char Board[ROWS][COLS];
	constexpr auto MAX_COINS = ROWS * COLS;
	constexpr char player_coin = char(254);
	constexpr char computer_coin = char(79);

	void initializeBoard(Board& board);
	void dropCoin(Board& board, const int choice, const char coin);
	void display(const Board board);
	int makeRandomDecision(const Board board);
	std::vector<int> getAvailableMoves(const Board board);
	bool checkWinner(const Board board, const char coin);
	bool checkVertical(const Board board, const char coin);
	bool checkHorizontal(const Board board, const char coin);
	bool checkForwardDiagonal(const Board board, const char coin);
	bool checkBackDiagonal(const Board board, const char coin);
	std::string playGame(bool players_turn);

	inline void copyState(Board& s1, const Board& s2)
	{
		for (int i = 0; i < ROWS; i++)
			for (int j = 0; j < COLS; j++)
				s1[i][j] = s2[i][j];
	}
} // end namespace connect4