/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Board class declarations
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "Defines.h"
#include "Move.h"

using namespace std;

extern const int pieceValue[16];

extern const int locValue[6][64];

class Board {
private:
	uint64 key;

	char square[64];

	char piece[32];

	int ply;

	char curr;

	int pieceIndex(const int loc, const int type) const;

	int pieceIndex(const int loc) const;

public:
	Board();

	char currPlayer() const
	{
		return curr;
	}

	uint64 hash() const
	{
		return key;
	}

	void initHash(uint64 hash);

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
