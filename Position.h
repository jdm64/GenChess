#ifndef __POSITION_H__
#define __POSITION_H__

#include <cstring>
#include "Defines.h"
#include "Move.h"
#include "MoveLookup.h"

class Position {
public:
	char square[64];
	char piece[32];
	int ply;

	void reset();

	bool setPiece(char loc, char type);

	bool incheck(const char color);

	bool parseFen(string st);

	bool parseZfen(string st);

	string printFen();

	string printZfen();
};

#endif