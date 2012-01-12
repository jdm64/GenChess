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

template<class MoveType>
class Position : public MoveLookup<MoveType>
{
protected:
	void parseReset();

	bool setPiece(const int8 loc, const int8 type);

public:
	bool incheck(const int8 color) const;

	int parseFen_BoardStm(const string &st);

	bool parseFen(const string &st);

	int parseZfen_Board(const string &st);

	bool parseZfen(const string &st);

	void printFen_BoardStm(ostringstream &buf, string &fen) const;

	string printFen() const;

	void printZfen_Board(ostringstream &buf, string &fen) const;

	string printZfen() const;
};

typedef Position<GenMove> GenPosition;
typedef Position<RegMove> RegPosition;

#endif
