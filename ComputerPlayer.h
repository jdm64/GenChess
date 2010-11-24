#ifndef __COMPUTER_PLAYER_H__
#define __COMPUTER_PLAYER_H__

#include <vector>
#include "Array.h"
#include "TransTable.h"

#define MAX_DEPTH		10
#define MIN_SCORE		-INT_MAX
#define MAX_SCORE		INT_MAX
#define CHECKMATE_SCORE		-(INT_MAX - 2)
#define STALEMATE_SCORE		-(INT_MAX / 2)

class ComputerPlayer {
private:
	MoveList *curr;

	Board *board;

	Array<Move> captureKiller;
	Array<Move> moveKiller;
	Array<Move> placeKiller;
	Array<Move> pvMove;

	int maxNg;

	Array<bool> tactical;
	Array<bool> ismate;

	ScoreSort cmpScore;

public:
	ComputerPlayer(Board* Board) : curr(NULL), board(Board)
	{
		maxNg = 5;
	}

	~ComputerPlayer()
	{
	}

	void printList(MoveList *ptr);

	void pickMove(MoveList *ptr);

	// swaps any ptr->list[].move that matches
	// killer1/killer2 to the front of ptr->list[]
//	void getKillerMoves(MoveList *ptr, int depth);

	// sets move to be one of the killermoves
//	void setKillerMoves(Move move, int depth);

	// returns the maximum score in ptr->list[].score
//	int getMaxScore(MoveList *ptr);

	int Quiescence(int alpha, int beta, int depth);

	bool NegaMoveType(int &alpha, const int beta, int &best, int depth, int limit, Array<Move> &killer, const int type);

	int NegaScout(int alpha, int beta, int depth, int limit);

	void search(int alpha, int beta, int depth, int limit);

	Move think();

	bool parseMove(string s, Move &move);

	void debugTree();
};

#endif
