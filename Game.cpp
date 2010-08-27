/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Game class functions
 */

#include "Game.h"

void Game::newGame()
{
	board->reset();
	history.clear();
}

void Game::doMove(Move move)
{
	board->make(move);
	history.push_back(move);
}

bool Game::undoMove()
{
	if (!history.size())
		return false;
	board->unmake(history.back());
        history.pop_back();

        return true;
}
