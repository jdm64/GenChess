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
#include "MoveLookup.h"

const int8 mailbox[120] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
	-1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
	-1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
	-1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
	-1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
	-1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
	-1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
	-1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

const int8 mailbox64[64] = {
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98};

const int8 offsets[7][8] = {
	{  0,   0,   0,  0,   0,  0,  0,  0},
	{-11,  -9,   9, 11, -10, -1,  1, 10},
	{-21, -19, -12, -8,   8, 12, 19, 21},
	{-11,  -9,   9, 11,   0,  0,  0,  0},
	{-10,  -1,   1, 10,   0,  0,  0,  0},
	{-11, -10,  -9, -1,   1,  9, 10, 11},
	{-11, -10,  -9, -1,   1,  9, 10, 11} };

int8* GenMoveLookup::genAll(const int8 From) const
{
	const int type = ABS(square[From]), mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	int next = 0;
	int8* const list = new int8[28];

	switch (type) {
	case PAWN:
		// captures
		for (int dir = 0; dir < 4; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		// moves
		for (int dir = 4; dir < 8; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (square[to] == EMPTY)
				list[next++] = to;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (ANY_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to])) {
					list[next++] = to;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to])) {
					list[next++] = to;
				}
				break;
			}
		}
		break;
	}
	list[next] = -1;
	return list;
}

int8* GenMoveLookup::genCapture(const int8 From) const
{
	const int type = ABS(square[From]), mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	int next = 0;
	int8* const list = new int8[28];

	switch (type) {
	case PAWN:
		// captures
		for (int dir = 0; dir < 4; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1)
					break;
				else if (square[to] == EMPTY)
					continue;
				else if (CAPTURE_MOVE(square[From], square[to]))
					list[next++] = to;
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1)
					break;
				else if (square[to] == EMPTY)
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

int8* GenMoveLookup::genMove(const int8 From) const
{
	const int type = ABS(square[From]), mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	int next = 0;
	int8* const list = new int8[28];

	switch (type) {
	case PAWN:
		// moves
		for (int dir = 4; dir < 8; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (square[to] == EMPTY)
				list[next++] = to;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (square[to] == EMPTY)
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				}
				break;
			}
		}
		break;
	}
	list[next] = -1;
	return list;
}

bool GenMoveLookup::fromto(const int8 From, const int8 To) const
{
	const int type = ABS(square[From]), mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	switch (type) {
	case PAWN:
		// captures
		for (int dir = 0; dir < 4; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(square[From], square[to]) && to == To)
				return true;
		}
		// moves
		for (int dir = 4; dir < 8; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (square[to] == EMPTY && to == To)
				return true;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (ANY_MOVE(square[From], square[to]) && to == To)
				return true;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					if (to == To)
						return true;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to]) && to == To) {
					return true;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int to, k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					if (to == To)
						return true;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to]) && to == To) {
					return true;
				}
				break;
			}
		}
		break;
	}
	return false;
}

bool GenMoveLookup::isAttacked(const int8 From) const
{
	const int mfrom = mailbox64[From];

	// ROOK
	for (int dir = 0; dir < 4; dir++) {
		for (int k = 1; k < 8; k++) {
			const int to = mailbox[mfrom + k * offsets[ROOK][dir]];
			if (to == -1)
				break;
			else if (square[to] == EMPTY)
				continue;
			else if (OWN_PIECE(square[From], square[to]))
				break;
			else if (ABS(square[to]) == ROOK || ABS(square[to]) == QUEEN)
				return true;
			else if (k == 1 && ABS(square[to]) == KING)
				return true;
			break;
		}
	}
	// BISHOP
	for (int dir = 0; dir < 4; dir++) {
		for (int k = 1; k < 8; k++) {
			const int to = mailbox[mfrom + k * offsets[BISHOP][dir]];
			if (to == -1)
				break;
			else if (square[to] == EMPTY)
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
	// KNIGHT
	for (int dir = 0; dir < 8; dir++) {
		const int to = mailbox[mfrom + offsets[KNIGHT][dir]];
		if (to == -1)
			continue;
		else if (NOT_CAPTURE(square[From], square[to]))
			continue;
		else if (ABS(square[to]) == KNIGHT)
			return true;
	}
	return false;
}

// --- Start Regular Chess ---

int8* RegMoveLookup::genAll(const int8 From) const
{
	const int type = ABS(square[From]), mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	int next = 0;
	int8* const list = new int8[28];

	switch (type) {
	case PAWN:
		if (square[From] > 0) { // WHITE
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From - 9]))
				list[next++] = From - 9;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From - 7]))
				list[next++] = From - 7;
			if (!square[From - 8]) {
				list[next++] = From - 8;
				if (From >= 48 && !square[From - 16])
					list[next++] = From - 16;
			}
		} else { // BLACK
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From + 7]))
				list[next++] = From + 7;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From + 9]))
				list[next++] = From + 9;
			if (!square[From + 8]) {
				list[next++] = From + 8;
				if (From <= 15 && !square[From + 16])
					list[next++] = From + 16;
			}
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int8 to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (ANY_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to])) {
					list[next++] = to;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to])) {
					list[next++] = to;
				}
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	list[next] = -1;
	return list;
}

int8* RegMoveLookup::genCapture(const int8 From) const
{
	const int type = ABS(square[From]),  mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	int next = 0;
	int8* const list = new int8[28];

	switch (type) {
	case PAWN:
		if (square[From] > 0) { // WHITE
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From - 9]))
				list[next++] = From - 9;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From - 7]))
				list[next++] = From - 7;
		} else { // BLACK
			if (COL(From) != 0 && CAPTURE_MOVE(square[From], square[From + 7]))
				list[next++] = From + 7;
			if (COL(From) != 7 && CAPTURE_MOVE(square[From], square[From + 9]))
				list[next++] = From + 9;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int8 to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(square[From], square[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1)
					break;
				else if (square[to] == EMPTY)
					continue;
				else if (CAPTURE_MOVE(square[From], square[to]))
					list[next++] = to;
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1)
					break;
				else if (square[to] == EMPTY)
					continue;
				else if (CAPTURE_MOVE(square[From], square[to]))
					list[next++] = to;
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	list[next] = -1;
	return list;
}

int8* RegMoveLookup::genMove(const int8 From) const
{
	const int type = ABS(square[From]), mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	int next = 0;
	int8* const list = new int8[28];

	switch (type) {
	case PAWN:
		if (square[From] > 0) { // WHITE
			if (!square[From - 8]) {
				list[next++] = From - 8;
				if (From >= 48 && !square[From - 16])
					list[next++] = From - 16;
			}
		} else { // BLACK
			if (!square[From + 8]) {
				list[next++] = From + 8;
				if (From <= 15 && !square[From + 16])
					list[next++] = From + 16;
			}
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int8 to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (square[to] == EMPTY)
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					list[next++] = to;
					continue;
				}
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	list[next] = -1;
	return list;
}

bool RegMoveLookup::fromto(const int8 From, const int8 To) const
{
	const int type = ABS(square[From]), mfrom = mailbox64[From];
	const int8* const offset = offsets[type];

	switch (type) {
	case PAWN:
		if (square[From] > 0) { // WHITE
			if (From - 9 == To && COL(From) != 0 && CAPTURE_MOVE(square[From], square[From - 9]))
				return true;
			if (From - 7 == To && COL(From) != 7 && CAPTURE_MOVE(square[From], square[From - 7]))
				return true;
			if (!square[From - 8]) {
				if (From - 8 == To)
					return true;
				if (From >= 48 && !square[From - 16] && From - 16 == To)
					return true;
			}
		} else { // BLACK
			if (From + 7 == To && COL(From) != 0 && CAPTURE_MOVE(square[From], square[From + 7]))
				return true;
			if (From + 9 == To && COL(From) != 7 && CAPTURE_MOVE(square[From], square[From + 9]))
				return true;
			if (!square[From + 8]) {
				if (From + 8 == To)
					return true;
				if (From <= 15 && !square[From + 16] && From + 16 == To)
					return true;
			}
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			const int8 to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (ANY_MOVE(square[From], square[to]) && to == To)
				return true;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					if (to == To)
						return true;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to])) {
					if (to == To)
						return true;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				const int8 to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (square[to] == EMPTY) {
					if (to == To)
						return true;
					continue;
				} else if (CAPTURE_MOVE(square[From], square[to])) {
					if (to == To)
						return true;
				}
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	return false;
}

bool RegMoveLookup::isAttacked(const int8 From, const int8 Bycolor) const
{
	const int mfrom = mailbox64[From];

	for (int dir = 0; dir < 4; dir++) {
		for (int k = 1; k < 8; k++) {
			const int8 to = mailbox[mfrom + k * offsets[ROOK][dir]];
			if (to == -1)
				break;
			else if (square[to] == EMPTY)
				continue;
			else if (OWN_PIECE(square[to], Bycolor))
				break;
			else if (ABS(square[to]) == ROOK || ABS(square[to]) == QUEEN)
				return true;
			else if (k == 1 && ABS(square[to]) == KING)
				return true;
			break;
		}
	}

	for (int dir = 0; dir < 4; dir++) {
		for (int k = 1; k < 8; k++) {
			const int8 to = mailbox[mfrom + k * offsets[BISHOP][dir]];
			if (to == -1) {
				break;
			} else if (square[to] == EMPTY) {
				continue;
			} else if (OWN_PIECE(square[to], Bycolor)) {
				break;
			} else if (ABS(square[to]) == BISHOP || ABS(square[to]) == QUEEN) {
				return true;
			} else if (k == 1) {
				if (ABS(square[to]) == PAWN && Bycolor * (From - to) > 0)
					return true;
				else if (ABS(square[to]) == KING)
					return true;
			}
			break;
		}
	}

	for (int dir = 0; dir < 8; dir++) {
		const int8 to = mailbox[mfrom + offsets[KNIGHT][dir]];
		if (to == -1)
			continue;
		else if (NOT_CAPTURE(square[to], Bycolor))
			continue;
		else if (ABS(square[to]) == KNIGHT)
			return true;
	}
	return false;
}
