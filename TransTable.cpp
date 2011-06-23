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

#include "TransTable.h"
#include "Rand64.h"

uint64 startHash;

uint64 hashBox[ZBOX_SIZE];

GenTransTable *gtt;

const int typeLookup[32] = {0, 0, 0, 0, 0, 0,  0,  0,
			    1, 1, 2, 2, 3, 3,  4,  5,
			    6, 6, 6, 6, 6, 6,  6,  6,
			    7, 7, 8, 8, 9, 9, 10, 11};

GenTransItem::GenTransItem()
{
	hash = 0;
	score = 0;
	depth = 0;
	type = NONE_NODE;
}

bool GenTransItem::getScore(const int alpha, const int beta, const int inDepth, int &outScore) const
{
	if ((type & HAS_SCORE) && depth >= inDepth) {
		switch (type) {
		case PV_NODE:
			outScore = score;
			gtt->scorehit++;
			return true;
		case CUT_NODE:
			if (score >= beta) {
				outScore = score;
				gtt->scorehit++;
				return true;
			}
			break;
		case ALL_NODE:
			if (score <= alpha) {
				outScore = score;
				gtt->scorehit++;
				return true;
			}
			break;
		case NONE_NODE:
			assert(0);
		}
	}
	gtt->scoremiss++;
	return false;
}

bool GenTransItem::getMove(GenMove &inMove) const
{
	if (type & HAS_MOVE) {
		inMove = move;
		gtt->movehit++;
		return true;
	} else {
		gtt->movemiss++;
		return false;
	}
}

GenTransTable::GenTransTable(const int num_MB)
{
	Rand64 rad;

	for (int i = 0; i < ZBOX_SIZE; i++)
		hashBox[i] = rad.next();
	size = (num_MB * 1048576) / sizeof(GenTransItem);
	table = new GenTransItem[size];

	startHash = rad.next();
	hit = miss = scorehit = scoremiss = movehit = movemiss = 0;
}

GenTransTable::~GenTransTable()
{
	delete[] table;
}

sixInt GenTransTable::stats() const
{
	return {hit, miss, scorehit, scoremiss, movehit, movemiss};
}

void GenTransTable::clear()
{
	for (int i = 0; i < size; i++)
		table[i].hash = 0;
}

void GenTransTable::clearStats()
{
	hit = miss = scorehit = scoremiss = movehit = movemiss = 0;
}

bool GenTransTable::getItem(const uint64 hash, GenTransItem *&item)
{
	item = &table[hash % size];

	if (item->hash == hash) {
		hit++;
		return true;
	} else {
		miss++;
		return false;
	}
}

void GenTransTable::setItem(const uint64 hash, const int score, const GenMove &move, const int8 depth, const int8 type) const
{
	GenTransItem* const item = &table[hash % size];

	item->hash = hash;
	item->score = score;
	item->move = move;
	item->depth = depth;
	item->type = type;
}
