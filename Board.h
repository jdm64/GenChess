/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Board class declarations
 */

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
	vector<Move> history;

	char board[64];

	Piece pieces[32];

	char curr;

	int hcounter;

	int validateMove(Move move, char color);

	string printSquare(int index);

	string printLoc(const char loc);

public:
	Board();

	static const char pieceSymbol[7];

	static const int pieceValues[16];

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

	int CalcScore();

	int getNumMoves(char color);

	MovesPly* getMovesList(char color);

	string printMove(Move move);

	void printBoard();

	void printPieceList();
};

#endif