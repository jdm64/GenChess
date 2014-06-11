/* GenChess, a genesis chess engine
 * Copyright (C) 2010-2014, Justin Madru (justin.jdm64@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __POSITION_H__
#define __POSITION_H__

#include "Move.h"
#include "Piece.h"
#include "MoveLookup.h"

#define EE64(x) (((x) >> 1) | ((x) & 7))
#define EE64F(x) (((x) & 7) + ((7 - ((x) >> 4)) << 3))
#define SF88(x) (((x) & ~7) + (x))
#define SFF88(x) (((x) & 7) + ((7 - ((x) >> 3)) << 4))

extern const int8 InitRegPiece[32];

template<class MoveType>
class Position : public MoveLookup<MoveType>
{
protected:
	void parseReset();

	bool setPiece(const int loc, const int type);

	void setMaxPly();

public:
	bool incheck(const int color) const;

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
