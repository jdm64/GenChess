/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      HumanPlayer class functions
 */

#ifndef __AUTO_PLAY_H__
#define __AUTO_PLAY_H__

#include "Board.h"
#include "Game.h"
#include "ComputerPlayer.h"

class AutoPlay {
private:
	Board board;

	ComputerPlayer *engine;

	Game *game;

	int *plyCount;

	char *winner;

	int numGames;

	bool again;

public:
	AutoPlay(const int NumGames);

	~AutoPlay();

	void printStats();

	void run();
};

#endif
