#ifndef __UI_H__
#define __UI_H__

#include "ComputerPlayer.h"
#include "Game.h"

class UI {
protected:
	static const string moveError[NUM_MOVE_ERRORS];

	Board board;
	
	ComputerPlayer *engine;

	Game *game;

	bool again;

public:
	UI();

	virtual ~UI();

	virtual void run() = 0;
};

#endif
