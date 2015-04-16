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

#ifndef __TRANS_TABLE_H__
#define __TRANS_TABLE_H__

#include <cassert>
#include "Rand64.h"

//#define COLLISION_DEBUG

#define ZBOX_SIZE 838
#define WTM_HASH 837
#define HOLD_START 768
#define HOLD_END 781
#define ENPASSANT_HASH 834
#define CASTLE_HASH 834

enum {NONE_NODE = 0, ALL_NODE = 3, CUT_NODE = 6, PV_NODE = 7};
enum {HAS_SCORE = 2, HAS_MOVE = 4};

extern uint64 startHash;

extern uint64 hashBox[ZBOX_SIZE];

extern const int typeLookup[32];

struct TableStats
{
	static uint64 hit;
	static uint64 miss;
	static uint64 scorehit;
	static uint64 scoremiss;
	static uint64 movehit;
	static uint64 movemiss;

	static void clear()
	{
		hit = miss = scorehit = scoremiss = movehit = movemiss = 0;
	}
};

template<class MoveType>
class TransItem
{
public:
	uint64 hash;
	int score;
	int8 depth;
	int8 type;
	MoveType move;

	TransItem()
	: type(NONE_NODE), move()
	{
		hash = score = depth = 0;
	}

	bool getScore(const int alpha, const int beta, const int inDepth, int &outScore) const
	{
		if ((type & HAS_SCORE) && depth >= inDepth) {
			switch (type) {
			case PV_NODE:
				outScore = score;
				TableStats::scorehit++;
				return true;
			case CUT_NODE:
				if (score >= beta) {
					outScore = score;
					TableStats::scorehit++;
					return true;
				}
				break;
			case ALL_NODE:
				if (score <= alpha) {
					outScore = score;
					TableStats::scorehit++;
					return true;
				}
				break;
			case NONE_NODE:
				assert(0);
			}
		}
		TableStats::scoremiss++;
		return false;
	}

	bool getMove(MoveType &inMove) const
	{
		if (type & HAS_MOVE) {
			inMove = move;
			TableStats::movehit++;
			return true;
		} else {
			TableStats::movemiss++;
			return false;
		}
	}
};

template<class MoveType>
class TransTable 
{
private:
	TransItem<MoveType> *table;
	int size;

public:
	TransTable(const int num_MB)
	{
		Rand64 rad;

		for (int i = 0; i < ZBOX_SIZE; i++)
			hashBox[i] = rad.next();
		size = (num_MB * 1048576) / sizeof(TransItem<MoveType>);
		table = new TransItem<MoveType>[size];

		startHash = rad.next();
		TableStats::clear();
	}

	TransTable(const TransTable& other) = delete;

	~TransTable()
	{
		delete[] table;
	}
	
	static sixInt stats()
	{
		return {TableStats::hit, TableStats::miss, TableStats::scorehit,
		TableStats::scoremiss, TableStats::movehit, TableStats::movemiss};
	}

	void clear()
	{
		for (int i = 0; i < size; i++)
			table[i].hash = 0;
	}

	void clearStats() const
	{
		TableStats::clear();
	}

	bool getItem(const uint64 hash, TransItem<MoveType> *&item) const
	{
		item = &table[hash % size];

		if (item->hash == hash) {
			TableStats::hit++;
			return true;
		} else {
			TableStats::miss++;
			return false;
		}
	}

	void setItem(const uint64 hash, const int score, const MoveType &move, const int depth, const int type) const
	{
		TransItem<MoveType>* const item = &table[hash % size];

		item->hash = hash;
		item->score = score;
		item->move = move;
		item->depth = depth;
		item->type = type;
	}
};

#endif
