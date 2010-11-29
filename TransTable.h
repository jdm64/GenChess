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
	char depth;
	char type;

	TransItem()
	{
		hash = 0;
		type = NONE_NODE;
	}

	bool getScore(const int alpha, const int beta, const int inDepth, int &outScore)
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

	bool getMove(Move &inMove)
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
	int size;
	uint64 hit, miss;

public:
	TransTable(const int num_MB);

	~TransTable();
	
	intPair hitStats();

	bool getItem(const uint64 hash, TransItem *&item);

	void setItem(const uint64 hash, const int score, const Move move, const char depth, const char type);
};

extern TransTable *tt;

#endif
