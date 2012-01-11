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

extern const int genPieceValue[16];

extern const int genLocValue[7][64];

extern const int regPieceValue[7];

extern const int regLocValue[7][64];

template<class Type>
class Board : public Position<Type>
{
private:
	uint64 key;

	int pieceIndex(const int8 loc, const int8 type) const;

	inline bool isPromote(const RegMove &move, int8 color) const
	{
		return (color == WHITE)?
			(move.to <= H8 && move.from <= H7) :
			(move.to >= A1 && move.from >= A2);
	}

	int validCastle(RegMove &move, const int8 color);

	int validEnPassant(RegMove &move, const int8 color);

	void validateBoard(const RegMove &move) const;

	void rebuildHash();

public:
	Board();

	uint64 hash() const
	{
		return key;
	}

	int getPly() const
	{
		return BaseBoard::ply;
	}

	int8 getStm() const
	{
		return BaseBoard::stm;
	}

	MoveFlags getMoveFlags() const
	{
		return BaseBoard::flags;
	}

	void reset();

	void setBoard(const Position<Type> &pos);

	void make(const Type &move);

	void unmake(const Type &move);

	void unmake(const RegMove &move, const MoveFlags &undoFlags);

	int isMate();

	bool validMove(const Type &moveIn, Type &move);

	int validMove(const string &smove, const int8 color, Type &move);

	int eval() const;

	bool anyMoves(const int8 color);

	void getPlaceMoveList(GenMoveList* const data, const int8 pieceType);

	void getCastleMoveList(RegMoveList *data, const int8 color);

	void getEnPassantMoveList(RegMoveList *data, const int8 color);

	void getMoveList(MoveList<Type>* const data, const int8 color, const MoveType movetype);

	MoveList<Type>* getMoveList(const int8 color, const MoveType movetype);

	string printPieceList() const;

	void dumpDebug() const;
};

template<class Type>
struct HistoryNode
{
};

template<>
struct HistoryNode<GenMove>
{
	GenMove move;
};

template<>
struct HistoryNode<RegMove>
{
	RegMove move;
	MoveFlags flags;
};

typedef Board<GenMove> GenBoard;
typedef Board<RegMove> RegBoard;

#endif
