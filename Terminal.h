/* GenChess, a genesis chess engine
 * Copyright (C) 2010-2014, Justin Madru (justin.jdm64@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TERMINAL_H__
#define __TERMINAL_H__

#include "UI.h"

template<class MoveType>
class Terminal : public UI<MoveType>
{
	using UI<MoveType>::moveError;
	using UI<MoveType>::board;
	using UI<MoveType>::engine;
	using UI<MoveType>::tt;
	using UI<MoveType>::game;
	using UI<MoveType>::again;
	using UI<MoveType>::perft;

private:
	static const string PS[3];

	const string *playerString;

	int PT[3];

	int *playerType;

	void divide(int depth)
	{
		uint64 nodes = 0, children;
		const MoveFlags undoFlags = board.getMoveFlags();

		MoveList<MoveType> *list = board.getMoveList(board.getStm(), MoveClass::ALL);
		for (auto& item : *list) {
			board.make(item.move);
			children = perft(depth - 1);
			cout << item.move.toString() << " " << children << endl;
			nodes += children;
			board.unmake(item.move, undoFlags);
		}
		delete list;

		cout << "total " << nodes << endl;
	}

	static void displayHelp()
	{
		cout << "Command\t\t\tDescription\n\n"
			<< "new\t\t\tstarts a new game\n"
			<< "quit\t\t\texits the program\n"
			<< "set [fen|zfen]\t\tsets the board using (Z)FEN notation\n"
			<< "print board\t\tredisplays the board\n"
			<< "print [fen|zfen]\tprints the current board's (Z)FEN notation\n\n";
	}

	bool playerCmd()
	{
		string cmd;

		cout << playerString[board.getStm()] << "> ";
		cin >> cmd;
		if (cmd == "move") {
			string mv;
			MoveType move;

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
			cout << board.printPieceList() << endl;
			return true;
		} else if (cmd == "divide") {
			int depth;
			cin >> depth;
			divide(depth);
			return true;
		} else if (cmd == "print") {
			cin >> cmd;

			if (cmd == "board")
				cout << board.printBoard();
			else if (cmd == "fen")
				cout << board.printFen() << endl;
			else if (cmd == "zfen")
				cout << board.printZfen() << endl;
			else
				cout << "error: required argument (board, fen, zfen)\n";
			return true;
		} else if (cmd == "set") {
			cin >> cmd;

			Position<MoveType> pos;
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

	bool mainMenu()
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
			cout << board.printPieceList() << endl;
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
public:
	Terminal(const int white, const int black)
	{
		PT[0] = black;
		PT[2] = white;
		playerType = PT + 1;
		playerString = PS + 1;
	}

	void run()
	{
		cout << board.printBoard();
		while (again) {
			if (playerType[board.getStm()] == HUMAN) {
				while (playerCmd());
			} else {
				MoveType move = engine->think();
				game->doMove(move);
				cout << "My move is: " << move.toString() << endl;
			}
			if (again)
				cout << board.printBoard();

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
};

template<class MoveType>
const string Terminal<MoveType>::PS[3] = {"black", "", "white"};

typedef Terminal<GenMove> GenTerminal;
typedef Terminal<RegMove> RegTerminal;

#endif
