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

struct GenMove {
	int8 index;
	int8 to;
	int8 from;
	int8 xindex;

	bool operator==(const GenMove &rhs) const;

	void setNull();

	bool isNull() const;

	int type() const;

	string toString() const;

	bool parse(const string &s);

	string dump() const;
};

// forward declations
struct GenMoveNode;
struct GenMoveList;

struct GenMoveNode {
	GenMove move;
	int score;
	bool check;

	GenMoveNode() : score(0), check(false) {}
};

struct GenMoveList {
	boost::array<GenMoveNode, 320> list;
	int size;

	GenMoveList() : size(0) {}

	void print() const;
};

struct GenScoreSort {
	bool operator()(const GenMoveNode &a, const GenMoveNode &b) const
	{
		return a.score > b.score;
	}
};

string printLoc(const int8 loc);

// ---Start Regular Chess ---

#define IS_PROMOTE(TO, COLOR)	((COLOR == WHITE)? (TO < A7) : (TO > H2))

struct RegMove {
	int8 index;
	int8 to;
	int8 from;
	int8 xindex;
	int8 flags; // castle, promote. en passant

	RegMove()
	{
		flags = 0;
	}

	bool operator==(const RegMove &rhs) const;

	int8 getCastle() const;

	void setCastle(int8 side);

	void setEnPassant();

	bool getEnPassant() const;

	void setPromote(int8 type);

	int8 getPromote() const;

	void setNull();

	bool isNull() const;

	int type() const;

	string toString() const;

	bool parse(const string &s);

	string dump() const;
};

// forward declations
struct RegMoveNode;
struct RegMoveList;

struct RegMoveNode {
	RegMove move;
	int score;
	bool check;

	RegMoveNode() : score(0), check(false) {}
};

struct RegMoveList {
	boost::array<RegMoveNode, 320> list;
	int size;

	RegMoveList() : size(0) {}

	void print() const;
};

struct RegScoreSort {
	bool operator()(const RegMoveNode &a, const RegMoveNode &b) const
	{
		return a.score > b.score;
	}
};

#endif
