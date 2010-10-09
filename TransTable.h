#ifndef __TRANS_TABLE_H__
#define __TRANS_TABLE_H__

#include "Board.h"

//#define COLLISION_DEBUG

#define ZBOX_SIZE 781
#define WTM_HASH 780
#define HOLD_START 768

enum {NONE_NODE = 0, ALL_NODE = 1, CUT_NODE = 3};

extern uint64 startHash;

extern uint64 hashBox[ZBOX_SIZE];

extern const int typeLookup[32];

struct TransItem {
#ifdef COLLISION_DEBUG
	Position pos;
#endif
	uint64 hash;
	int score;
	Move move;
	char depth;
	char type;

	TransItem()
	{
		hash = 0;
		type = NONE_NODE;
	}
};

class TransTable {
private:
	TransItem *table;
	int size, col_count;
	uint64 hit, miss;

public:
	TransTable(int num_MB);

	~TransTable();
	
	intPair hitStats();

#ifdef COLLISION_DEBUG
	int numCollisions();
#endif
	bool checkPosition(Board *board);

	int getScore(Board *board);

	bool getMove(Board *board, Move &move);

	void setScore(Board *board, char depth, int score);

	void setBest(Board *board, char depth, int score, Move move);
};

extern TransTable *tt;

#endif
