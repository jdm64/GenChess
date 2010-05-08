/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Data declarations
 */

#ifndef __DATA_H__
#define __DATA_H__

#include <boost/array.hpp>

#define NONE		-1
#define PLACEABLE	-2
#define DEAD		-4

#define NEW_GAME	-4
#define QUIT_GAME	-2
#define BLACK		-1
#define WHITE		1

enum {BLACK_KING = -6, BLACK_QUEEN, BLACK_ROOK, BLACK_BISHOP, BLACK_KNIGHT,
	BLACK_PAWN = -1, EMPTY = 0, WHITE_PAWN = 1, WHITE_KNIGHT, WHITE_BISHOP,
	WHITE_ROOK, WHITE_QUEEN, WHITE_KING = 6};

enum {PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING, ALLSTAR};

enum {IS_COMMAND = -3, NOT_COMMAND = -2, NOT_MOVE = -1, VALID_MOVE = 0,
	INVALID_FORMAT, NOPIECE_ERROR, DONT_OWN, KING_FIRST, NON_EMPTY_PLACE,
	CAPTURE_OWN, INVALID_MOVEMENT, IN_CHECK, IN_CHECK_PLACE, NUM_ERRORS};

enum {MOVE, UNDO};

using namespace std;

struct MoveScores;
struct MovesPly;

struct Move {
	char to;
	char from;
	char index;
	char xindex;

	Move()
	{
		index = NONE;
		xindex = NONE;
		from = PLACEABLE;
		to = PLACEABLE;
	}
	bool operator==(const Move &rhs)
	{
		if (to == rhs.to && from == rhs.from && index == rhs.index &&
				xindex == rhs.xindex)
			return true;
		return false;
	}
};

struct Piece {
	const char type;
	char loc;
};

struct Action {
	int type;
	Move move;
};

struct MoveScores {
	Move move;
	int score;
	MovesPly *next;
	bool check;

	MoveScores()
	{
		next = NULL;
		check = false;
	}
};

struct MovesPly {
	int size;
	boost::array<MoveScores, 320> list;

	MovesPly()
	{
		size = 0;
	}
};

struct ScoresCapture {
	bool operator()(const MoveScores &a, const MoveScores &b)
	{
		if (a.check && !b.check)
			return true;
		if (!a.check && b.check)
			return false;
		if (a.move.xindex > b.move.xindex)
			return true;
		if (a.move.xindex < b.move.xindex)
			return false;
		if (a.move.index > b.move.index)
			return true;
		return false;
	}
};

struct ScoresHiLow {
	bool operator()(const MoveScores &a, const MoveScores &b)
	{
		return a.score > b.score;
	}
};

#endif