/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Game class declaration
 */

#ifndef __GAME_H__
#define __GAME_H__

#include "HumanPlayer.h"
#include "ComputerPlayer.h"

class Game {
private:
	Board board;

	Player *white, *black;

	bool xMode;

public:
	Game(bool, bool, bool);

	~Game()
	{
		delete white;
		delete black;
	}

	void mainMenu();

	void run();
};

#endif