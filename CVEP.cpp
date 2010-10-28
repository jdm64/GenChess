#include <iostream>
#include "CVEP.h"
#include "TransTable.h"

const string CVEP::moveError[NUM_MOVE_ERRORS] = {
	"", "That's an invalid move format",
	"That move doesn't reference a valid piece",
	"That is not your piece", "You must place your king first",
	"Can't place piece on another piece", "Can't capture your own piece",
	"That piece can't move like that", "That move places you in check",
	"Can't check by placing a piece"
};


CVEP::CVEP()
{
	quit_engine = false;
	game = new Game(&board);
	engine = new ComputerPlayer(&board);
	tt = new TransTable(32);
}

CVEP::~CVEP()
{
	delete game;
	delete engine;
	delete tt;
}

bool CVEP::moveResults()
{
	switch (board.isMate()) {
	case NOT_MATE:
	default:
		if (board.getPly() > 600) {
			cout << "result N-N (too many moves)" << endl;
			return false;
		} else if (board.incheck(board.currPlayer())) {
			cout << "special check" << endl;
		} else {
			cout << "ok" << endl;
		}
		return true;
	case CHECK_MATE:
		cout << "result " << (((board.currPlayer() ^ -2) == WHITE)? "1-0" : "0-1") << " (checkmate)" << endl;
		return false;
	case STALE_MATE:
		cout << "result N-N (stalemate)" << endl;
		return false;
	}
}

bool CVEP::mainCmd()
{
	string cmd;

	cin >> cmd;
	if (cmd == "newgame") {
		game->newGame();
	} else if (cmd == "quit") {
		quit_engine = true;
	} else if (cmd == "undo") {
		game->undoMove();
	} else if (cmd == "retract") {
		game->undoMove();
		game->undoMove();
	} else {
		cout << "error (command not recognized): " << cmd << endl;
		return true;
	}
	return false;
}

bool CVEP::gameCmd()
{
	string cmd;

	cin >> cmd;
	if (cmd == "move") {
		string mv;
		Move move;

		cin >> mv;
		int status = board.validMove(mv, board.currPlayer(), move);
		if (status != VALID_MOVE) {
			cout << "illegal (" << moveError[status] << "): " << mv << endl;
		} else {
			game->doMove(move);
			return moveResults();
		}
	} else if (cmd == "newgame") {
		game->newGame();
	} else if (cmd == "quit") {
		quit_engine = true;
		return false;
	} else if (cmd == "undo") {
		game->undoMove();
		// recheck player, print error if not undo-able
	} else if (cmd == "retract") {
		game->undoMove();
		game->undoMove();
	} else if (cmd == "go") {
		Move move = engine->think();
		game->doMove(move);
		cout << "move " << move.toString() << endl;
		return moveResults();
	} else {
		cout << "error (command not recognized): " << cmd << endl;
	}
	return true;
}

void CVEP::run()
{
	while (true) {
		while (mainCmd());

		if (quit_engine)
			break;

		while (gameCmd());

		if (quit_engine)
			break;
	}
}
