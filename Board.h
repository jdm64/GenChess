#ifndef __BOARD_H__
#define __BOARD_H__

#include <cstring>
#include <iostream>
#include "Move.h"
#include "Defines.h"
#include "Position.h"

//#define DEBUG_MAKE_MOVE

extern const int pieceValue[16];

extern const int locValue[7][64];

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

	bool vply() const
	{
		return (curr == WHITE)? !(ply % 2) : (ply % 2);
	}

	int getPly() const
	{
		return ply;
	}

	void rebuildHash();

	void setBoard(Position pos);

	void reset();

	int isMate();

	bool incheck(const char color);

	void make(const Move move);

	void unmake(const Move move);

	void makeP(const Move move);

	void unmakeP(const Move move);

	bool validMove(const Move moveIn, Move &move);

	int validMove(string smove, const char color, Move &move);

	int eval() const;

	Position getPosition() const;

	int getNumMoves(const char color);

	MoveList* getMovesList(const char color);

	MoveList* getMovesList(const char color, const int movetype);

	MoveList* getPerftMovesList(const char color, const int movetype);

	string printSquare(const int index) const;

	void printBoard() const;

	void printPieceList() const;

	void dumpDebug() const;
};

#endif
