/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Game class functions
 */

#include "Game.h"

Game::Game(bool whiteAI, bool blackAI, bool XMode)
{
	if (whiteAI)

		white = new ComputerPlayer(&board, WHITE);
	else
		white = new HumanPlayer(&board, WHITE);
	if (blackAI)
		black = new ComputerPlayer(&board, BLACK);
	else
		black = new HumanPlayer(&board, BLACK);
	white->setPrintMode(XMode);
	black->setPrintMode(XMode);
}

void Game::run()
{
	bool check;
	char state;
	int nMoves;
	string player;

	while (true) {
		state = board.currentPlayer();

		if (state == WHITE || state == BLACK) {
			check = board.inCheck(state);
			nMoves = board.getNumMoves(state);
			player = (state == WHITE)? "white":"black";

			if (check && nMoves)
				cout << "! " << player << " in check\n";
			else if (check && !nMoves)
				cout << "$ " << player << " in checkmate\n";
			else if (!check && !nMoves)
				cout << "$ stalemate\n";
		}
		switch (state) {
		case WHITE:
			white->think();
			break;
		case BLACK:
			black->think();
			break;
		case QUIT_GAME:
			return;
		case NEW_GAME:
			// TODO how to pass parameters from within player?
			return;
		}
	}
}
