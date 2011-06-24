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
#include "Terminal.h"

const string GenTerminal::PS[3] = {"black", "", "white"};

GenTerminal::GenTerminal(const int white, const int black)
{
	PT[0] = black;
	PT[2] = white;
	playerType = PT + 1;
	playerString = PS + 1;
}

void GenTerminal::displayHelp() const
{
	cout << "Command\t\t\tDescription\n\n"
		<< "new\t\t\tstarts a new game\n"
		<< "quit\t\t\texits the program\n"
		<< "set [fen|zfen]\t\tsets the board using (Z)FEN notation\n"
		<< "print board\t\tredisplays the board\n"
		<< "print [fen|zfen]\tprints the current board's (Z)FEN notation\n\n";
}

bool GenTerminal::playerCmd()
{
	string cmd;

	cout << playerString[board.getStm()] << "> ";
	cin >> cmd;
	if (cmd == "move") {
		string mv;
		GenMove move;

		cin >> mv;
		const int status = board.validMove(mv, board.getStm(), move);
		if (status != VALID_MOVE) {
			cout << moveError[status] << endl;
			return true;
		}
		game->doMove(move);
	} else if (cmd == "new") {
		game->newGame();
	} else if (cmd == "quit") {
		again = false;
	} else if (cmd == "undo") {
		game->undoMove();
		// recheck player, print error if not undo-able
	} else if (cmd == "dundo") {
		game->undoMove();
		game->undoMove();
	} else if (cmd == "list") {
		board.printPieceList();
		return true;
	} else if (cmd == "print") {
		cin >> cmd;

		if (cmd == "board")
			board.printBoard();
		else if (cmd == "fen")
			cout << board.getPosition().printFen() << endl;
		else if (cmd == "zfen")
			cout << board.getPosition().printZfen() << endl;
		else
			cout << "error: required argument (board, fen, zfen)\n";
		return true;
	} else if (cmd == "set") {
		cin >> cmd;

		GenPosition pos;
		if (cmd == "fen") {
			getline(cin, cmd);
			if (!pos.parseFen(cmd)) {
				cout << "error: not valid fen string\n";
				return true;
			}
			board.setBoard(pos);
		} else if (cmd == "zfen") {
			cin >> cmd;
			if (!pos.parseZfen(cmd)) {
				cout << "error: not valid zfen string\n";
				return true;
			}
			board.setBoard(pos);
		} else {
			cout << "error: required argument (fen, zfen)\n";
			return true;
		}
	} else if (cmd == "help") {
		displayHelp();
		return true;
	} else {
		cout << "Error: \"" << cmd << "\" not recognized\n";
		return true;
	}
	return false;
}

bool GenTerminal::mainMenu()
{
	string cmd;

	cout << "main> ";
	cin >> cmd;
	if (cmd == "new") {
		game->newGame();
	} else if (cmd == "quit") {
		again = false;
	} else if (cmd == "undo") {
		game->undoMove();
		// recheck player, print error if not undo-able
	} else if (cmd == "dundo") {
		game->undoMove();
		game->undoMove();
	} else if (cmd == "list") {
		board.printPieceList();
		return true;
	} else if (cmd == "help") {
		displayHelp();
		return true;
	} else {
		cout << "Error: \"" << cmd << "\" not recognized\n";
		return true;
	}
	return false;
}

void GenTerminal::run()
{
	board.printBoard();

	while (again) {
		if (playerType[board.getStm()] == HUMAN) {
			while (playerCmd());
		} else {
			GenMove move = engine->think();
			game->doMove(move);
			cout << "My move is: " << move.toString() << endl;
		}
		if (again)
			board.printBoard();

		switch (board.isMate()) {
		case NOTMATE:
		default:
			break;
		case WHITE_CHECKMATE:
		case BLACK_CHECKMATE:
			cout << playerString[board.getStm() ^ -2] << " got checkmate\n";
			while (mainMenu());
			break;
		case STALEMATE:
			cout << "game is stalemate\n";
			while (mainMenu());
			break;
		}
	}
}

// --- Start Regular Chess ---

const string RegTerminal::PS[3] = {"black", "", "white"};

uint64 RegTerminal::perft(int depth)
{
	if (depth == 0)
		return 1;

	uint64 nodes = 0;
	const MoveFlags undoFlags = board.getMoveFlags();
	RegMoveList *list = board.getPerftMoveList(board.getStm());

	for (int i = 0; i < list->size; i++) {
		uint64 hash = board.hash();

		board.makeP(list->list[i].move);
		nodes += perft(depth - 1);
		board.unmakeP(list->list[i].move, undoFlags);

		if (hash != board.hash())
			cout << "error: " << list->list[i].move.toString() << endl;
	}
	delete list;
	return nodes;
}

void RegTerminal::divide(int depth)
{
	uint64 nodes = 0, children;
	const MoveFlags undoFlags = board.getMoveFlags();
	RegMoveList *list = board.getPerftMoveList(board.getStm());

	for (int i = 0; i < list->size; i++) {
		uint64 hash = board.hash();

		board.makeP(list->list[i].move);
		children = perft(depth - 1);
		cout << list->list[i].move.toString() << " " << children << endl;
		nodes += children;
		board.unmakeP(list->list[i].move, undoFlags);

		if (hash != board.hash())
			cout << "error: " << list->list[i].move.toString() << endl;
	}
	cout << "total " << nodes << endl;
	delete list;
}

RegTerminal::RegTerminal(const int white, const int black)
{
	PT[0] = black;
	PT[2] = white;
	playerType = PT + 1;
	playerString = PS + 1;
}

void RegTerminal::displayHelp() const
{
	cout << "Command\t\t\tDescription\n\n"
		<< "new\t\t\tstarts a new game\n"
		<< "quit\t\t\texits the program\n"
		<< "set [fen|zfen]\t\tsets the board using (Z)FEN notation\n"
		<< "print board\t\tredisplays the board\n"
		<< "print [fen|zfen]\tprints the current board's (Z)FEN notation\n\n";
}

bool RegTerminal::playerCmd()
{
	string cmd;

	cout << playerString[board.getStm()] << "> ";
	cin >> cmd;
	if (cmd == "move") {
		string mv;
		RegMove move;

		cin >> mv;
		int status = board.validMove(mv, board.getStm(), move);
		if (status != VALID_MOVE) {
			cout << moveError[status] << endl;
			return true;
		}
		game->doMove(move);
	} else if (cmd == "new") {
		game->newGame();
	} else if (cmd == "quit") {
		again = false;
	} else if (cmd == "undo") {
		game->undoMove();
		// recheck player, print error if not undo-able
	} else if (cmd == "dundo") {
		game->undoMove();
		game->undoMove();
	} else if (cmd == "list") {
		// TODO
		return true;
	} else if (cmd == "divide") {
		int depth;
		cin >> depth;
		divide(depth);
		return true;
	} else if (cmd == "print") {
		cin >> cmd;

		if (cmd == "board")
			board.printBoard();
		else if (cmd == "fen")
			cout << board.getPosition().printFen() << endl;
		else if (cmd == "zfen")
			cout << board.getPosition().printZfen() << endl;
		else
			cout << "error: required argument (board, fen, zfen)\n";
		return true;
	} else if (cmd == "set") {
		cin >> cmd;

		RegPosition pos;
		if (cmd == "fen") {
			getline(cin, cmd);
			if (!pos.parseFen(cmd)) {
				cout << "error: not valid fen string\n";
				return true;
			}
			board.setBoard(pos);
		} else if (cmd == "zfen") {
			cin >> cmd;
			if (!pos.parseZfen(cmd)) {
				cout << "error: not valid zfen string\n";
				return true;
			}
			board.setBoard(pos);
		} else {
			cout << "error: required argument (fen, zfen)\n";
			return true;
		}
	} else if (cmd == "autoplay") {
		playerType[-1] = COMPUTER;
		playerType[1] = COMPUTER;
	} else if (cmd == "help") {
		displayHelp();
		return true;
	} else {
		cout << "Error: \"" << cmd << "\" not recognized\n";
		return true;
	}
	return false;
}

bool RegTerminal::mainMenu()
{
	string cmd;

	cout << "main> ";
	cin >> cmd;
	if (cmd == "new") {
		game->newGame();
	} else if (cmd == "quit") {
		again = false;
	} else if (cmd == "undo") {
		game->undoMove();
		// recheck player, print error if not undo-able
	} else if (cmd == "dundo") {
		game->undoMove();
		game->undoMove();
	} else if (cmd == "list") {
		// TODO
		return true;
	} else if (cmd == "help") {
		displayHelp();
		return true;
	} else {
		cout << "Error: \"" << cmd << "\" not recognized\n";
		return true;
	}
	return false;
}

void RegTerminal::run()
{
	board.printBoard();

	while (again) {
		if (playerType[board.getStm()] == HUMAN) {
			while (playerCmd());
		} else {
			RegMove move = engine->think();
			game->doMove(move);
			cout << "My move is: " << move.toString() << endl;
		}
		if (again)
			board.printBoard();

		switch (board.isMate()) {
		case NOTMATE:
		default:
			break;
		case WHITE_CHECKMATE:
		case BLACK_CHECKMATE:
			cout << playerString[board.getStm() ^ -2] << " got checkmate\n";
			while (mainMenu());
			break;
		case STALEMATE:
			cout << "game is stalemate\n";
			while (mainMenu());
			break;
		}
	}
}
