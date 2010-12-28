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

#ifndef __TRANS_TABLE_H__
#define __TRANS_TABLE_H__

#include "Board.h"

//#define COLLISION_DEBUG

#define ZBOX_SIZE 781
#define WTM_HASH 780
#define HOLD_START 768

enum {NONE_NODE = 0, ALL_NODE = 3, CUT_NODE = 6, PV_NODE = 7};
enum {HAS_SCORE = 2, HAS_MOVE = 4};

extern uint64 startHash;

extern uint64 hashBox[ZBOX_SIZE];

extern const int typeLookup[32];

struct TransItem {
	uint64 hash;
	int score;
	Move move;
	int8 depth;
	int8 type;

	TransItem()
	{
		hash = 0;
		type = NONE_NODE;
		depth = 0;
	}

	bool getScore(const int alpha, const int beta, const int inDepth, int &outScore) const
	{
		if ((type & HAS_SCORE) && depth >= inDepth) {
			switch (type) {
			case PV_NODE:
				outScore = score;
				return true;
			case CUT_NODE:
				if (score >= beta) {
					outScore = score;
					return true;
				}
				break;
			case ALL_NODE:
				if (score <= alpha) {
					outScore = score;
					return true;
				}
				break;
			case NONE_NODE:
				assert(0);
			}
		}
		return false;
	}

	bool getMove(Move &inMove) const
	{
		if (type & HAS_MOVE) {
			inMove = move;
			return true;
		} else {
			return false;
		}
	}
};

class TransTable {
private:
	TransItem *table;
	uint64 hit, miss;
	int size;

public:
	TransTable(const int num_MB);

	~TransTable();
	
	intPair hitStats() const;

	bool getItem(const uint64 hash, TransItem *&item);

	void setItem(const uint64 hash, const int score, const Move move, const int8 depth, const int8 type);
};

extern TransTable *tt;

#endif
