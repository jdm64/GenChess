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
array<int8,28> MoveLookup<GenMove>::genAll_Pawn(const int From) const
{
	array<int8,28> list;
	int next = 0;
	int8 *offset = offsets[PAWN];

	for (bool evn = true; *offset; offset++, evn ^= true) {
		const int to = From + *offset;
		if (to & 0x88)
			continue;
		const bool val = evn? CAPTURE_MOVE(square[From], square[to]) : (square[to] == EMPTY);
		if (val)
			list[next++] = to;
	}
	list[next] = -1;
	return list;
}

template<>
array<int8,28> MoveLookup<GenMove>::genCapture_Pawn(const int From) const
{
	array<int8,28> list;
	int next = 0;

	for (int8 *offset = offsets[PAWN]; *offset; offset += 2) {
		const int to = From + *offset;
		if (to & 0x88)
			continue;
		else if (CAPTURE_MOVE(square[From], square[to]))
			list[next++] = to;
	}
	list[next] = -1;
	return list;
}

template<>
array<int8,28> MoveLookup<GenMove>::genMove_Pawn(const int From) const
{
	array<int8,28> list;
	int next = 0;

	for (int8 *offset = &offsets[PAWN][1]; *offset; offset += 2) {
		const int to = From + *offset;
		if (to & 0x88)
			continue;
		else if (square[to] == EMPTY)
			list[next++] = to;
	}
	list[next] = -1;
	return list;
}

template<>
bool MoveLookup<GenMove>::fromto_Pawn(const int From, const int To) const
{
	const int diff = ABS(From - To);
	for (int i = 0; i < 4; i++) {
		if (diff == offsets[PAWN][i])
			return ((i%2)? (square[To] == EMPTY) : CAPTURE_MOVE(square[From], square[To]));
	}
	return false;
}

template<>
bool MoveLookup<GenMove>::attackLine_Bishop(const DistDB &db, const int From, const int To) const
{
	int offset = db.step * ((To > From)? 1:-1);
	for (int to = From + offset, k = 1; !(to & 0x88); to += offset, k++) {
		if (square[to] == EMPTY)
			continue;
		else if (OWN_PIECE(square[From], square[to]))
			return false;
		else if (ABS(square[to]) == BISHOP || ABS(square[to]) == QUEEN)
			return true;
		else if (k == 1 && (ABS(square[to]) == PAWN || ABS(square[to]) == KING))
			return true;
		break;
	}
	return false;
}

template<>
bool MoveLookup<GenMove>::isAttacked(const int From) const
{
	int8 *offset = offsets[BISHOP];
	for (; *offset; offset++) {
		for (int to = From + *offset, k = 1; !(to & 0x88); to += *offset, k++) {
			if (square[to] == EMPTY)
				continue;
			else if (OWN_PIECE(square[From], square[to]))
				break;
			else if (ABS(square[to]) == BISHOP || ABS(square[to]) == QUEEN)
				return true;
			else if (k == 1 && (ABS(square[to]) == PAWN || ABS(square[to]) == KING))
				return true;
			break;
		}
	}
	return isAttacked_xBishop(From, square[From]);
}
