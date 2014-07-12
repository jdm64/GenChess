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

#ifndef __MOVE_H__
#define __MOVE_H__

#include <iostream>
#include <array>
#include <algorithm>
#include <deque>
#include "Defines.h"

using namespace std;

extern const int8 InitPieceType[32];
extern const char pieceSymbol[7];

string printLoc(const int loc);

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

	MoveClass moveclass() const;

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

	MoveClass moveclass() const;

	string toString() const;

	bool parse(const string &s);

	string dump() const;

	int getCastle() const;

	void setCastle(int side);

	void setEnPassant();

	bool getEnPassant() const;

	void setPromote(int type);

	int getPromote() const;
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
bool scoreSort(const MoveNode<MoveType> &a, const MoveNode<MoveType> &b)
{
	return a.score > b.score;
}

// forward declaration
template<class MoveType>
class MoveListPool;

template<class MoveType>
class MoveList
{
	friend MoveListPool<MoveType>;
	typedef typename array<MoveNode<MoveType>, 320>::iterator MoveListIter;

	array<MoveNode<MoveType>, 320> list;
	int len;

	MoveList() : len(0) {}

	void clear()
	{
		len = 0;
	}
public:

	void add(const MoveNode<MoveType> &item)
	{
		list[len++] = item;
	}

	void sort()
	{
		stable_sort(begin(), end(), scoreSort<MoveType>);
	}

	bool empty() const
	{
		return !len;
	}

	int size() const
	{
		return len;
	}

	MoveNode<MoveType>& at(int i)
	{
		return list[i];
	}

	MoveListIter begin()
	{
		return list.begin();
	}

	MoveListIter end()
	{
		return list.begin() + len;
	}

	void print() const
	{
		for (int i = 0; i < len; i++)
			cout << list[i].move.toString() << "[" << list[i].score << "] ";
		cout << "\n";
	}
};

typedef MoveList<GenMove> GenMoveList;
typedef MoveList<RegMove> RegMoveList;

template<class MoveType>
class MoveListPool
{
	deque<MoveList<MoveType>*> pool;

public:
	MoveList<MoveType>* get()
	{
		if (pool.empty())
			return new MoveList<MoveType>;
		auto item = pool.back();
		pool.pop_back();
		return item;
	}

	void put(MoveList<MoveType>* item)
	{
		item->clear();
		pool.push_back(item);
	}
};

#endif
