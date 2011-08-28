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

class GenBoard : public GenPosition
{
private:
	uint64 key;

	int pieceIndex(const int8 loc, const int8 type) const;

	void rebuildHash();

public:
	GenBoard();

	inline uint64 hash() const
	{
		return key;
	}

	inline int getPly() const
	{
		return ply;
	}

	inline int8 getStm() const
	{
		return stm;
	}

	void reset();

	void setBoard(const GenPosition &pos);

	GenPosition getPosition() const;

	void make(const GenMove &move);

	void unmake(const GenMove &move);

	void makeP(const GenMove &move);

	void unmakeP(const GenMove &move);

	int isMate();

	bool validMove(const GenMove &moveIn, GenMove &move);

	int validMove(const string &smove, const int8 color, GenMove &move);

	int eval() const;

	bool anyMoves(const int8 color);

	void getPlaceMoveList(GenMoveList* const data, const int8 pieceType);

	void getMoveList(GenMoveList* const data, const int8 color, const int movetype);

	GenMoveList* getMoveList(const int8 color, const int movetype);

	GenMoveList* getPerftMoveList(const int8 color, const int movetype);

	GenMoveList* getPerftMoveList(const int8 color);

	string printPieceList() const;

	void dumpDebug() const;
};

// --- Start Regular Chess --

extern const int regPieceValue[7];

extern const int regLocValue[7][64];

struct HistoryNode
{
	RegMove move;
	MoveFlags flags;
};

class RegBoard : public RegPosition {
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
	RegBoard();

	inline uint64 hash() const
	{
		return key;
	}

	inline int getPly() const
	{
		return ply;
	}

	inline int8 getStm() const
	{
		return stm;
	}

	inline MoveFlags getMoveFlags() const
	{
		return flags;
	}

	void reset();

	void setBoard(RegPosition pos);

	RegPosition getPosition() const;

	void make(const RegMove &move);

	void unmake(const RegMove &move, const MoveFlags &undoFlags);

	void makeP(const RegMove &move);

	void unmakeP(const RegMove &move, const MoveFlags &undoFlags);

	int isMate();

	bool validMove(const RegMove &moveIn, RegMove &move);

	int validMove(const string &smove, const int8 color, RegMove &move);

	int eval() const;

	bool anyMoves(const int8 color);

	void getMoveList(RegMoveList *data, const int8 color, int movetype);

	void getCastleMoveList(RegMoveList *data, const int8 color);

	void getEnPassantMoveList(RegMoveList *data, const int8 color);

	RegMoveList* getMoveList(const int8 color, const int movetype);

	RegMoveList* getPerftMoveList(const int8 color);

	string printPieceList() const;

	void dumpDebug() const;
};

template<class MoveType>
struct Board;

template<>
struct Board<GenMove> { typedef GenBoard type; };

template<>
struct Board<RegMove> { typedef RegBoard type; };

#endif
