#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <cstdint>

typedef uint64_t uint64;

#define VERSION		"2.0.75"

enum {NONE = -1, PLACEABLE = -2, DEAD = -4};

enum {BLACK = -1, WHITE = 1};

enum {BLACK_KING = -6, BLACK_QUEEN, BLACK_ROOK, BLACK_BISHOP, BLACK_KNIGHT,
	BLACK_PAWN = -1, EMPTY = 0, WHITE_PAWN = 1, WHITE_KNIGHT, WHITE_BISHOP,
	WHITE_ROOK, WHITE_QUEEN, WHITE_KING = 6};

enum {PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING};

enum {IS_COMMAND = -3, NOT_COMMAND = -2, NOT_MOVE = -1, VALID_MOVE = 0,
	INVALID_FORMAT, NOPIECE_ERROR, DONT_OWN, KING_FIRST, NON_EMPTY_PLACE,
	CAPTURE_OWN, INVALID_MOVEMENT, IN_CHECK, IN_CHECK_PLACE, NUM_MOVE_ERRORS};

enum {MOVE_ALL, MOVE_CAPTURE, MOVE_MOVE, MOVE_PLACE};

enum {NOT_MATE, CHECK_MATE, STALE_MATE};

enum {HUMAN, COMPUTER};

struct intPair {
	uint64 one;
	uint64 two;
};

#endif
