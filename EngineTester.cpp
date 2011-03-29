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

#include <cmath>
#include <iostream>
#include <sstream>
#include "Defines.h"
#include "Util.h"

using namespace std;

enum {ENG1WIN, ENG2WIN, ENGDRAW};

struct IOptr {
	FILE *in;
	FILE *out;
};

struct GameResults {
	timeval whiteTime;
	timeval blackTime;
	int ply;
	char winner;

	GameResults()
	{
		whiteTime = {0, 0};
		blackTime = {0, 0};
		ply = 0;
		winner = 'D';
	}
};

struct AsColor {
	timeval time;
	int win;
	int lose;
	int draw;
	int ply;

	AsColor operator+(const AsColor &b) const
	{
		AsColor r;

		// r.time = time + b.time;
		r.win = win + b.win;
		r.lose = lose + b.lose;
		r.draw = draw + b.draw;
		r.ply = ply + b.ply;

		return r;
	}
};

struct EngineResults {
	AsColor aswhite;
	AsColor asblack;

	EngineResults()
	{
		aswhite = {{0, 0}, 0, 0, 0, 0};
		asblack = {{0, 0}, 0, 0, 0, 0};
	}
};

char buff[256];

int numGames, elo1 = 0, elo2 = 0;

string eng1Bin, eng2Bin;

IOptr engine1, engine2;

EngineResults eng1res, eng2res;

double WinRatio(const AsColor &a)
{
	// (win + draw / 2) / (win + lose + draw)
	double win = a.win, draw = a.draw, lose = a.lose;

	return 100.0 * ((win + draw / 2) / (win + draw + lose));
}

void UpdateElo(const int results)
{
	double score1 = 0, score2 = 0, exp;

	switch (results) {
	case ENG1WIN:
		score1 = 1;
		break;
	case ENG2WIN:
		score2 = 1;
		break;
	case ENGDRAW:
		score1 = 0.5;
		score2 = 0.5;
		break;
	}
	// Engine 1 Elo
	exp = 1.0 / (pow(2.0, (elo2 - elo1) / 128.0) + 1.0);
	elo1 += round(32.0 * (score1 - exp));
	// Engine 2 Elo
	exp = 1.0 / (pow(2.0, (elo1 - elo2) / 128.0) + 1.0);
	elo2 += round(32.0 * (score2 - exp));
}

void printStats()
{
	double w1 = WinRatio(eng1res.aswhite), b1 = WinRatio(eng1res.asblack),
		w2 = WinRatio(eng2res.aswhite), b2 = WinRatio(eng2res.asblack),
		w = WinRatio(eng1res.aswhite + eng2res.aswhite),
		b = WinRatio(eng1res.asblack + eng2res.asblack),
		e1 = WinRatio(eng1res.aswhite + eng1res.asblack),
		e2 = WinRatio(eng2res.aswhite + eng2res.asblack);

	cout.setf(ios::showpoint);

	cout << "Engine 1: " << eng1Bin << endl
		<< "White: win:" << eng1res.aswhite.win
			<< " loss:" << eng1res.aswhite.lose
			<< " draw:" << eng1res.aswhite.draw
			<< " ply:" << eng1res.aswhite.ply
			<< " time:" << eng1res.aswhite.time.tv_sec
			<< endl
		<< "Black: win:" << eng1res.asblack.win
			<< " loss:" << eng1res.asblack.lose
			<< " draw:" << eng1res.asblack.draw
			<< " ply:" << eng1res.asblack.ply
			<< " time:" << eng1res.asblack.time.tv_sec
			<< "\n\n"

		<< "Engine 2: " << eng2Bin << endl
		<< "White: win:" << eng2res.aswhite.win
			<< " loss:" << eng2res.aswhite.lose
			<< " draw:" << eng2res.aswhite.draw
			<< " ply:" << eng2res.aswhite.ply
			<< " time:" << eng2res.aswhite.time.tv_sec
			<< endl
		<< "Black: win:" << eng2res.asblack.win
			<< " loss:" << eng2res.asblack.lose
			<< " draw:" << eng2res.asblack.draw
			<< " ply:" << eng2res.asblack.ply
			<< " time:" << eng2res.asblack.time.tv_sec
			<< "\n\n";

	printf("Stats:\n");
	printf("\twhite\t\tblack\n");
	printf("one\t%4.2f\t%4.2f\t%4.2f\n", w1, e1, b1);
	printf("\t%4.2f\t\t%4.2f\n", w, b);
	printf("two\t%4.2f\t%4.2f\t%4.2f\n\n", w2, e2, b2);
	printf("Engine 1 Elo:\t%i\n", elo1);
	printf("Engine 2 Elo:\t%i\n", elo2);
	printf("Elo Diff:\t%i\n", abs(elo1 - elo2));
}

GameResults runGame(const IOptr &white, const IOptr &black)
{
	istringstream line;
	string move, word;
	GameResults res;
	timeval tm1, tm2;

	fputs("newgame\n", white.in);
	fputs("newgame\n", black.in);

	while (true) {
		// get white's move
		res.ply++;
		fputs("go\n", white.in);
		gettimeofday(&tm1, NULL);

		// loop over stat output
		while (true) {
			if (!fgets(buff, 256, white.out)) {
				cout << "Error: broken pipe\n";
				exit(EXIT_FAILURE);
			}
			line.str(string(buff));
			line >> word;

			if (word == "stats")
				continue;
			if (word == "move")
				break;
		}
		gettimeofday(&tm2, NULL);
		sum(res.whiteTime, tm2 - tm1);

		move = line.str();

		if (!fgets(buff, 256, white.out)) {
			cout << "Error: broken pipe\n";
			exit(EXIT_FAILURE);
		}
		line.str(string(buff));
		line >> word;

		// game is over on result
		if (word == "result")
			break;

		// send white's move to black
		fputs(move.c_str(), black.in);
		if (!fgets(buff, 256, black.out)) {
			cout << "Error: broken pipe\n";
			exit(EXIT_FAILURE);
		}

		// get black's move
		res.ply++;
		fputs("go\n", black.in);
		gettimeofday(&tm1, NULL);

		// loop over stat output
		while (true) {
			if (!fgets(buff, 256, black.out)) {
				cout << "Error: broken pipe\n";
				exit(EXIT_FAILURE);
			}
			line.str(string(buff));
			line >> word;

			if (word == "stats")
				continue;
			if (word == "move")
				break;
		}
		gettimeofday(&tm2, NULL);
		sum(res.blackTime, tm2 - tm1);

		move = line.str();

		if (!fgets(buff, 256, black.out)) {
			cout << "Error: broken pipe\n";
			exit(EXIT_FAILURE);
		}
		line.str(string(buff));
		line >> word;

		// game is over on result
		if (word == "result")
			break;

		// send black's move to white
		fputs(move.c_str(), white.in);
		if (!fgets(buff, 256, white.out)) {
			cout << "Error: broken pipe\n";
			exit(EXIT_FAILURE);
		}
	}
	// results of game
	line >> word;

	res.winner = (word[0] == 'N')? 'D' : ((word[0] == '1')? 'W' : 'B');

	return res;
}

void runMatch()
{
	GameResults results;

	for (int i = 0; i < numGames / 2; i++) {
		// One=white; Two=black
		results = runGame(engine1, engine2);

		eng1res.aswhite.ply += results.ply;
		eng2res.asblack.ply += results.ply;

		sum(eng1res.aswhite.time, results.whiteTime);
		sum(eng2res.asblack.time, results.blackTime);

		switch (results.winner) {
		case 'W':
			eng1res.aswhite.win++;
			eng2res.asblack.lose++;
			UpdateElo(ENG1WIN);
			break;
		case 'B':
			eng2res.asblack.win++;
			eng1res.aswhite.lose++;
			UpdateElo(ENG2WIN);
			break;
		case 'D':
			eng1res.aswhite.draw++;
			eng2res.asblack.draw++;
			UpdateElo(ENGDRAW);
			break;
		}
		// One=black; Two=white
		results = runGame(engine2, engine1);

		eng2res.aswhite.ply += results.ply;
		eng1res.asblack.ply += results.ply;

		sum(eng2res.aswhite.time, results.whiteTime);
		sum(eng1res.asblack.time, results.blackTime);

		switch (results.winner) {
		case 'W':
			eng2res.aswhite.win++;
			eng1res.asblack.lose++;
			UpdateElo(ENG2WIN);
			break;
		case 'B':
			eng1res.asblack.win++;
			eng2res.aswhite.lose++;
			UpdateElo(ENG1WIN);
			break;
		case 'D':
			eng2res.aswhite.draw++;
			eng1res.asblack.draw++;
			UpdateElo(ENGDRAW);
			break;
		}
	}
	fputs("quit\n", engine1.in);
	fputs("quit\n", engine2.in);
}

IOptr connectIO(const string program)
{
	enum {IN, OUT};

	int pipe1[2], pipe2[2];

	if (pipe(pipe1)) {
		cout << "Error: can't create pipe to: " << program << endl;
		exit(EXIT_FAILURE);
	}
	if (pipe(pipe2)) {
		cout << "Error: can't create pipe to: " << program << endl;
		exit(EXIT_FAILURE);
	}

	if (!fork()) {
		close(IN);
		if (dup(pipe1[IN]) == -1) {
			cout << "Error: can't duplicate file descriptor\n";
			exit(EXIT_FAILURE);
		}
		close(OUT);
		if (dup(pipe2[OUT]) == -1) {
			cout << "Error: can't duplicate file descriptor\n";
			exit(EXIT_FAILURE);
		}
		close(pipe1[OUT]);
		close(pipe2[IN]);
		execlp(program.c_str(), program.c_str(), NULL);
		// program ends here
	}
	IOptr ret;

	close(pipe1[IN]);
	close(pipe2[OUT]);
	ret.in = fdopen(pipe1[OUT], "w");
	ret.out = fdopen(pipe2[IN], "r");

	// set unbuffered I/O
	setbuf(ret.in, NULL);

	return ret;
}

void initEngines()
{
	engine1 = connectIO(eng1Bin);
	engine2 = connectIO(eng2Bin);
}

int main(int argc, char **argv)
{
	char c;

	while ((c = getopt(argc, argv, "n:")) != -1) {
		switch (c) {
		case 'n':
			numGames = atoi(optarg);
			break;
		}
	}
	
	eng1Bin = argv[optind++];
	eng2Bin = argv[optind];

	initEngines();
	runMatch();
	printStats();

	return 0;
}
