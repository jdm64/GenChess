#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include "Defines.h"

using namespace std;

enum {SINGLE, DUAL};

struct IOptr {
	FILE *in;
	FILE *out;
};

struct GameResults {
	uint64 whiteTime;
	uint64 blackTime;
	int ply;
	char winner;

	GameResults()
	{
		ply = 0;
		whiteTime = 0;
		blackTime = 0;
	}
};

struct AsColor {
	uint64 time;
	int win;
	int lose;
	int draw;
	int ply;
};

struct EngineResults {
	AsColor aswhite;
	AsColor asblack;

	EngineResults()
	{
		aswhite = {0, 0, 0, 0, 0};
		asblack = {0, 0, 0, 0, 0};
	}
};

EngineResults eng1res, eng2res;

IOptr engine1, engine2;

int numGames, testType;

string eng1Bin, eng2Bin;

char buff[256];

IOptr connectIO(string program)
{
	enum {IN, OUT};

	int pipe1[2], pipe2[2];

	pipe(pipe1);
	pipe(pipe2);

	if (!fork()) {
		close(IN);
		dup(pipe1[IN]);
		close(OUT);
		dup(pipe2[OUT]);
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

double playerError(AsColor a, AsColor b)
{
	int num = 2 * (2 * (a.win - a.lose) + (b.win - b.lose)),
		den = a.win + a.lose + b.win + b.lose;
	double ratio = double(abs(num)) / double(den);

	return 100.0 * erf(ratio / sqrt(2.0)) / 2.0;
}

void printStats()
{
	cout << "Engine One:\n\tAsWhite:\n\t\t"
		<< "win:" << eng1res.aswhite.win
		<< " loss:" << eng1res.aswhite.lose
		<< " draw:" << eng1res.aswhite.draw
		<< " ply:" << eng1res.aswhite.ply
		<< " time:" << eng1res.aswhite.time
		<< "\n\tAsBlack:\n\t\t"
		<< "win:" << eng1res.asblack.win
		<< " loss:" << eng1res.asblack.lose
		<< " draw:" << eng1res.asblack.draw
		<< " ply:" << eng1res.asblack.ply
		<< " time:" << eng1res.asblack.time
		<< "\nEngine Two:\n\tAsWhite:\n\t\t"
		<< "win:" << eng2res.aswhite.win
		<< " loss:" << eng2res.aswhite.lose
		<< " draw:" << eng2res.aswhite.draw
		<< " ply:" << eng2res.aswhite.ply
		<< " time:" << eng2res.aswhite.time
		<< "\n\tAsBlack:\n\t\t"
		<< "win:" << eng2res.asblack.win
		<< " loss:" << eng2res.asblack.lose
		<< " draw:" << eng2res.asblack.draw
		<< " ply:" << eng2res.asblack.ply
		<< " time:" << eng2res.asblack.time
		<< endl;

	cout << "1W-1B = " << playerError(eng1res.aswhite, eng1res.asblack) << endl
		<< "2W-2B = " << playerError(eng2res.aswhite, eng2res.asblack) << endl
		<< "1W-2W = " << playerError(eng1res.aswhite, eng2res.aswhite) << endl
		<< "1B-2B = " << playerError(eng1res.asblack, eng2res.asblack) << endl
		<< "1W-2B = " << playerError(eng1res.aswhite, eng2res.asblack) << endl
		<< "1B-2W = " << playerError(eng1res.asblack, eng2res.aswhite) << endl;
}

GameResults runGame(IOptr white, IOptr black)
{
	istringstream line;
	string move, word;
	GameResults res;
	timespec tm1, tm2;

	fputs("newgame\n", white.in);
	fputs("newgame\n", black.in);

	while (true) {
		// get white's move
		res.ply++;
		fputs("go\n", white.in);
		clock_gettime(CLOCK_REALTIME, &tm1);
		fgets(buff, 256, white.out);
		clock_gettime(CLOCK_REALTIME, &tm2);
		res.whiteTime = tm2.tv_nsec - tm1.tv_nsec;

		move = string(buff);
		fgets(buff, 256, white.out);

		line.str(string(buff));
		line >> word;

		// ok|special are discarded
		if (word == "result")
			break;

		// send white's move to black
		fputs(move.c_str(), black.in);
		fgets(buff, 256, black.out);

		// get black's move
		res.ply++;
		fputs("go\n", black.in);
		clock_gettime(CLOCK_REALTIME, &tm1);
		fgets(buff, 256, black.out);
		clock_gettime(CLOCK_REALTIME, &tm2);
		res.blackTime = tm2.tv_nsec - tm1.tv_nsec;

		if (res.blackTime <= 0) {
			cout << tm2.tv_nsec << " " << tm1.tv_nsec << endl;
			assert(0);
		}

		move = string(buff);
		fgets(buff, 256, black.out);

		line.str(string(buff));
		line >> word;

		// ok|special are discarded
		if (word == "result")
			break;

		// send black's move to white
		fputs(move.c_str(), white.in);
		fgets(buff, 256, white.out);
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

		eng1res.aswhite.time += results.whiteTime;
		eng1res.aswhite.ply += results.ply;

		eng2res.asblack.time += results.blackTime;
		eng2res.asblack.ply += results.ply;

		switch (results.winner) {
		case 'W':
			eng1res.aswhite.win++;
			eng2res.asblack.lose++;
			break;
		case 'B':
			eng2res.asblack.win++;
			eng1res.aswhite.lose++;
			break;
		case 'D':
			eng1res.aswhite.draw++;
			eng2res.asblack.draw++;
			break;
		}
		// One=black; Two=white
		results = runGame(engine2, engine1);

		eng2res.aswhite.time += results.whiteTime;
		eng2res.aswhite.ply += results.ply;

		eng1res.asblack.time += results.blackTime;
		eng1res.asblack.ply += results.ply;

		assert(eng1res.asblack.time > 0);
		switch (results.winner) {
		case 'W':
			eng2res.aswhite.win++;
			eng1res.asblack.lose++;
			break;
		case 'B':
			eng1res.asblack.win++;
			eng2res.aswhite.lose++;
			break;
		case 'D':
			eng2res.aswhite.draw++;
			eng1res.asblack.draw++;
			break;
		}
	}
	fputs("quit\n", engine1.in);
	fputs("quit\n", engine2.in);
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
