#ifndef __CVEP_H__
#define __CVEP_H__

#include "Board.h"
#include "Game.h"
#include "ComputerPlayer.h"

class CVEP {
private:
	static const string moveError[NUM_MOVE_ERRORS];

	Board board;

	ComputerPlayer *engine;

	Game *game;

	bool game_done;

	bool quit_engine;

public:
	CVEP();

	~CVEP();

	bool moveResults();

	bool mainCmd();

	bool gameCmd();

	void run();
};

#endif
