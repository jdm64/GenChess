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
	MoveList *curr;

	Move *killer1;
	Move *killer2;

	int maxNg;

	vector<bool> tactical;

	ScoresCapture cmpCapture;
	ScoresHiLow cmpHiLow;

public:
	ComputerPlayer(Board* b, char c): Player(b, c)
	{
		// set this for how deep the search is. higher values
		// will take longer, 4 takes about 3mins per move
		maxNg = 3;
		curr = NULL;
	};

	~ComputerPlayer()
	{
		delete[] killer1;
		delete[] killer2;
	};

	void pickMove(MoveList *ptr);

	// swaps any ptr->list[].move that matches
	// killer1/killer2 to the front of ptr->list[]
	void getKillerMoves(MoveList *ptr, int depth);

	// sets move to be one of the killermoves
	void setKillerMoves(Move move, int depth);

	// returns the maximum score in ptr->list[].score
	int getMaxScore(MoveList *ptr);

	int Quiescence(MoveList *&ptr, int alpha, int beta, int depth);

	int NegaScout(MoveList *&ptr, int alpha, int beta, int depth, int limit);

	void think();
};

#endif
