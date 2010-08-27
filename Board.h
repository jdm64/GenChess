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
#include "Move.h"
#include "MoveLookup.h"

using namespace std;

extern const int pieceValue[16];

class Board {
private:
	char square[64];

	char piece[32];

	int ply;

	char curr;

	int pieceIndex(const int loc, const int type) const;

	int pieceIndex(const int loc) const;

public:
	Board();

	char currPlayer();

	void reset();

	int isMate();

	bool incheck(const char color);

	void make(const Move move);

	void unmake(const Move move);

	bool validMove(const Move move);

	int validMove(string smove, const char color, Move &move);

	int eval() const;

	int getNumMoves(const char color);

	MoveList* getMovesList(const char color);

	string printSquare(const int index) const;

	void printBoard() const;
};

#endif
