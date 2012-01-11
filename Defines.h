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

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <cstdint>

#define VERSION		"2.0.75"
#define ABS(X)		((X >= 0)? X : -(X))
#define COL(X)		((X) & 7)

typedef uint64_t uint64;
typedef int64_t int64;
typedef int8_t int8;
typedef uint8_t uint8;

enum {NONE = -1, PLACEABLE = -2, DEAD = -4, NULL_MOVE = -8};

enum {BLACK = -1, WHITE = 1};

enum {BLACK_KING = -6, BLACK_QUEEN, BLACK_ROOK, BLACK_BISHOP, BLACK_KNIGHT,
	BLACK_PAWN = -1, EMPTY = 0, WHITE_PAWN = 1, WHITE_KNIGHT, WHITE_BISHOP,
	WHITE_ROOK, WHITE_QUEEN, WHITE_KING = 6};

enum {PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING};

enum {IS_COMMAND = -3, NOT_COMMAND = -2, NOT_MOVE = -1, VALID_MOVE = 0,
	INVALID_FORMAT, NOPIECE_ERROR, DONT_OWN, KING_FIRST, NON_EMPTY_PLACE,
	CAPTURE_OWN, INVALID_MOVEMENT, IN_CHECK, IN_CHECK_PLACE, CANT_CASTLE, NUM_MOVE_ERRORS};

enum class MoveClass {ALL, CAPTURE, MOVE, PLACE};

enum {NOTMATE, WHITE_CHECKMATE, BLACK_CHECKMATE, STALEMATE,
	IMPOSSIBLE, WHITE_RESIGN, BLACK_RESIGN};

enum {HUMAN, COMPUTER};

enum {	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1};

struct sixInt {
	uint64 one;
	uint64 two;
	uint64 three;
	uint64 four;
	uint64 five;
	uint64 six;
};

#endif
