#pragma once
#include <stdlib.h>  //rand
#include <vector>

const int COLS = 7;
const int ROWS = 6;
typedef  char Board[ROWS][COLS];
constexpr auto MAX_COINS = ROWS*COLS;
constexpr char player_coin = char(254);
constexpr char computer_coin = char(79);

void initializeBoard(Board& board);
void dropCoin(Board& board, const int choice, const char coin);
void display(Board& board);
int makeRandomDecision(Board& board);
bool checkWinner(const Board& board, const char coin);
bool checkVertical(const Board& board, const char coin);
bool checkHorizontal(const Board& board, const char coin);
bool checkForwardDiagonal(const Board& board, const char coin);
bool checkBackDiagonal(const Board& board, const char coin);
std::string playGame();
