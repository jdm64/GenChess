/* GenChess, a genesis chess engine
 * Copyright (C) 2010-2014, Justin Madru (justin.jdm64@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <sstream>
#include "Position.h"

extern const int genPieceValue[7];

extern const int genLocValue[7][64];

extern const int regPieceValue[7];

extern const int regLocValue[7][64];

template<class MoveType>
class Board : public Position<MoveType>
{
private:
	uint64 key;
	int mscore;

	int pieceIndex(const int loc, const int type) const;

	static bool isPromote(const RegMove &move, int color)
	{
		return (color == WHITE)? (move.to >= A8) : (move.to <= H1);
	}

	bool incheckMove(const MoveType &move, const int color, const bool stmCk) const
	{
		const int king = (color == WHITE)? 31:15;
		if (stmCk || move.index == king)
			return Position<MoveType>::incheck(color);
		else
			return MoveLookup<MoveType>::attackLine(BB::piece[king], move.from)
				|| MoveLookup<MoveType>::attackLine(BB::piece[king], move.to);
	}

	int validCastle(RegMove &move, const int color);

	int validEnPassant(RegMove &move, const int color);

	void rebuildHash();

	void rebuildScore();

public:
	Board()
	{
		reset();
	}

	uint64 hash() const
	{
		return key;
	}

	int getPly() const
	{
		return BB::ply;
	}

	int getStm() const
	{
		return BB::stm;
	}

	MoveFlags getMoveFlags() const
	{
		return BB::flags;
	}

	void reset();

	void setBoard(const Position<MoveType> &pos);

	void make(const MoveType &move);

	void unmake(const MoveType &move, const MoveFlags &undoFlags);

	int isMate()
	{
		if (anyMoves(BB::stm))
			return NOTMATE;
		else if (Position<MoveType>::incheck(BB::stm))
			return (BB::stm == WHITE)? BLACK_CHECKMATE : WHITE_CHECKMATE;
		else
			return STALEMATE;
	}

	bool validMove(const MoveType &moveIn, MoveType &move);

	int validMove(const string &smove, const int color, MoveType &move);

	int eval() const
	{
		return (BB::stm == WHITE)? -mscore : mscore;
	}

	bool anyMoves(const int color);

	void getPlaceMoveList(GenMoveList* const data, const int pieceType);

	void getCastleMoveList(RegMoveList *data, const int color);

	void getEnPassantMoveList(RegMoveList *data, const int color);

	void getMoveList(MoveList<MoveType>* const data, const int color, const MoveClass movetype);

	MoveList<MoveType>* getMoveList(const int color, const MoveClass movetype);

	string printPieceList() const
	{
		stringstream buff;
		string tmp;

		buff << "White:\t";
		for (int i = 16; i < 32; i++) {
			if (i != 16 && !(i % 8))
				buff << "\n\t";
			buff << pieceSymbol[BB::piecetype[i]] << "(";
			tmp = printLoc(BB::piece[i]);
			if (tmp.length() == 2)
				buff << ' ' << tmp << ' ';
			else
				buff << tmp;
			buff << ") ";
		}
		buff << "\nBlack:\t";
		for (int i = 0; i < 16; i++) {
			if (i && !(i % 8))
				buff << "\n\t";
			buff << pieceSymbol[-BB::piecetype[i]] << "(";
			tmp = printLoc(BB::piece[i]);
			if (tmp.length() == 2)
				buff << ' ' << tmp << ' ';
			else
				buff << tmp;
			buff << ") ";
		}
		return buff.str();
	}
};

template<class MoveType>
struct HistoryNode
{
	MoveType move;
	MoveFlags flags;
};

typedef Board<GenMove> GenBoard;
typedef Board<RegMove> RegBoard;

#endif
