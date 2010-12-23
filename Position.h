#ifndef __POSITION_H__
#define __POSITION_H__

#include "Defines.h"
#include "Move.h"
#include "MoveLookup.h"

class Position {
public:
	char square[64];
	char piece[32];
	int ply;

	void reset();

	bool setPiece(const char loc, const char type);

	bool incheck(const char color);

	bool parseFen(const string st);

	bool parseZfen(const string st);

	string printFen() const;

	string printZfen() const;
};

#endif