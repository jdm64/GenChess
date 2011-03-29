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

#ifndef __MOVE_H__
#define __MOVE_H__

#include <boost/array.hpp>
#include "Defines.h"

using namespace std;

extern const int8 pieceType[32];
extern const char pieceSymbol[7];

struct Move {
	int8 index;
	int8 to;
	int8 from;
	int8 xindex;

	bool operator==(const Move &rhs) const;

	void setNull();

	bool isNull() const;

	int type() const;

	string toString() const;

	bool parse(const string s);

	string dump() const;
};

// forward declations
struct MoveNode;
struct MoveList;

struct MoveNode {
	Move move;
	int score;
	bool check;

	MoveNode() : score(0), check(false) {}
};

struct MoveList {
	boost::array<MoveNode, 320> list;
	int size;

	MoveList() : size(0) {}

	void print() const;
};

struct ScoreSort {
	bool operator()(const MoveNode &a, const MoveNode &b) const
	{
		return a.score > b.score;
	}
};

string printLoc(const int8 loc);

#endif
