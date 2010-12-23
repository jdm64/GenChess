#include "Game.h"

void Game::newGame()
{
	board->reset();
	history.clear();
}

void Game::doMove(const Move move)
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
