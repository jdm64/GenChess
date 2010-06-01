/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      main function and helpers
 */

#include <unistd.h>
#include "Game.h"

#define VERSION		"2.0.75"

using namespace std;

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
	bool white_ai = false, black_ai = false, error = false, xMode = false;
	char c;

	while ((c = getopt(argc, argv, "Xw:b:hv")) != -1) {
		switch (c) {
		case 'X':
			xMode = true;
			break;
		case 'w':
			if (optarg[0] == 'c' || optarg[0] == 'C')
				white_ai = true;
			else if (optarg[0] == 'h' || optarg[0] == 'H')
				white_ai = false;
			else
				error = true;
			break;
		case 'b':
			if (optarg[0] == 'c' || optarg[0] == 'C')
				black_ai = true;
			else if (optarg[0] == 'h' || optarg[0] == 'H')
				black_ai = false;
			else
				error = true;
			break;
		case 'h':
			show_help();
			return EXIT_SUCCESS;
		case 'v':
			cout << "GenChess v" << VERSION << " a Genesis Chess playing engine\n"
				<< "Copyright (C) 2010 Justin Madru under the GPLv3\n\n";
			return EXIT_SUCCESS;
		}
	}
	if (error) {
		cout << "invalid command line";
		return EXIT_FAILURE;
	}

	Game game(white_ai, black_ai, xMode);

	game.run();

	return EXIT_SUCCESS;
}
