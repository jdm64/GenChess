/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      HumanPlayer class functions
 */

#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "Board.h"
#include "Game.h"
#include "ComputerPlayer.h"

class Terminal {
private:
	static const string moveError[NUM_MOVE_ERRORS];

	Board board;
	
	ComputerPlayer *engine;

	Game *game;

	static const string PS[3];
	
	const string *playerString;

	char PT[3];
	
	char *playerType;

	bool again;

public:
	Terminal(const int white, const int black);

	~Terminal();

	bool playerCmd();

	void displayHelp();

	void run();
};

#endif
