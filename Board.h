/*	GenChess, a genesis chess engine
	Copyright (C) 2010, Justin Madru (jdm64@gawab.com)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
