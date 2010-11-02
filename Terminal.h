#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "ComputerPlayer.h"
#include "Game.h"

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

	bool mainMenu();

	bool playerCmd();

	void displayHelp();

	void run();
};

#endif
