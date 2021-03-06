#include <iostream>
#include "connect4.h"
using namespace std;

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

void display(Board& board)
{
	system("cls");
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

int makeRandomDecision(Board& board)
{
	vector<int> available_moves;
	for (int j = 0; j < COLS; j++)
		if (board[ROWS - 1][j] == ' ')
			available_moves.push_back(j);
	if (available_moves.size() == 0)
	{
		return -1;
	}
	const int random_choice = rand() % available_moves.size();
	return available_moves[random_choice];
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

string playGame()
{
	Board board;
	initializeBoard(board);
	display(board);
	int coin_count = 0;
	bool no_winner = true;
	bool players_turn = false;

	while (true)
	{
		if (coin_count == MAX_COINS)
		{
			return "draw";
		}
		//player choose
		players_turn = true;
		cout << "Please enter value 0->6\n";
		int player_choice;
		cin >> player_choice;
		dropCoin(board, player_choice, player_coin);
		coin_count++;
		display(board);
		if (checkWinner(board, player_coin))
		{
			return "player";
		}
		//computer choose
		players_turn = false;
		cout << "Computer is choosing";
		const int computer_choice = makeRandomDecision(board);
		if (computer_choice == -1)
		{
			cout << "this shouldnt happen";
			return "draw";
		}
		dropCoin(board, computer_choice, computer_coin);
		coin_count++;
		display(board);
		if (checkWinner(board, computer_coin))
		{
			return "computer";
		}
	}
}

int main()
{
	char replay = 'y';
	while (replay == 'y')
	{
		const string winner = playGame();
		if (winner == "draw")
		{
			cout << "Draw. No winner.";
		}
		else
		{
			cout << "The winner is " << winner << "!!!\n";
		}
		cout << "Play again? y/n \n";
		cin >> replay;
	}
	return 1;
}