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
#include <iostream>
#include <cstring>

using namespace std;

extern const int pieceValue[16];

extern const int locValue[7][64];

void printPieceList(const char *piece);

struct Position {
	char piece[32];
	char curr;

	bool operator==(const Position &b)
	{
		if (curr != b.curr)
			return false;
		char board[66];

		memset(board, 0, 66);
		for (int i = 0; i < 32; i++)
			board[b.piece[i] + 2] += pieceType[i];
		for (int i = 0; i < 32; i++)
			board[piece[i] + 2] -= pieceType[i];
		for (int i = 0; i < 66; i++) {
			if (board[i])
				return false;
		}
		return true;
	}
	void print()
	{
		cout << "curr: " << (int)curr << endl;
		printPieceList(piece);
	}
};

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

	void initHash(uint64 hash);

	void reset();

	int isMate();

	bool incheck(const char color);

	void make(const Move move);

	void unmake(const Move move);

	void makeP(const Move move);

	void unmakeP(const Move move);

	bool validMove(Move &move);

	int validMove(string smove, const char color, Move &move);

	int eval() const;

	Position getPosition();

	int getNumMoves(const char color);

	MoveList* getMovesList(const char color);

	MoveList* getPerftMovesList(const char color, const int movetype);

	string printSquare(const int index) const;

	void printBoard() const;

	void dumpDebug() const;
};

#endif
