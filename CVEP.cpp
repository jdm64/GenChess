/*	GenChess, a genesis chess engine
	Copyright (C) 2010, Justin Madru (jdm64@gawab.com)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "CVEP.h"

bool CVEP::moveResults()
{
	switch (board.isMate()) {
	case NOTMATE:
	default:
		if (board.getPly() > 600) {
			cout << "result N-N (too many moves)" << endl;
			return false;
		} else if (board.incheck(board.getStm())) {
			cout << "special check" << endl;
		} else {
			cout << "ok" << endl;
		}
		return true;
	case WHITE_CHECKMATE:
	case BLACK_CHECKMATE:
		cout << "result " << ((board.getStm() == BLACK)? "1-0" : "0-1") << " (checkmate)" << endl;
		return false;
	case STALEMATE:
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
		int status = board.validMove(mv, board.getStm(), move);
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
	} else if (cmd == "clearhash") {
		tt->clear();
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
