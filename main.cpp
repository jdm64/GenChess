#include <iostream>
#include "Terminal.h"
#include "CVEP.h"

Board board;

/* depth=1	nodes=64
 * depth=2	nodes=3612
 * depth=3	nodes=953632
 * depth=4	nodes=248188772
 * depth=5	nodes=64518625428
 */
uint64 perft(int depth)
{
	MoveList *list;
	uint64 nodes;

	if (depth == 1) {
		list = board.getPerftMoveList(board.currPlayer(), MOVE_ALL);
		nodes = list->size;
		delete list;
		return nodes;
	}
	nodes = 0;

	// Placement Moves
	list = board.getPerftMoveList(board.currPlayer(), MOVE_PLACE);
	if (list->size == 0)
		goto moves;
	for (int i = 0; i < list->size; i++) {
		board.makeP(list->list[i].move);
		nodes += perft(depth - 1);
		board.unmakeP(list->list[i].move);
	}
moves:
	delete list;
	// Movement Moves
	list = board.getPerftMoveList(board.currPlayer(), MOVE_MOVE);
	if (list->size == 0)
		goto captures;
	for (int i = 0; i < list->size; i++) {
		board.makeP(list->list[i].move);
		nodes += perft(depth - 1);
		board.unmakeP(list->list[i].move);
	}
captures:
	delete list;
	// Capture Moves
	list = board.getPerftMoveList(board.currPlayer(), MOVE_CAPTURE);
	if (list->size == 0)
		goto done;
	for (int i = 0; i < list->size; i++) {
		board.makeP(list->list[i].move);
		nodes += perft(depth - 1);
		board.unmakeP(list->list[i].move);
	}
done:
	delete list;
	return nodes;
}

void show_help()
{
	cout << "GenChess v" << VERSION << " a Genesis Chess playing engine\n"
		<< "Copyright (C) 2010 Justin Madru under the GPLv3\n\n"
		<< "Usage: genchess [options]\n\n"
		<< "The following options are recognized:\n"
		<< "-X\t\tputs the engine in GUI mode, the board and prompt are not printed\n"
		<< "-w[c|h]\t\tselects type of white player: c=computer, h=human\n"
		<< "-b[c|h]\t\tselects type of black player\n"
		<< "-h\t\tprints the help screen for program and argument info\n\n";
}

int main(int argc, char **argv)
{
	int white = HUMAN, black = COMPUTER;
	char c;
	bool error = false, xMode = false;

	// set I/O to unbuffered
	cout.setf(ios::unitbuf);

	while ((c = getopt(argc, argv, "Xw:b:hp:v")) != -1) {
		switch (c) {
		case 'X':
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
		case 'v':
			cout << "GenChess v" << VERSION << " a Genesis Chess playing engine\n"
				<< "Copyright (C) 2010 Justin Madru under the GPLv3\n\n";
			return EXIT_SUCCESS;
		}
	}
	if (error) {
		cout << "invalid command line\n";
		return EXIT_FAILURE;
	}
	if (xMode) {
		Terminal ui(white, black);
		ui.run();
	} else {
		CVEP ui;
		ui.run();
	}

	return EXIT_SUCCESS;
}
