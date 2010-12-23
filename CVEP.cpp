#include <iostream>
#include "CVEP.h"

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
		again = false;
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
	} else if (cmd == "setboard") {
		cin >> cmd;

		Position pos;
		if (cmd == "fen") {
			getline(cin, cmd);
			if (!pos.parseFen(cmd)) {
				cout << "error (invalid format)" << endl;
			} else {
				board.setBoard(pos);
				cout << "ok" << endl;
			}
		} else if (cmd == "zfen") {
			cin >> cmd;
			if (!pos.parseZfen(cmd)) {
				cout << "error (invalid format)" << endl;
			} else {
				board.setBoard(pos);
				cout << "ok" << endl;
			}
		} else {
			cout << "error (must specify fen or zfen)" << endl;
		}
	} else {
		cout << "error (command not recognized): " << cmd << endl;
	}
	return true;
}

bool CVEP::mainCmd()
{
	string cmd;

	cin >> cmd;
	if (cmd == "newgame") {
		game->newGame();
	} else if (cmd == "quit") {
		again = false;
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

void CVEP::run()
{
	while (true) {
		while (mainCmd());

		if (!again)
			break;

		while (gameCmd());

		if (!again)
			break;
	}
}
