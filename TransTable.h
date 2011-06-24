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

class GenTransTable;
extern GenTransTable *gtt;

class GenTransItem {
public:
	uint64 hash;
	int score;
	GenMove move;
	int8 depth;
	int8 type;

	GenTransItem();

	bool getScore(const int alpha, const int beta, const int inDepth, int &outScore) const;

	bool getMove(GenMove &inMove) const;
};

class GenTransTable {
private:
	GenTransItem *table;
	int size;

public:
	uint64 hit, miss, scorehit, scoremiss, movehit, movemiss;

	GenTransTable(const int num_MB);

	~GenTransTable();
	
	sixInt stats() const;

	void clear();

	void clearStats();

	bool getItem(const uint64 hash, GenTransItem *&item);

	void setItem(const uint64 hash, const int score, const GenMove &move, const int8 depth, const int8 type) const;
};

// --- Start Regular Chess ---

class RegTransTable;
extern RegTransTable *rtt;

class RegTransItem {
public:
	uint64 hash;
	int score;
	RegMove move;
	int8 depth;
	int8 type;

	RegTransItem();

	bool getScore(const int alpha, const int beta, const int inDepth, int &outScore) const;

	bool getMove(RegMove &inMove) const;
};

class RegTransTable {
private:
	RegTransItem *table;
	int size;

public:
	uint64 hit, miss, scorehit, scoremiss, movehit, movemiss;

	RegTransTable(const int num_MB);

	~RegTransTable();

	sixInt stats() const;

	void clear();

	void clearStats();

	bool getItem(const uint64 hash, RegTransItem *&item);

	void setItem(const uint64 hash, const int score, const RegMove &move, const int8 depth, const int8 type) const;
};

#endif
