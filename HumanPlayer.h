#ifndef __HUMAN_PLAYER_H__
#define __HUMAN_PLAYER_H__

#include "Player.h"

class HumanPlayer : public Player {
private:
	static string errorMessage[NUM_ERRORS];

public:
	HumanPlayer(Board* b, char c) : Player(b, c) {};

	int parseCommand(string s);

	int parseMove(string s);

	void think();
};

#endif