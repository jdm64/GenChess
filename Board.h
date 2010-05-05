#ifndef __BOARD_H__
#define __BOARD_H__

#include <vector>
#include <string>
#include <iostream>
#include "Data.h"
#include "MoveLookup.h"

using namespace std;

class Board {
private:
	static const char pieceSymbol[7];

	static const int pieceValues[16];

	vector<Move> history;

	char board[64];

	Piece pieces[32];

	char curr;

	int hcounter;

	int validateMove(Move move, char color);

	int CalcScore();

	string printSquare(int index);

	string printLoc(const char loc);

public:
	Board();

	char currentPlayer();

	void newGame();

	void quitGame();

	int pieceIndex(int loc, int type);

	int pieceIndex(int loc);

	bool inCheck(char color);

	void doMove(Move move);

	void undo(Move move);

	int doMove(Move move, char color);

	bool undo();

	int getNumMoves(char color);

	MovesPly* getMovesList(char color, bool getScore = false);

	string printMove(Move move);

	void printBoard();

	void printPieceList();
};

#endif