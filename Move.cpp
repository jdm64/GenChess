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

#include <iostream>
#include "Move.h"

const int8 pieceType[32] = {
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

string printLoc(const int8 loc)
{
	string s;

	if (loc > PLACEABLE) {
		s += ('a' + (loc % 8));
		s += ('8' - (loc / 8));
		return s;
	} else if (loc == PLACEABLE) {
		return "aval";
	} else {
		return "dead";
	}
}

bool GenMove::operator==(const GenMove &rhs) const
{
	if (to == rhs.to && from == rhs.from && index == rhs.index && xindex == rhs.xindex)
		return true;
	return false;
}

void GenMove::setNull()
{
	index = NULL_MOVE;
	xindex = NULL_MOVE;
	from = NULL_MOVE;
	to = NULL_MOVE;
}

bool GenMove::isNull() const
{
	if (index == NULL_MOVE && xindex == NULL_MOVE && from == NULL_MOVE && to == NULL_MOVE)
		return true;
	return false;
}

int GenMove::type() const
{
	if (from == PLACEABLE)
		return MOVE_PLACE;
	else if (xindex != NONE)
		return MOVE_CAPTURE;
	else
		return MOVE_MOVE;
}

string GenMove::toString() const
{
	string out;

	if (from == PLACEABLE)
		out += pieceSymbol[ABS(pieceType[index])];
	else
		out = printLoc(from);
	out += printLoc(to);
	return out;
}

bool GenMove::parse(const string s)
{
	int8 piece;
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
		if (s[1] < 'a' || s[1] > 'h' || s[2] < '1' || s[2] > '8')
			return false;
		to = s[1] - 'a';
		to += 8 * (8 - (s[2] - '0'));
		from = PLACEABLE;
		index = piece;
	} else {
		// parse movement move
		if (s[0] < 'a' || s[0] > 'h' || s[1] < '1' || s[1] > '8' ||
				s[2] < 'a' || s[2] > 'h' || s[3] < '1' || s[3] > '8')
			return false;
		from = s[0] - 'a';
		from += 8 * (8 - (s[1] - '0'));
		to = s[2] - 'a';
		to += 8 * (8 - (s[3] - '0'));
	}
	return true;
}

string GenMove::dump() const
{
	char data[24];

	sprintf(data, "[%d %d %d %d]", (int)index, (int)xindex, (int)from, (int)to);

	return string(data);
}

void GenMoveList::print() const
{
	for (int i = 0; i < size; i++)
		cout << list[i].move.toString() << "[" << list[i].score << "] ";
	cout << "\n";
}
