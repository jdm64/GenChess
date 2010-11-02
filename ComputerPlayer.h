#ifndef __COMPUTER_PLAYER_H__
#define __COMPUTER_PLAYER_H__

#include <vector>
#include "Array.h"
#include "TransTable.h"

class ComputerPlayer {
private:
	MoveList *curr;

	Board *board;

	Move *killer1;
	Move *killer2;

	int maxNg;

	Array<bool> tactical;

	ScoreSort cmpScore;

public:
	ComputerPlayer(Board* Board) : curr(NULL), board(Board)
	{
		maxNg = 5;
	}

	~ComputerPlayer()
	{
		delete[] killer1;
		delete[] killer2;
	}

	void printList(MoveList *ptr);

	void pickMove(MoveList *ptr);

	// swaps any ptr->list[].move that matches
	// killer1/killer2 to the front of ptr->list[]
	void getKillerMoves(MoveList *ptr, int depth);

	// sets move to be one of the killermoves
	void setKillerMoves(Move move, int depth);

	// returns the maximum score in ptr->list[].score
	int getMaxScore(MoveList *ptr);

	int Quiescence(int alpha, int beta, int depth);

	int NegaScout(int alpha, int beta, int depth, int limit);

	Move think();

	bool parseMove(string s, Move &move);

	void debugTree();
};

#endif
