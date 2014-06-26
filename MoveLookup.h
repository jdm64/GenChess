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

extern int8 offsets[7][10];
extern DistDB ddb[120];

template<class MoveType>
class MoveLookup : public BB
{
protected:
	bool attackLine_Bishop(const DistDB &db, const int From, const int To) const;

	array<int8,28> genAll_Pawn(const int From) const;

	array<int8,28> genAll(const int From) const
	{
		const int type = ABS(square[From]);
		return type == PAWN? genAll_Pawn(From) : genAll_xPawn(From, type);
	}

	array<int8,28> genAll_xPawn(const int From, const int type) const
	{
		array<int8,28> list;
		int next = 0, i = 0;
		int8 *offset = offsets[type];

		switch (type) {
		case BISHOP:
		case ROOK:
		case QUEEN:
			for (; offset[i]; i++) {
				for (int to = From + offset[i]; !(to & 0x88); to += offset[i]) {
					if (square[to] == EMPTY) {
						list[next++] = to;
						continue;
					} else if (CAPTURE_MOVE(square[From], square[to])) {
						list[next++] = to;
					}
					break;
				}
			}
			break;
		case KNIGHT:
		case KING:
			for (; offset[i]; i++) {
				const int to = From + offset[i];
				if (to & 0x88)
					continue;
				else if (ANY_MOVE(square[From], square[to]))
					list[next++] = to;
			}
			break;
		}
		list[next] = -1;
		return list;
	}

	array<int8,28> genCapture_Pawn(const int From) const;

	array<int8,28> genCapture(const int From) const
	{
		const int type = ABS(square[From]);
		return type == PAWN? genCapture_Pawn(From) : genCapture_xPawn(From, type);
	}

	array<int8,28> genCapture_xPawn(const int From, const int type) const
	{
		array<int8,28> list;
		int next = 0, i = 0;
		int8* offset = offsets[type];

		switch (type) {
		case KNIGHT:
		case KING:
			for (; offset[i]; i++) {
				const int to = From + offset[i];
				if (to & 0x88)
					continue;
				else if (CAPTURE_MOVE(square[From], square[to]))
					list[next++] = to;
			}
			break;
		case BISHOP:
		case ROOK:
		case QUEEN:
			for (; offset[i]; i++) {
				for (int to = From + offset[i]; !(to & 0x88); to += offset[i]) {
					if (square[to] == EMPTY)
						continue;
					else if (CAPTURE_MOVE(square[From], square[to]))
						list[next++] = to;
					break;
				}
			}
			break;
		}
		list[next] = -1;
		return list;
	}

	array<int8,28> genMove_Pawn(const int From) const;

	array<int8,28> genMove(const int From) const
	{
		const int type = ABS(square[From]);
		return type == PAWN? genMove_Pawn(From) : genMove_xPawn(From, type);
	}

	array<int8,28> genMove_xPawn(const int From, const int type) const
	{
		array<int8,28> list;
		int next = 0, i = 0;
		int8 *offset = offsets[type];

		switch (type) {
		case KNIGHT:
		case KING:
			for (; offset[i]; i++) {
				const int to = From + offset[i];
				if (to & 0x88)
					continue;
				else if (square[to] == EMPTY)
					list[next++] = to;
			}
			break;
		case BISHOP:
		case ROOK:
		case QUEEN:
			for (; offset[i]; i++) {
				for (int to = From + offset[i]; !(to & 0x88); to += offset[i]) {
					if (square[to] == EMPTY)
						list[next++] = to;
					else
						break;
				}
			}
			break;
		}
		list[next] = -1;
		return list;
	}

	bool fromto_Pawn(const int From, const int to) const;

	bool fromto(const int From, const int To) const
	{
		if ((From | To) & 0x88)
			return false;

		const int type = ABS(square[From]);
		return type == PAWN? fromto_Pawn(From, To) : fromto_xPawn(From, To, type);
	}

	bool fromto_xPawn(const int From, const int To, const int type) const
	{
		int diff = ABS(From - To), n = 2;
		int8 *offset = offsets[type];

		switch (type) {
		case KNIGHT:
		case KING:
			for (int i = 0; i < 4; i++) {
				if (diff == offset[i])
					return ANY_MOVE(square[From], square[To]);
			}
			break;
		case QUEEN:
			n = 4;
		case BISHOP:
		case ROOK:
			for (int i = 0; i < n; i++) {
				if (diff % offset[i] == 0) {
					if (OWN_PIECE(square[From], square[To]))
						return false;
					i += ((To - From > 0)? 0 : n);
					for (int k = From + offset[i]; !(k & 0x88); k += offset[i]) {
						if (k == To)
							return true;
						else if (square[k] != EMPTY)
							return false;
					}
				}
			}
			break;
		}
		return false;
	}

	bool isAttacked_xBishop(const int From, const int FromColor) const
	{
		// ROOK
		int8 *offset = offsets[ROOK];
		for (; *offset; offset++) {
			for (int to = From + *offset, k = 1; !(to & 0x88); to += *offset, k++) {
				if (square[to] == EMPTY)
					continue;
				else if (OWN_PIECE(FromColor, square[to]))
					break;
				else if (k == 1 && ABS(square[to]) == KING)
					return true;
				else if (ABS(square[to]) == ROOK || ABS(square[to]) == QUEEN)
					return true;
				else
					break;
			}
		}
		// KNIGHT
		offset = offsets[KNIGHT];
		for (; *offset; offset++) {
			const int to = From + *offset;
			if (to & 0x88)
				continue;
			else if (OWN_PIECE(FromColor, square[to]))
				continue;
			else if (ABS(square[to]) == KNIGHT)
				return true;
		}
		return false;
	}

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

	bool isAttacked(const int From, const int FromColor) const;

	bool isAttacked(const int From) const
	{
		return isAttacked(From, square[From]);
	}
};

#endif
