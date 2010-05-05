#ifndef __COMPUTER_PLAYER_H__
#define __COMPUTER_PLAYER_H__

#include <climits>
#include "Player.h"

class ComputerPlayer : public Player {
private:
	MovesPly *root;
	MovesPly *curr;

	Move *killer1;
	Move *killer2;

	int maxDepth;

	ScoresCapture cmpCapture;
	ScoresHiLow cmpHiLow;

public:
	ComputerPlayer(Board* b, char c): Player(b, c)
	{
		killer1 = new Move[maxDepth];
		killer2 = new Move[maxDepth];
		maxDepth = 3;
		curr = NULL;
	};

	void pickMove(MovesPly* ptr, int score);

	void getKillerMoves(MovesPly *ptr, int depth);

	void setKillerMoves(Move move, int depth);

	int getMaxScore(MovesPly *ptr);

	int NegaScout(int alpha, int beta, int depth, MovesPly *&ptr);

	void think();
};

#endif
