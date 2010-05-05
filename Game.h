#ifndef __GAME_H__
#define __GAME_H__

#include "HumanPlayer.h"
#include "ComputerPlayer.h"

class Game {
private:
	Board board;

	Player *white, *black;

public:
	Game(bool, bool, bool);

	void run();
};

#endif