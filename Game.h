/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Game class declaration
 */

#ifndef __GAME_H__
#define __GAME_H__

#include "Board.h"

class Game {
private:
	Board *board;

	vector<Move> history;

public:
	Game(Board *Board) : board(Board) {}

	void newGame();

	void doMove(Move move);

	bool undoMove();
};

#endif
