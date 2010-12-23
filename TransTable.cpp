#include "TransTable.h"
#include "Rand64.h"

uint64 startHash;

uint64 hashBox[ZBOX_SIZE];

TransTable *tt;

const int typeLookup[32] = {0, 0, 0, 0, 0, 0,  0,  0,
			    1, 1, 2, 2, 3, 3,  4,  5,
			    6, 6, 6, 6, 6, 6,  6,  6,
			    7, 7, 8, 8, 9, 9, 10, 11};

TransTable::TransTable(const int num_MB)
{
	Rand64 rad;

	for (int i = 0; i < ZBOX_SIZE; i++)
		hashBox[i] = rad.next();
	size = (num_MB * 1048576) / sizeof(TransItem);
	table = new TransItem[size];

	startHash = rad.next();
	for (int i = HOLD_START; i < ZBOX_SIZE; i++)
		startHash += hashBox[i];
	hit = miss = 0;
}

TransTable::~TransTable()
{
	delete[] table;
}

intPair TransTable::hitStats() const
{
	return {hit, miss};
}

bool TransTable::getItem(const uint64 hash, TransItem *&item)
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

void TransTable::setItem(const uint64 hash, const int score, const Move move, const char depth, const char type)
{
	TransItem *item = &table[hash % size];

	if (depth < item->depth)
		return;

	item->hash = hash;
	item->score = score;
	item->move = move;
	item->depth = depth;
	item->type = type;
}
