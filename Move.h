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

#include <iostream>
#include <boost/array.hpp>
#include "Defines.h"

using namespace std;

extern const int8 InitPieceType[32];
extern const char pieceSymbol[7];

string printLoc(const int8 loc);

template<char Type>
struct Move
{
	int8 index;
	int8 to;
	int8 from;
	int8 xindex;

	bool operator==(const Move<Type> &rhs) const;

	void setNull();

	bool isNull() const;

	MoveType type() const;

	string toString() const;

	bool parse(const string &s);

	string dump() const;
};

template<>
struct Move<'R'>
{
	int8 index;
	int8 to;
	int8 from;
	int8 xindex;
	int8 flags;

	Move()
	{
		flags = 0;
	}

	bool operator==(const Move<'R'> &rhs) const;

	void setNull();

	bool isNull() const;

	int type() const;

	string toString() const;

	bool parse(const string &s);

	string dump() const;

	int8 getCastle() const;

	void setCastle(int8 side);

	void setEnPassant();

	bool getEnPassant() const;

	void setPromote(int8 type);

	int8 getPromote() const;
};

typedef Move<'G'> GenMove;
typedef Move<'R'> RegMove;


template<class MoveType>
struct MoveNode
{
	MoveType move;
	int score;
	bool check;

	MoveNode() : score(0), check(false) {}
};

typedef MoveNode<GenMove> GenMoveNode;
typedef MoveNode<RegMove> RegMoveNode;


template<class MoveType>
struct MoveList
{
	boost::array<MoveNode<MoveType>, 320> list;
	int size;

	MoveList() : size(0) {}

	void print() const
	{
		for (int i = 0; i < size; i++)
			cout << list[i].move.toString() << "[" << list[i].score << "] ";
		cout << "\n";
	}
};

typedef MoveList<GenMove> GenMoveList;
typedef MoveList<RegMove> RegMoveList;


template<class MoveType>
struct ScoreSort
{
	bool operator()(const MoveNode<MoveType> &a, const MoveNode<MoveType> &b) const
	{
		return a.score > b.score;
	}
};

typedef ScoreSort<GenMove> GenScoreSort;
typedef ScoreSort<RegMove> RegScoreSort;

#endif
