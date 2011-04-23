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
#define TT_ENABLED

extern const int pieceValue[16];

extern const int locValue[7][64];

class Board {
private:
	int8 square[64];

	int8 piece[32];

	uint64 key;

	int ply;

	int8 stm;

	int pieceIndex(const int8 loc, const int8 type) const;

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

	int8 getStm() const
	{
		return stm;
	}

	void reset();

	void rebuildHash();

	void setBoard(const Position &pos);

	Position getPosition() const;

	void make(const Move &move);

	void unmake(const Move &move);

	void makeP(const Move &move);

	void unmakeP(const Move &move);

	bool incheck(const int8 color);

	int isMate();

	bool validMove(const Move &moveIn, Move &move);

	int validMove(const string smove, const int8 color, Move &move);

	int eval() const;

	bool anyMoves(const int8 color);

	MoveList* getMoveList(const int8 color);

	MoveList* getMoveList(const int8 color, const int movetype);

	MoveList* getPerftMoveList(const int8 color, const int movetype);

	string printSquare(const int index) const;

	void printBoard() const;

	void printPieceList() const;

	void dumpDebug() const;
};

#endif
