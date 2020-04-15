#include "connect4.h"
#include "MCTS.h"
#include "DecisiveMCTS.h"
#include <iostream>
#include <stdlib.h>  //rand
#include <fstream>

using std::cout;
int plays;

namespace connect4
{
	void initializeBoard(Board& board)
	{
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
				board[i][j] = ' ';
		}
	}

	void dropCoin(Board& board, const int choice, const char coin)
	{
		for (int i = 0; i < ROWS; i++)
		{
			if (board[i][choice] == ' ')
			{
				board[i][choice] = coin;
				return;
			}
		}
	}

	void display(const Board& board)
	{
		system("cls");
		cout << "Game No: " << plays << std::endl;
		cout << " 0   1   2   3   4   5   6\n";
		for (int i = ROWS - 1; i >= 0; i--)
		{
			for (int j = 0; j < COLS; j++) cout << char(218) << char(196) << char(191) << " ";
			cout << '\n';
			for (int j = 0; j < COLS; j++) cout << char(179) << board[i][j] << char(179) << " ";
			cout << '\n';
			for (int j = 0; j < COLS; j++) cout << char(192) << char(196) << char(217) << " ";
			cout << '\n';
		}
	}

	int makeRandomDecision(const Board& board)
	{
		std::vector<int> available_moves = getAvailableMoves(board);
		if (available_moves.size() == 0)
		{
			return -1;
		}
		const int random_choice = rand() % available_moves.size();
		return available_moves[random_choice];
	}

	std::vector<int> getAvailableMoves(const Board& board)
	{
		std::vector<int> available_moves;
		for (int j = 0; j < COLS; j++)
			if (board[ROWS - 1][j] == ' ')
			{
				available_moves.push_back(j);
         	}
		return available_moves;
	}

	bool checkMoveValid(const Board& board, const int player_move)
	{
		const auto available_moves = getAvailableMoves(board);
		for (auto move : available_moves)
		{
			if (move == player_move)
				return true;
		}
		return false;
	}

	bool checkWinner(const Board& board, const char coin)
	{
		if (checkVertical(board, coin))
			return true;
		if (checkHorizontal(board, coin))
			return true;
		if (checkForwardDiagonal(board, coin))
			return true;
		if (checkBackDiagonal(board, coin))
			return true;
		return false;
	}

	bool checkForwardDiagonal(const Board& board, const char coin)
	{
		for (int i = 0; i < ROWS - 3; i++)
		{
			for (int j = 0; j < COLS - 3; j++)
			{
				if (board[i + 3][j] == board[i + 2][j + 1]
					&& board[i + 2][j + 1] == board[i + 1][j + 2]
					&& board[i + 1][j + 2] == board[i][j + 3]
					&& board[i][j + 3] == coin)
					return true;
			}
		}
		return false;
	}
	bool checkBackDiagonal(const Board& board, const char coin)
	{
		for (int i = 0; i < ROWS - 3; i++)
		{
			for (int j = 0; j < COLS - 3; j++)
			{
				if (board[i][j] == board[i + 1][j + 1]
					&& board[i + 1][j + 1] == board[i + 2][j + 2]
					&& board[i + 2][j + 2] == board[i + 3][j + 3]
					&& board[i + 3][j + 3] == coin)
					return true;
			}
		}
		return false;
	}

	bool checkHorizontal(const Board& board, const char coin)
	{
		for (int i = 0; i < ROWS; i++)
		{
			int coin_coint = 0;
			for (int j = 0; j < COLS; j++)
			{
				if (board[i][j] == coin)
				{
					coin_coint++;
					if (coin_coint == 4)
					{
						return true;
					}
				}
				else
				{
					coin_coint = 0;
				}
			}
		}
		return false;
	}

	bool checkVertical(const Board& board, const char coin)
	{
		for (int j = 0; j < COLS; j++)
		{
			int coin_count = 0;
			for (int i = 0; i < ROWS; i++)
			{
				if (board[i][j] == ' ')
				{
					continue;
				}
				if (board[i][j] == coin)
				{
					coin_count++;
					if (coin_count == 4)
					{
						return true;
					}
				}
				else
				{
					coin_count = 0;
				}
			}
		}
		return false;
	}

	int playGame()
	{
		Board board;
		initializeBoard(board);
		display(board);
		int coin_count = 42;
		MCTS::MCTS base;
		MCTS::DecisiveMCTS modified;
		while (true)
		{
			if (coin_count == 0)
			{
				return 0;
			}
			//base choose
			cout << "Base is choosing";
			int base_choice = base.search(board, coin_count);
			bool move_valid = false;
			dropCoin(board, base_choice, player_coin);
			coin_count--;
			display(board);
			if (checkWinner(board, player_coin))
			{
				return -1;
			}
		

			//mod choose
			cout << "Modified is choosing";
			const int computer_choice = modified.search(board, coin_count);
			dropCoin(board, computer_choice, computer_coin);
			coin_count--;
			display(board);
			if (checkWinner(board, computer_coin))
			{
				return 1;
			}
		}
	}
} //end namespace connect4

int main()
{
	std::ofstream results_file;
	const auto file_name = "MCTS 2s vs DecisiveMCTS 2s 100 plays.csv";
	results_file.open(file_name, std::ios::app);
	results_file << "MCTS first\n";
	results_file << "Game, Win, Draw, Loss\n";
	for(int i = 0; i < 100; i++)
	{
		plays = i;
		const auto winner = connect4::playGame();
		switch (winner)
		{
			case -1:
				results_file << i << ",0,0,1\n";
				break;
			case 1:
				results_file << i << ",1,0,0\n";
				break;
			default:
				results_file << i << ",0,1,0\n";
				break;
		}
		if (i % 10 == 0)
		{
			results_file.close();
			results_file.open("MCTS vs random 100 plays.csv");
		}
	}
	results_file.close();
	return 0;
}