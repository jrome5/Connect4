#include "connect4.h"
#include "MCTS.h"
#include "RootParallelisation.h"
#include "LeafParallelisation.h"
#include "LocalMutexMCTS.h"
#include <iostream>
#include <stdlib.h>  //rand
#include <fstream>
#include <time.h>
#include <numeric>

using std::cout;
int plays = 0;
std::vector<double> computation_times;

namespace connect4
{
	Board::Board()
	{
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
				data[i][j] = ' ';
		}
	}

	void Board::dropCoin(const int choice, const char coin)
	{
		for (int i = 0; i < ROWS; i++)
		{
			if (data[i][choice] == ' ')
			{
				data[i][choice] = coin;
				return;
			}
		}
	}

	void Board::display()
	{
		system("cls");
		cout << "Game No: " << plays << std::endl;
		cout << " 0   1   2   3   4   5   6\n";
		for (int i = ROWS - 1; i >= 0; i--)
		{
			for (int j = 0; j < COLS; j++) cout << char(218) << char(196) << char(191) << " ";
			cout << '\n';
			for (int j = 0; j < COLS; j++) cout << char(179) << data[i][j] << char(179) << " ";
			cout << '\n';
			for (int j = 0; j < COLS; j++) cout << char(192) << char(196) << char(217) << " ";
			cout << '\n';
		}
	}

	std::vector<int> Board::getAvailableMoves() const
	{
		std::vector<int> available_moves;
		for (int j = 0; j < COLS; j++)
			if (data[ROWS - 1][j] == ' ')
			{
				available_moves.push_back(j);
         	}
		return available_moves;
	}

	bool Board::checkMoveValid(const int player_move)
	{
		const auto available_moves = getAvailableMoves();
		for (auto move : available_moves)
		{
			if (move == player_move)
				return true;
		}
		return false;
	}

	bool Board::checkWinner(const char coin)
	{
		if (checkVertical(coin))
			return true;
		if (checkHorizontal(coin))
			return true;
		if (checkForwardDiagonal(coin))
			return true;
		if (checkBackDiagonal(coin))
			return true;
		return false;
	}

	bool Board::checkForwardDiagonal(const char coin)
	{
		for (int i = 0; i < ROWS - 3; i++)
		{
			for (int j = 0; j < COLS - 3; j++)
			{
				if (data[i + 3][j] == data[i + 2][j + 1]
					&& data[i + 2][j + 1] == data[i + 1][j + 2]
					&& data[i + 1][j + 2] == data[i][j + 3]
					&& data[i][j + 3] == coin)
					return true;
			}
		}
		return false;
	}
	bool Board::checkBackDiagonal(const char coin)
	{
		for (int i = 0; i < ROWS - 3; i++)
		{
			for (int j = 0; j < COLS - 3; j++)
			{
				if (data[i][j] == data[i + 1][j + 1]
					&& data[i + 1][j + 1] == data[i + 2][j + 2]
					&& data[i + 2][j + 2] == data[i + 3][j + 3]
					&& data[i + 3][j + 3] == coin)
					return true;
			}
		}
		return false;
	}

	bool Board::checkHorizontal(const char coin)
	{
		for (int i = 0; i < ROWS; i++)
		{
			int coin_coint = 0;
			for (int j = 0; j < COLS; j++)
			{
				if (data[i][j] == coin)
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

	bool Board::checkVertical(const char coin)
	{
		for (int j = 0; j < COLS; j++)
		{
			int coin_count = 0;
			for (int i = 0; i < ROWS; i++)
			{
				if (data[i][j] == ' ')
				{
					continue;
				}
				if (data[i][j] == coin)
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

	int makeRandomDecision(const Board& board)
	{
		std::vector<int> available_moves = board.getAvailableMoves();
		if (available_moves.size() == 0)
		{
			return -1;
		}
		const int random_choice = rand() % available_moves.size();
		return available_moves[random_choice];
	}

	int playGame()
	{
		Board board;
		board.display();
		int coin_count = 0;
		int choice;
		int max_turns = 42;
		MCTS::MCTS base;
		MCTS::LeafParallelisation mod;
		while (coin_count < max_turns)
		{
			//	cout << turns_remaining;
	//base choose
			cout << "Base is choosing";
			choice = base.search(board, coin_count);
			if (choice == -1)
			{
				cout << "this shouldnt happen";
				return 0;
			}
			board.dropCoin(choice, computer_coin);
			coin_count++;
			board.display();
			if (board.checkWinner(computer_coin))
			{
				return -1;
			}
		//mod choose
		cout << "Mod is choosing";
		const time_t start = time(0); //record move time
		choice = MCTS::LocalMutexMCTS::search(board, coin_count);
		computation_times.push_back(difftime(time(0), start));
		if (choice == -1)
		{
			cout << "this shouldnt happen";
			return 0;
		}
		board.dropCoin(choice, player_coin);
		coin_count++;
		board.display();
		if (board.checkWinner(player_coin))
		{
			return 1;
		}
	}
		return 0;
	}
} //end namespace connect4

int main()
{
	std::ofstream results_file;
	const auto filename = "Test Leaf Parallelisation1.csv";
	results_file.open(filename, std::ios::app);
	results_file << "Base first 4 threads\n";
	results_file << "Game, Win, Draw, Loss, Avg Comp Time\n";
	//char replay = 'y';
	if (not results_file.is_open())
	{
		std::cout << "Cannot write to file";
		return 0;
	}
	for(int i = 0; i < 100; i++)
	{
		if (not results_file.is_open())
		{
			std::cout << "Cannot write to file";
			return 0;
		}
		plays = i;
		results_file << i;
		const auto winner = connect4::playGame();
		switch (winner)
		{
			case -1:
				results_file << ",0,0,1,";
				break;
			case 1:
				results_file << ",1,0,0,";
				break;
			default:
				results_file << ",0,1,0,";
				break;
		}
		for (const double time : computation_times)
		{
			results_file << time << ",";
		}
		results_file << std::endl;

		if (plays % 10 == 0)
		{
			results_file.close();
			results_file.open(filename, std::ios::app);
		}
		computation_times.clear();
	}
	return 0;
}