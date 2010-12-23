#ifndef __BOARD_H__
#define __BOARD_H__

#include "Position.h"

//#define DEBUG_MAKE_MOVE

extern const int pieceValue[16];

extern const int locValue[7][64];

class Board {
private:
	char square[64];

	char piece[32];

	uint64 key;

	int ply;

	char curr;

	int pieceIndex(const int loc, const int type) const;

	int pieceIndex(const int loc) const;

public:
	Board();

	uint64 hash() const
	{
		return key;
	}

	int getPly() const
	{
		return ply;
	}

	char currPlayer() const
	{
		return curr;
	}

	void reset();

	void rebuildHash();

	void setBoard(Position pos);

	Position getPosition() const;

	void make(const Move move);

	void unmake(const Move move);

	void makeP(const Move move);

	void unmakeP(const Move move);

	bool incheck(const char color);

	int isMate();

	bool validMove(const Move moveIn, Move &move);

	int validMove(const string smove, const char color, Move &move);

	int eval() const;

	int getNumMoves(const char color);

	MoveList* getMoveList(const char color);

	MoveList* getMoveList(const char color, const int movetype);

	MoveList* getPerftMoveList(const char color, const int movetype);

	string printSquare(const int index) const;

	void printBoard() const;

	void printPieceList() const;

	void dumpDebug() const;
};

#endif
