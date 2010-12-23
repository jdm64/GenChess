#include <iostream>
#include "Terminal.h"

const string Terminal::PS[3] = {"black", "", "white"};

Terminal::Terminal(const int white, const int black)
{
	PT[0] = black;
	PT[2] = white;
	playerType = PT + 1;
	playerString = PS + 1;
}

void Terminal::displayHelp()
{
	cout << "help messages comming soon...";
}

bool Terminal::playerCmd()
{
	string cmd;

	cout << playerString[board.currPlayer()] << "> ";
	cin >> cmd;
	if (cmd == "move") {
		string mv;
		Move move;

		cin >> mv;
		int status = board.validMove(mv, board.currPlayer(), move);
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
	} else if (cmd == "set") {
		cin >> cmd;

		Position pos;
		if (cmd == "fen") {
			getline(cin, cmd);
			if (!pos.parseFen(cmd))
				cout << "error in setting board\n";
			else
				board.setBoard(pos);
		} else if (cmd == "zfen") {
			cin >> cmd;
			if (!pos.parseZfen(cmd))
				cout << "error in setting board\n";
			else
				board.setBoard(pos);
		} else {
			cout << "error in setting board\n";
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

bool Terminal::mainMenu()
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

void Terminal::run()
{
	board.printBoard();

	while (again) {
		if (playerType[board.currPlayer()] == HUMAN) {
			while (playerCmd());
		} else {
			Move move = engine->think();
			game->doMove(move);
			cout << "My move is: " << move.toString() << endl;
		}
		if (again)
			board.printBoard();

		switch (board.isMate()) {
		case NOT_MATE:
		default:
			break;
		case CHECK_MATE:
			cout << playerString[board.currPlayer() ^ -2] << " got checkmate\n";
			while (mainMenu());
			break;
		case STALE_MATE:
			cout << "game is stalemate\n";
			while (mainMenu());
			break;
		}
	}
}
