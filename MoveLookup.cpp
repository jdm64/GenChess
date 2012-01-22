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

#include <cassert>
#include "Move.h"
#include "MoveLookup.h"

int8 offsets[7][10] = {
	{ 0,  0,   0,   0,   0,   0,   0,   0, 0, 0},
	{17, 16,  15,   1, -17, -16, -15,  -1, 0, 0}, // Pawn: even=capture
	{33, 31,  18,  14, -33, -31, -18, -14, 0, 0}, // Knight
	{17, 15, -17, -15,   0,   0,   0,   0, 0, 0}, // Bishop
	{16,  1, -16,  -1,   0,   0,   0,   0, 0, 0}, // Rook
	{17, 16,  15,   1, -17, -16, -15,  -1, 0, 0}, // Queen
	{17, 16,  15,   1, -17, -16, -15,  -1, 0, 0} }; // King

DistDB ddb[120] = {
	{ 0,  EMPTY}, {1,    ROOK}, { 1,   ROOK}, { 1,   ROOK}, { 1,   ROOK}, { 1,   ROOK},
	{ 1,   ROOK}, {1,    ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, {0,   EMPTY}, {14, KNIGHT}, {15, BISHOP}, {16,   ROOK}, {17, BISHOP},
	{18, KNIGHT}, {0,   EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, {0,   EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{15, BISHOP}, {31, KNIGHT}, {16,   ROOK}, {33, KNIGHT}, {17, BISHOP}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{16,   ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, {17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {16,   ROOK}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, {17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, {16,   ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, {17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{16,   ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, {15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {16,   ROOK}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {17, BISHOP}
};

// --- Start General Template Code ---

template<class MoveType>
int MoveLookup<MoveType>::genAll_xPawn(int8* const list, int8* offset, const int From, const int type) const
{
	int next = 0;

	switch (type) {
	case BISHOP:
	case ROOK:
	case QUEEN:
		for (; *offset; offset++) {
			for (int to = From + *offset; !(to & 0x88); to += *offset) {
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
		for (; *offset; offset++) {
			const int to = From + *offset;
			if (to & 0x88)
				continue;
			else if (ANY_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		break;
	}
	return next;
}

template<class MoveType>
int MoveLookup<MoveType>::genCapture_xPawn(int8* const list, int8* offset, const int From, const int type) const
{
	int next = 0;

	switch (type) {
	case KNIGHT:
	case KING:
		for (; *offset; offset++) {
			const int to = From + *offset;
			if (to & 0x88)
				continue;
			else if (CAPTURE_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
	case QUEEN:
		for (; *offset; offset++) {
			for (int to = From + *offset; !(to & 0x88); to += *offset) {
				if (square[to] == EMPTY)
					continue;
				else if (CAPTURE_MOVE(square[From], square[to]))
					list[next++] = to;
				break;
			}
		}
		break;
	}
	return next;
}

template<class MoveType>
int MoveLookup<MoveType>::genMove_xPawn(int8* const list, int8* offset, const int From, const int type) const
{
	int next = 0;

	switch (type) {
	case KNIGHT:
	case KING:
		for (; *offset; offset++) {
			const int to = From + *offset;
			if (to & 0x88)
				continue;
			else if (square[to] == EMPTY)
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
	case QUEEN:
		for (; *offset; offset++) {
			for (int to = From + *offset; !(to & 0x88); to += *offset) {
				if (square[to] == EMPTY)
					list[next++] = to;
				else
					break;
			}
		}
		break;
	}
	return next;
}

template<class MoveType>
bool MoveLookup<MoveType>::fromto_xPawn(const int From, const int To, const int type, int8* const offset) const
{
	int diff = ABS(From - To), n = 2;

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

template<class MoveType>
bool MoveLookup<MoveType>::isAttacked_xBishop(const int From, const int FromColor) const
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

// --- Start Genesis Code ---

template<>
int8* MoveLookup<GenMove>::genAll(const int From) const
{
	const int type = ABS(square[From]);
	int8 *offset = offsets[type];
	int8 *list = new int8[28];
	int next = 0;

	if (type == PAWN) {
		for (bool evn = true; *offset; offset++, evn ^= true) {
			const int to = From + *offset;
			if (to & 0x88)
				continue;
			const bool val = evn? CAPTURE_MOVE(square[From], square[to]) : (square[to] == EMPTY);
			if (val)
				list[next++] = to;
		}
	} else {
		next = genAll_xPawn(list, offset, From, type);
	}
	list[next] = -1;
	return list;
}

template<>
int8* MoveLookup<GenMove>::genCapture(const int From) const
{
	const int type = ABS(square[From]);
	int8 *offset = offsets[type];
	int8 *list = new int8[28];
	int next = 0;

	if (type == PAWN) {
		// captures
		for (; *offset; offset += 2) {
			const int to = From + *offset;
			if (to & 0x88)
				continue;
			else if (CAPTURE_MOVE(square[From], square[to]))
				list[next++] = to;
		}
	} else {
		next = genCapture_xPawn(list, offset, From, type);
	}
	list[next] = -1;
	return list;
}

template<>
int8* MoveLookup<GenMove>::genMove(const int From) const
{
	const int type = ABS(square[From]);
	int8 *offset = offsets[type];
	int8 *list = new int8[28];
	int next = 0;

	if (type == PAWN) {
		// moves
		for (offset++; *offset; offset += 2) {
			const int to = From + *offset;
			if (to & 0x88)
				continue;
			else if (square[to] == EMPTY)
				list[next++] = to;
		}
	} else {
		next = genMove_xPawn(list, offset, From, type);
	}
	list[next] = -1;
	return list;
}

template<>
bool MoveLookup<GenMove>::fromto(const int From, const int To) const
{
	if ((From | To) & 0x88)
		return false;

	const int type = ABS(square[From]);
	int8 *offset = offsets[type];

	if (type == PAWN) {
		const int diff = ABS(From - To);
		for (int i = 0; i < 4; i++) {
			if (diff == offset[i])
				return ((i%2)? (square[To] == EMPTY) : CAPTURE_MOVE(square[From], square[To]));
		}
	} else {
		return fromto_xPawn(From, To, type, offset);
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
		else if (k == 1 && (ABS(square[to]) == PAWN || ABS(square[to]) == KING))
			return true;
		else if (ABS(square[to]) == BISHOP || ABS(square[to]) == QUEEN)
			return true;
		break;
	}
	return false;
}

template<>
bool MoveLookup<GenMove>::isAttacked(const int From) const
{
	// BISHOP
	int8 *offset = offsets[BISHOP];
	for (; *offset; offset++) {
		for (int to = From + *offset, k = 1; !(to & 0x88); to += *offset, k++) {
			if (square[to] == EMPTY)
				continue;
			else if (OWN_PIECE(square[From], square[to]))
				break;
			else if (k == 1 && (ABS(square[to]) == PAWN || ABS(square[to]) == KING))
				return true;
			else if (ABS(square[to]) == BISHOP || ABS(square[to]) == QUEEN)
				return true;
			else
				break;
		}
	}
	return isAttacked_xBishop(From, square[From]);
}

// --- Start Regular Chess ---

template<>
int8* MoveLookup<RegMove>::genAll(const int From) const
{
	const int type = ABS(square[From]);
	int8* const list = new int8[28];
	int next = 0;

	if (type == PAWN) {
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
	} else {
		next = genAll_xPawn(list, offsets[type], From, type);
	}
	list[next] = -1;
	return list;
}

template<>
int8* MoveLookup<RegMove>::genCapture(const int From) const
{
	const int type = ABS(square[From]); //  mfrom = mailbox64[From];
	int8* const list = new int8[28];
	int next = 0;

	if (type == PAWN) {
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
	} else {
		next = genCapture_xPawn(list, offsets[type], From, type);
	}
	list[next] = -1;
	return list;
}

template<>
int8* MoveLookup<RegMove>::genMove(const int From) const
{
	const int type = ABS(square[From]);
	int8* const list = new int8[28];
	int next = 0;

	if (type == PAWN) {
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
	} else {
		next = genMove_xPawn(list, offsets[type], From, type);
	}
	list[next] = -1;
	return list;
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
