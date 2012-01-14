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

#include "Move.h"

const int8 InitPieceType[32] = {
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

string printLoc(const int loc)
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

template<char Type>
bool Move<Type>::operator==(const Move<Type> &rhs) const
{
	if (to == rhs.to && from == rhs.from && index == rhs.index && xindex == rhs.xindex)
		return true;
	return false;
}

template<>
void Move<'G'>::setNull()
{
	index = NULL_MOVE;
	xindex = NULL_MOVE;
	from = NULL_MOVE;
	to = NULL_MOVE;
}

void Move<'R'>::setNull()
{
	index = NULL_MOVE;
	xindex = NULL_MOVE;
	from = NULL_MOVE;
	to = NULL_MOVE;
}

template<>
bool Move<'G'>::isNull() const
{
	if (index == NULL_MOVE && xindex == NULL_MOVE && from == NULL_MOVE && to == NULL_MOVE)
		return true;
	return false;
}

bool Move<'R'>::isNull() const
{
	if (index == NULL_MOVE && xindex == NULL_MOVE && from == NULL_MOVE && to == NULL_MOVE)
		return true;
	return false;
}

template<char Type>
MoveClass Move<Type>::moveclass() const
{
	if (from == PLACEABLE)
		return MoveClass::PLACE;
	else if (xindex != NONE)
		return MoveClass::CAPTURE;
	else
		return MoveClass::MOVE;
}

template<>
string Move<'G'>::toString() const
{
	string out;

	if (from == PLACEABLE)
		out += pieceSymbol[ABS(InitPieceType[index])];
	else
		out = printLoc(from);
	out += printLoc(to);
	return out;
}

string Move<'R'>::toString() const
{
	if (getCastle() & K_CASTLE)
		return "O-O";
	else if (getCastle() & Q_CASTLE)
		return "O-O-O";

	string out;

	out = printLoc(from);
	out += printLoc(to);

	switch (getPromote()) {
	case 2:
		out += 'N';
		break;
	case 3:
		out += 'B';
		break;
	case 4:
		out += 'R';
		break;
	case 5:
		out += 'Q';
		break;
	}
	return out;
}

template<>
bool Move<'G'>::parse(const string &s)
{
	int piece;
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

bool Move<'R'>::parse(const string &s)
{
	switch (s[0]) {
	case 'O':
	case 'o':
	case '0':
		if (s[1] != '-')
			return false;

		if (s[2] != 'O' && s[2] != 'o' && s[2] != '0')
			return false;
		if (s.length() == 3) {
			setCastle(CASTLE_KS);
			return true;
		}
		if (s[3] != '-')
			return false;
		if (s[4] != 'O' && s[4] != 'o' && s[4] != '0')
			return false;
		setCastle(CASTLE_QS);
		return true;
	}

	if (s[0] < 'a' || s[0] > 'h' || s[1] < '1' || s[1] > '8' ||
			s[2] < 'a' || s[2] > 'h' || s[3] < '1' || s[3] > '8')
		return false;

	from = s[0] - 'a';
	from += 8 * (8 - (s[1] - '0'));
	to = s[2] - 'a';
	to += 8 * (8 - (s[3] - '0'));

	if (s.length() == 5) {
		switch (s[4]) {
		case 'Q':
			setPromote(QUEEN);
			break;
		case 'R':
			setPromote(ROOK);
			break;
		case 'B':
			setPromote(BISHOP);
			break;
		case 'N':
			setPromote(KNIGHT);
			break;
		default:
			return false;
		}
	}
	return true;
}

template<>
string Move<'G'>::dump() const
{
	char data[24];

	sprintf(data, "[%d %d %d %d]", (int)index, (int)xindex, (int)from, (int)to);

	return string(data);
}

string Move<'R'>::dump() const
{
	char data[24];

	sprintf(data, "[%d %d %d %d %d]", (int)index, (int)xindex, (int)from, (int)to, (int)flags);

	return string(data);
}

int Move<'R'>::getCastle() const
{
	return flags & (CASTLE_KS | CASTLE_QS);
}

void Move<'R'>::setCastle(int side) // 0x10 =kingside, 0x20=queensize
{
	flags = side & (CASTLE_KS | CASTLE_QS);
}

void Move<'R'>::setEnPassant()
{
	flags = CAN_EP;
}

bool Move<'R'>::getEnPassant() const
{
	return flags & CAN_EP;
}

void Move<'R'>::setPromote(int type)
{
	flags = type & 0x7;
}

int Move<'R'>::getPromote() const
{
	return flags & 0x7;
}
