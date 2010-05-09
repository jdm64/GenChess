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
	xMode = XMode;
	white->setPrintMode(xMode);
	black->setPrintMode(xMode);
}

void Game::mainMenu()
{
	string cmd;

	while (true) {
		if (!xMode)
			cout << "main> ";
		cin >> cmd;

		if (cmd == "quit") {
			board.quitGame();
			return;
		} else if (cmd == "list") {
			board.printPieceList();
		} else if (cmd == "new") {
			board.newGame();
			return;
		} else if (cmd == "help") {
			cout << "list\tshows where each piece is located\n"
				<< "new\tstarts a new game\n"
				<< "quit\texits the game\n"
				<< "help\tdisplays this help message\n";
		} else {
			cout << "? game is over, select new game\n";
		}
	}
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

			if (check && nMoves) {
				cout << "! " << player << " in check\n";
			} else if (check && !nMoves) {
				cout << "$ " << player << " in checkmate\n";
				mainMenu();
				continue;
			} else if (!check && !nMoves) {
				cout << "$ stalemate\n";
				mainMenu();
				continue;
			}
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
		}
	}
}
