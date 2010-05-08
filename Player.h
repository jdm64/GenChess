/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Player class declarations
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Board.h"

using namespace std;

class Player {
protected:
	Board *board;

	char color, xMode;

	string color_str;

public:
	Player(Board *b, char c) : board(b), color(c)
	{
		if (color == WHITE)
			color_str = string("white");
		else
			color_str = string("black");
	};

	~Player() {};

	void setPrintMode(char type)
	{
		xMode = type;
	};

	virtual void think() {};
};

#endif