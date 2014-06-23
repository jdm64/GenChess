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

#include <cassert>
#include "Move.h"
#include "MoveLookup.h"

template<>
array<int8,28> MoveLookup<RegMove>::genAll(const int From) const
{
	const int type = ABS(square[From]);

	if (type == PAWN) {
		array<int8,28> list;
		int next = 0;
		if (square[From] == WHITE_PAWN) { // WHITE
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From + 15]))
				list[next++] = From + 15;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From + 17]))
				list[next++] = From + 17;
			if (!square[From + 16]) {
				list[next++] = From + 16;
				if (From <= H2 && !square[From + 32])
					list[next++] = From + 32;
			}
		} else { // BLACK
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From - 17]))
				list[next++] = From - 17;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From - 15]))
				list[next++] = From - 15;
			if (!square[From - 16]) {
				list[next++] = From - 16;
				if (From >= A7 && !square[From - 32])
					list[next++] = From - 32;
			}
		}
		list[next] = -1;
		return list;
	} else {
		return genAll_xPawn(offsets[type], From, type);
	}
}

template<>
array<int8,28> MoveLookup<RegMove>::genCapture(const int From) const
{
	const int type = ABS(square[From]);

	if (type == PAWN) {
		array<int8,28> list;
		int next = 0;
		if (square[From] == WHITE_PAWN) { // WHITE
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From + 15]))
				list[next++] = From + 15;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From + 17]))
				list[next++] = From + 17;
		} else { // BLACK
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From - 17]))
				list[next++] = From - 17;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From - 15]))
				list[next++] = From - 15;
		}
		list[next] = -1;
		return list;
	} else {
		return genCapture_xPawn(offsets[type], From, type);
	}
}

template<>
array<int8,28> MoveLookup<RegMove>::genMove(const int From) const
{
	const int type = ABS(square[From]);

	if (type == PAWN) {
		array<int8,28> list;
		int next = 0;
		if (square[From] == WHITE_PAWN) { // WHITE
			if (!square[From + 16]) {
				list[next++] = From + 16;
				if (From <= H2 && !square[From + 32])
					list[next++] = From + 32;
			}
		} else { // BLACK
			if (!square[From - 16]) {
				list[next++] = From - 16;
				if (From >= A7 && !square[From - 32])
					list[next++] = From - 32;
			}
		}
		list[next] = -1;
		return list;
	} else {
		return genMove_xPawn(offsets[type], From, type);
	}
}

template<>
bool MoveLookup<RegMove>::fromto(const int From, const int To) const
{
	const int type = ABS(square[From]);

	if (type == PAWN) {
		if (square[From] == WHITE_PAWN) { // WHITE
			if (From + 15 == To && COL(From) != 0 && CAPTURE_MOVE(square[From], square[From + 15]))
				return true;
			if (From + 17 == To && COL(From) != 7 && CAPTURE_MOVE(square[From], square[From + 17]))
				return true;
			if (!square[From + 16]) {
				if (From + 16 == To)
					return true;
				if (From + 32 == To && From <= H2 && !square[From + 32])
					return true;
			}
		} else { // BLACK
			if (From - 17 == To && COL(From) != 0 && CAPTURE_MOVE(square[From], square[From - 17]))
				return true;
			if (From - 15 == To && COL(From) != 7 && CAPTURE_MOVE(square[From], square[From - 15]))
				return true;
			if (!square[From - 16]) {
				if (From - 16 == To)
					return true;
				else if (From - 32 == To && From >= A7 && !square[From - 32])
					return true;
			}
		}
	} else {
		return fromto_xPawn(From, To, type, offsets[type]);
	}
	return false;
}

template<>
bool MoveLookup<RegMove>::attackLine_Bishop(const DistDB &db, const int From, const int To) const
{
	int offset = db.step * ((To > From)? 1:-1);
	for (int to = From + offset, k = 1; !(to & 0x88); to += offset, k++) {
		if (square[to] == EMPTY) {
			continue;
		} else if (OWN_PIECE(square[From], square[to])) {
			return false;
		} else if (ABS(square[to]) == BISHOP || ABS(square[to]) == QUEEN) {
			return true;
		} else if (k == 1) {
			if (ABS(square[to]) == PAWN && square[From] * (to - From) > 0)
				return true;
			else if (ABS(square[to]) == KING)
				return true;
		}
		break;
	}
	return false;
}

template<>
bool MoveLookup<RegMove>::isAttacked(const int From, const int FromColor) const
{
	int8 *offset = offsets[BISHOP];
	for (; *offset; offset++) {
		for (int to = From + *offset, k = 1; !(to & 0x88); to += *offset, k++) {
			if (square[to] == EMPTY) {
				continue;
			} else if (OWN_PIECE(FromColor, square[to])) {
				break;
			} else if (ABS(square[to]) == BISHOP || ABS(square[to]) == QUEEN) {
				return true;
			} else if (k == 1) {
				if (ABS(square[to]) == PAWN && FromColor * (to - From) > 0)
					return true;
				else if (ABS(square[to]) == KING)
					return true;
			}
			break;
		}
	}
	return isAttacked_xBishop(From, FromColor);
}
