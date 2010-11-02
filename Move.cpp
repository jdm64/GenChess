#include "Move.h"
#include "Defines.h"

const char pieceType[32] = {
	BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,
	BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,
	BLACK_KNIGHT, BLACK_KNIGHT, BLACK_BISHOP, BLACK_BISHOP,
	BLACK_ROOK,   BLACK_ROOK,   BLACK_QUEEN,  BLACK_KING,
	WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,
	WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,
	WHITE_KNIGHT, WHITE_KNIGHT, WHITE_BISHOP, WHITE_BISHOP,
	WHITE_ROOK,   WHITE_ROOK,   WHITE_QUEEN,  WHITE_KING
};

const char pieceSymbol[7] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};

bool Move::operator==(const Move &rhs) const
{
	if (to == rhs.to && from == rhs.from && index == rhs.index && xindex == rhs.xindex)
		return true;
	return false;
}

string Move::toString() const
{
	string out;

	if (from == PLACEABLE)
		out += pieceSymbol[abs(pieceType[index])];
	else
		out = printLoc(from);
	out += printLoc(to);
	return out;
}

bool Move::parse(string s)
{
	char piece;
	bool place = true;

	switch (s[0]) {
	case 'a':	case 'b':
	case 'c':	case 'd':
	case 'e':	case 'f':
	case 'g':	case 'h':
		place = false;
		break;
	case 'P':
		piece = PAWN;
		break;
	case 'N':
		piece = KNIGHT;
		break;
	case 'B':
		piece = BISHOP;
		break;
	case 'R':
		piece = ROOK;
		break;
	case 'Q':
		piece = QUEEN;
		break;
	case 'K':
		piece = KING;
		break;
	default:
		return false;
	}
	if (place) {
		// parse placement move
		if (s[1] < 'a' || s[1] > 'h' || s[2] < '0' || s[2] > '9')
			return false;
		to = s[1] - 'a';
		to += 8 * (8 - (s[2] - '0'));
		from = PLACEABLE;
		index = piece;
	} else {
		// parse movement move
		if (s[0] < 'a' || s[0] > 'h' || s[1] < '0' || s[1] > '9' ||
				s[2] < 'a' || s[2] > 'h' || s[3] < '0' || s[3] > '9')
			return false;
		from = s[0] - 'a';
		from += 8 * (8 - (s[1] - '0'));
		to = s[2] - 'a';
		to += 8 * (8 - (s[3] - '0'));
	}
	return true;
}

string printLoc(const char loc)
{
	string s;

	if (loc > PLACEABLE) {
		s += (char)('a' + (loc % 8));
		s += (char)('8' - (loc / 8));
		return s;
	} else if (loc == PLACEABLE) {
		return "aval";
	} else {
		return "dead";
	}
}
