#ifndef __GAME_H__
#define __GAME_H__

#include <vector>
#include "Board.h"

class Game {
private:
	Board *board;

	vector<Move> history;

public:
	Game(Board *Board) : board(Board) {}

	void newGame();

	void doMove(const Move move);

	bool undoMove();
};

#endif
