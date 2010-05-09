#ifndef __MOVE_LOOKUP_H__
#define __MOVE_LOOKUP_H__

#include "Data.h"

class MoveLookup {
private:
	static char *board;

	char *lookup;

	int type, loc, offset;
public:
	static void setBoard(char *b);

	void setPiece(int Index);

	bool setLoc(int Index);

	int nextAttack();

	bool isAttacked();
};

#endif