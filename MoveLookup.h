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

#ifndef __MOVE_LOOKUP_H__
#define __MOVE_LOOKUP_H__

#include "BaseBoard.h"

#define CAPTURE_MOVE(A, B)	(A * B <  0)
#define ANY_MOVE(A, B)		(A * B <= 0)
#define NOT_CAPTURE(A, B)	(A * B >= 0)
#define OWN_PIECE(A, B)		(A * B >  0)

using namespace std;

struct DistDB
{
	int8 step;
	int8 type;
};

extern DistDB ddb[120];

template<class MoveType>
class MoveLookup : public BB
{
protected:
	int genAll_xPawn(int8* const list, int8* const offset, const int From, const int type) const;

	int genCapture_xPawn(int8* const list, int8* const offset, const int From, const int type) const;

	int genMove_xPawn(int8* const list, int8* const offset, const int From, const int type) const;

	bool fromto_xPawn(const int From, const int To, const int type, int8* const offset) const;

	bool attackLine_Bishop(const DistDB &db, const int From, const int To) const;

	bool isAttacked_xBishop(const int From, const int FromColor) const;

	int8* genAll(const int From) const;

	int8* genCapture(const int From) const;

	int8* genMove(const int From) const;

	bool fromto(const int From, const int To) const;

	bool attackLine(const int From, const int To) const
	{
		if ((From | To) & 0x88)
			return false;

		int diff = ABS(From - To);

		if (!ddb[diff].step)
			return false;

		DistDB db = ddb[diff];
		switch (db.type) {
		case KNIGHT:
			return (ABS(square[To]) == KNIGHT && CAPTURE_MOVE(square[From], square[To]));
		case BISHOP:
			return attackLine_Bishop(db, From, To);
		case ROOK:
			int offset = db.step * ((To > From)? 1:-1);
			for (int to = From + offset, k = 1; !(to & 0x88); to += offset, k++) {
				if (square[to] == EMPTY)
					continue;
				else if (OWN_PIECE(square[From], square[to]))
					break;
				else if (k == 1 && ABS(square[to]) == KING)
					return true;
				else if (ABS(square[to]) == ROOK || ABS(square[to]) == QUEEN)
					return true;
				else
					break;
			}
			break;
		}
		return false;
	}

	bool isAttacked(const int From) const;

	bool isAttacked(const int From, const int FromColor) const;
};

#endif
