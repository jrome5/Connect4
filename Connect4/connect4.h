#pragma once
#include <string>
#include <vector>

namespace connect4
{
	const int COLS = 7;
	const int ROWS = 6;
	constexpr auto MAX_COINS = ROWS * COLS;
	constexpr char player_coin = char(254);
	constexpr char computer_coin = char(79);

	class Board
	{
	public:
		Board();

		void dropCoin(const int choice, const char coin);
		void display();
		std::vector<int> getAvailableMoves() const;
		bool checkMoveValid(const int player_move);
		bool checkWinner(const char coin);
		bool checkVertical(const char coin);
		bool checkHorizontal(const char coin);
		bool checkForwardDiagonal(const char coin);
		bool checkBackDiagonal(const char coin);

		char data[ROWS][COLS];
	};

	int makeRandomDecision(const Board& board);
	int playGame();

	inline char getCoin(const bool players_turn)
	{
		return players_turn ? connect4::player_coin : connect4::computer_coin;
	}

	//inline void copyState(Board& s1, const Board& s2)
	//{
	//	for (int i = 0; i < ROWS; i++)
	//		for (int j = 0; j < COLS; j++)
	//			s1[i][j] = s2[i][j];
	//}
} // end namespace connect4