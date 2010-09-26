/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Data declarations
 */

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <cstdint>

typedef uint64_t uint64;

#define VERSION		"2.0.75"

#define NONE		-1
#define PLACEABLE	-2
#define DEAD		-4

#define QUIT_GAME	-4
#define NEW_GAME	-2
#define BLACK		-1
#define WHITE		1

enum {BLACK_KING = -6, BLACK_QUEEN, BLACK_ROOK, BLACK_BISHOP, BLACK_KNIGHT,
	BLACK_PAWN = -1, EMPTY = 0, WHITE_PAWN = 1, WHITE_KNIGHT, WHITE_BISHOP,
	WHITE_ROOK, WHITE_QUEEN, WHITE_KING = 6};

enum {PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING};

enum {IS_COMMAND = -3, NOT_COMMAND = -2, NOT_MOVE = -1, VALID_MOVE = 0,
	INVALID_FORMAT, NOPIECE_ERROR, DONT_OWN, KING_FIRST, NON_EMPTY_PLACE,
	CAPTURE_OWN, INVALID_MOVEMENT, IN_CHECK, IN_CHECK_PLACE, NUM_MOVE_ERRORS};

enum {NOT_MATE, CHECK_MATE, STALE_MATE};

enum {HUMAN, COMPUTER};

struct intPair {
	int one;
	int two;
};

#endif
