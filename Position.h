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

#ifndef __POSITION_H__
#define __POSITION_H__

#include "Move.h"
#include "Piece.h"
#include "MoveLookup.h"

class GenPosition
{
public:
	int8 square[64];
	int8 piece[32];
	int ply;

	void reset();

	bool setPiece(const int8 loc, const int8 type);

	bool incheck(const int8 color) const;

	bool parseFen(const string &st);

	bool parseZfen(const string &st);

	string printFen() const;

	string printZfen() const;
};

class RegPosition
{
public:
	int8 square[64];
	Piece piece[32];
	int ply;
	MoveFlags flags;

	void reset();

	bool setPiece(const int8 loc, const int8 type);

	bool incheck(const int8 color) const;

	bool parseFen(const string &st);

	bool parseZfen(const string &st);

	string printFen() const;

	string printZfen() const;
};

#endif