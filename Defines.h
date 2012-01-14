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

enum { EP_FILE = 0x7,
	CAN_EP = 0x8,
	CASTLE_KS = 0x10,
	CASTLE_QS = 0x20,
	WK_CASTLE = 0x10,
	WQ_CASTLE = 0x20,
	BK_CASTLE = 0x40,
	BQ_CASTLE = 0x80,
	W_CASTLE = (WK_CASTLE | WQ_CASTLE),
	B_CASTLE = (BK_CASTLE | BQ_CASTLE),
	K_CASTLE = (WK_CASTLE | BK_CASTLE),
	Q_CASTLE = (WQ_CASTLE | BQ_CASTLE) };

enum {	A1, B1, C1, D1, E1, F1, G1, H1, I1, J1, K1, L1, M1, N1, P1, Q1,
	A2, B2, C2, D2, E2, F2, G2, H2, I2, J2, K2, L2, M2, N2, P2, Q2,
	A3, B3, C3, D3, E3, F3, G3, H3, I3, J3, K3, L3, M3, N3, P3, Q3,
	A4, B4, C4, D4, E4, F4, G4, H4, I4, J4, K4, L4, M4, N4, P4, Q4,
	A5, B5, C5, D5, E5, F5, G5, H5, I5, J5, K5, L5, M5, N5, P5, Q5,
	A6, B6, C6, D6, E6, F6, G6, H6, I6, J6, K6, L6, M6, N6, P6, Q6,
	A7, B7, C7, D7, E7, F7, G7, H7, I7, J7, K7, L7, M7, N7, P7, Q7,
	A8, B8, C8, D8, E8, F8, G8, H8, I8, J8, K8, L8, M8, N8, P8, Q8};

struct sixInt {
	uint64 one;
	uint64 two;
	uint64 three;
	uint64 four;
	uint64 five;
	uint64 six;
};

#endif
