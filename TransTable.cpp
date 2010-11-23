#include "TransTable.h"
#include "Rand64.h"

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

#ifdef COLLISION_DEBUG
int TransTable::numCollisions()
{
	return col_count;
}

bool TransTable::checkPosition(Board *board)
{
	uint64 hash = board->hash();

	if (table[hash % size].hash != hash)
		return true;
	if (board->getPosition() == table[hash % size].pos)
		return true;
	board->dumpDebug();
	table[hash % size].pos.print();
	return false;
}
#endif

int TransTable::getScore(Board *board)
{
	uint64 hash = board->hash();
	
	if (table[hash % size].hash == hash) {
#ifdef COLLISION_DEBUG
		if (!(board->getPosition() == table[hash % size].pos)) {
			board->dumpDebug();
			table[hash % size].pos.print();
			col_count++;
			assert(0);
		}
#endif
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
	} else if (table[hash % size].type == ALL_NODE) {
		miss++;
		return false;
	}
#ifdef COLLISION_DEBUG
	if (!(board->getPosition() == table[hash % size].pos))
		col_count++;
#endif
	hit++;
	move = table[hash % size].move;
	return true;
}

void TransTable::setScore(Board *board, char depth, int score)
{
	uint64 hash = board->hash();

	if (table[hash % size].type > ALL_NODE)
		return;

	table[hash % size].hash = hash;
	table[hash % size].score = score;
	table[hash % size].depth = depth;
	table[hash % size].type = ALL_NODE;
#ifdef COLLISION_DEBUG
	table[hash % size].pos = board->getPosition();
#endif
}

void TransTable::setBest(Board *board, char depth, int score, Move move)
{
	uint64 hash = board->hash();

	if (table[hash % size].type > CUT_NODE)
		return;

	table[hash % size].hash = hash;
	table[hash % size].score = score;
	table[hash % size].move = move;
	table[hash % size].depth = depth;
	table[hash % size].type = CUT_NODE;
#ifdef COLLISION_DEBUG
	table[hash % size].pos = board->getPosition();
#endif
}

void TransTable::setPV(Board *board, char depth, int score, Move move)
{
	uint64 hash = board->hash();

	if (table[hash % size].type > PV_NODE)
		return;

	table[hash % size].hash = hash;
	table[hash % size].score = score;
	table[hash % size].move = move;
	table[hash % size].depth = depth;
	table[hash % size].type = PV_NODE;
#ifdef COLLISION_DEBUG
	table[hash % size].pos = board->getPosition();
#endif
}
