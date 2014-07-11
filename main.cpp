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

#include <memory>
#include <unistd.h>
#include "Terminal.h"
#include "CVEP.h"

void show_help()
{
	cout << "GenChess " << VERSION << " a Genesis Chess playing engine\n"
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
	enum Mode { PROTOCAL, TERMINAL, PERFT };
	int white = HUMAN, black = COMPUTER, depth = 3;
	char c;
	Mode mode = PROTOCAL;
	bool error = false, genesis = true;

	// set I/O to unbuffered
	cout.setf(ios::unitbuf);

	while ((c = getopt(argc, argv, "iw:b:hp:vgr")) != -1) {
		switch (c) {
		case 'i':
			mode = TERMINAL;
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
			mode = PERFT;
			depth = atoi(optarg);
			break;
		case 'g':
			genesis = true;
			break;
		case 'r':
			genesis = false;
			break;
		case 'v':
			cout << "GenChess " << VERSION << " a Genesis Chess playing engine\n"
				<< "Copyright (C) 2011 Justin Madru under the GPLv3\n\n";
			return EXIT_SUCCESS;
		}
	}
	if (error) {
		cout << "invalid command line\n";
		return EXIT_FAILURE;
	}

	unique_ptr<BaseUI> ui;
	switch (mode) {
	case PROTOCAL:
		if (genesis)
			ui = unique_ptr<BaseUI>(new GenCVEP);
		else
			ui = unique_ptr<BaseUI>(new RegCVEP);
		break;
	case TERMINAL:
		if (genesis)
			ui = unique_ptr<BaseUI>(new GenTerminal(white, black));
		else
			ui = unique_ptr<BaseUI>(new RegTerminal(white, black));
		break;
	case PERFT:
		if (genesis)
			ui = unique_ptr<BaseUI>(new UI<GenMove>());
		else
			ui = unique_ptr<BaseUI>(new UI<RegMove>());
		cout << ui->perft(depth) << endl;
		return EXIT_SUCCESS;
	}

	ui->run();
	return EXIT_SUCCESS;
}
