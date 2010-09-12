#include "TransTable.h"
#include "Defines.h"
#include "Rand64.h"

using namespace std;

uint64 startHash;

uint64 hashBox[ZBOX_SIZE];

TransTable *tt;

const int typeLookup[32] = {0, 0, 0, 0, 0, 0,  0,  0,
			    1, 1, 2, 2, 3, 3,  4,  5,
			    6, 6, 6, 6, 6, 6,  6,  6,
			    7, 7, 8, 8, 9, 9, 10, 11};

TransTable::TransTable(int num_MB)
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

intPair TransTable::hitStats()
{
	return {hit, miss};
}

int TransTable::getScore(Board *board)
{
	uint64 hash = board->hash();
	
	if (table[hash % size].hash == hash) {
		hit++;
		return table[hash % size].score;
	} else {
		miss++;
		return board->eval();
	}
}

bool TransTable::getMove(Board *board, Move &move)
{
	uint64 hash = board->hash();
	
	if (table[hash % size].hash != hash) {
		miss++;
		return false;
	} else if (table[hash % size].type != CUT_NODE) {
		miss++;
		return false;
	}
	hit++;
	move = table[hash % size].move;
	return true;
}

void TransTable::setScore(Board *board, char depth, int score)
{
	// TODO: determin replacement strategy
	uint64 hash = board->hash();
	
	table[hash % size].hash = hash;
	table[hash % size].score = score;
	table[hash % size].depth = depth;
	table[hash % size].type = ALL_NODE;
}

void TransTable::setBest(Board *board, char depth, int score, Move move)
{
	// TODO: determin replacement strategy
	uint64 hash = board->hash();

	table[hash % size].hash = hash;
	table[hash % size].score = score;
	table[hash % size].move = move;
	table[hash % size].depth = depth;
	table[hash % size].type = CUT_NODE;
}