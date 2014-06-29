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

#include <iostream>
#include <unistd.h>
#include "Terminal.h"
#include "CVEP.h"

GenBoard board;

/* depth=1	nodes=64
 * depth=2	nodes=3612
 * depth=3	nodes=953632
 * depth=4	nodes=248188772
 * depth=5	nodes=64518625428
 */
uint64 perft(int depth)
{
	GenMoveList *list;
	uint64 nodes;
	MoveFlags undoFlags = board.getMoveFlags();

	if (depth == 1) {
		list = board.getMoveList(board.getStm(), MoveClass::ALL);
		nodes = list->size;
		delete list;
		return nodes;
	}
	nodes = 0;

	// Placement Moves
	list = board.getMoveList(board.getStm(), MoveClass::PLACE);
	if (list->size == 0)
		goto moves;
	for (int i = 0; i < list->size; i++) {
		board.make(list->list[i].move);
		nodes += perft(depth - 1);
		board.unmake(list->list[i].move, undoFlags);
	}
moves:
	delete list;
	// Movement Moves
	list = board.getMoveList(board.getStm(), MoveClass::MOVE);
	if (list->size == 0)
		goto captures;
	for (int i = 0; i < list->size; i++) {
		board.make(list->list[i].move);
		nodes += perft(depth - 1);
		board.unmake(list->list[i].move, undoFlags);
	}
captures:
	delete list;
	// Capture Moves
	list = board.getMoveList(board.getStm(), MoveClass::CAPTURE);
	if (list->size == 0)
		goto done;
	for (int i = 0; i < list->size; i++) {
		board.make(list->list[i].move);
		nodes += perft(depth - 1);
		board.unmake(list->list[i].move, undoFlags);
	}
done:
	delete list;
	return nodes;
}

void show_help()
{
	cout << "GenChess v" << VERSION << " a Genesis Chess playing engine\n"
		<< "Copyright (C) 2011 Justin Madru under the GPLv3\n\n"
		<< "Usage: genchess [options]\n\n"
		<< "The following options are recognized:\n"
		<< "-i\t\tputs the engine in terminal interactive mode; board and prompt are printed\n"
		<< "\t\tif not supplied, engine is started in GUI mode; no prompts are displayed\n\n"
		<< "-w[c|h]\t\tselects type of white player: c=computer, h=human\n"
		<< "-b[c|h]\t\tselects type of black player\n"
		<< "-p #\t\truns the perft test to depth #\n"
		<< "-v\t\tdisplays version and copyright information\n"
		<< "-h\t\tprints the help screen for program and argument info\n\n";
}

int main(int argc, char **argv)
{
	int white = HUMAN, black = COMPUTER;
	char c;
	bool error = false, xMode = false, genesis = true;

	// set I/O to unbuffered
	cout.setf(ios::unitbuf);

	while ((c = getopt(argc, argv, "iw:b:hp:vgr")) != -1) {
		switch (c) {
		case 'i':
			xMode = true;
			break;
		case 'w':
			if (optarg[0] == 'c' || optarg[0] == 'C')
				white = COMPUTER;
			else if (optarg[0] == 'h' || optarg[0] == 'H')
				white = HUMAN;
			else
				error = true;
			break;
		case 'b':
			if (optarg[0] == 'c' || optarg[0] == 'C')
				black = COMPUTER;
			else if (optarg[0] == 'h' || optarg[0] == 'H')
				black = HUMAN;
			else
				error = true;
			break;
		case 'h':
			show_help();
			return EXIT_SUCCESS;
		case 'p':
			cout << perft(atoi(optarg)) << endl;
			return EXIT_SUCCESS;
		case 'g':
			genesis = true;
			break;
		case 'r':
			genesis = false;
			break;
		case 'v':
			cout << "GenChess v" << VERSION << " a Genesis Chess playing engine\n"
				<< "Copyright (C) 2011 Justin Madru under the GPLv3\n\n";
			return EXIT_SUCCESS;
		}
	}
	if (error) {
		cout << "invalid command line\n";
		return EXIT_FAILURE;
	}
	if (genesis) {
		if (xMode) {
			GenTerminal ui(white, black);
			ui.run();
		} else {
			GenCVEP ui;
			ui.run();
		}
	} else {
		if (xMode) {
			RegTerminal ui(white, black);
			ui.run();
		} else {
			RegCVEP ui;
			ui.run();
		}
	}


	return EXIT_SUCCESS;
}
