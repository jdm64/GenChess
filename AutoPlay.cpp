#include "AutoPlay.h"
#include "TransTable.h"
#include <iostream>

AutoPlay::AutoPlay(const int NumGames)
{
	numGames = NumGames;
	again = true;

	winner = new char[numGames];
	plyCount = new int[numGames];
	game = new Game(&board);
	engine = new ComputerPlayer(&board);
	tt = new TransTable(32);
}

AutoPlay::~AutoPlay()
{
	delete[] winner;
	delete[] plyCount;

	delete game;
	delete engine;
	delete tt;
}

void AutoPlay::printStats()
{
	int winSum[3] = {0, 0, 0}, avgPly = 0;

	for (int i = 0; i < numGames; i++) {
		winSum[winner[i] + 1]++;
		avgPly += plyCount[i];
	}
	avgPly /= numGames;

	cout << "White: " << winSum[2] << " Black: " << winSum[0]
		<< " Tie: " << winSum[1] << " AvgPly: " << avgPly << endl;
}

void AutoPlay::run()
{
	for (int i = 0; i < numGames; i++) {
		while (again) {
			Move move = engine->think();
			game->doMove(move);

			switch (board.isMate()) {
			case NOT_MATE:
			default:
				if (board.getPly() > 600) {
					again = false;
					winner[i] = 0;
					plyCount[i] = 600;
					game->newGame();
				}
				break;
			case CHECK_MATE:
				again = false;
				winner[i] = board.currPlayer() ^ -2;
				plyCount[i] = board.getPly();
				game->newGame();
				break;
			case STALE_MATE:
				again = false;
				winner[i] = 0;
				plyCount[i] = board.getPly();
				game->newGame();
				break;
			}
		}
		again = true;
	}
	printStats();
}
