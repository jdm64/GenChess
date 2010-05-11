/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      ComputerPlayer class declarations
 */

#ifndef __COMPUTER_PLAYER_H__
#define __COMPUTER_PLAYER_H__

#include <climits>
#include "Player.h"

class ComputerPlayer : public Player {
private:
	MovesPly *curr;

	Move *killer1;
	Move *killer2;

	int maxDepth;

	ScoresCapture cmpCapture;
	ScoresHiLow cmpHiLow;

public:
	ComputerPlayer(Board* b, char c): Player(b, c)
	{
		// set this for how deep the search is. higher values
		// will take longer, 4 takes about 3mins per move
		maxDepth = 3;
		killer1 = new Move[maxDepth];
		killer2 = new Move[maxDepth];
		curr = NULL;
	};

	~ComputerPlayer()
	{
		delete[] killer1;
		delete[] killer2;
	};

	void pickMove(MovesPly *ptr, int score);

	// swaps any ptr->list[].move that matches
	// killer1/killer2 to the front of ptr->list[]
	void getKillerMoves(MovesPly *ptr, int depth);

	// sets move to be one of the killermoves
	void setKillerMoves(Move move, int depth);

	// returns the maximum score in ptr->list[].score
	int getMaxScore(MovesPly *ptr);

	int Quiescence(MovesPly *&ptr, int alpha, int beta, int depth);

	int NegaScout(MovesPly *&ptr, int alpha, int beta, int depth, int limit);

	void think();
};

#endif
