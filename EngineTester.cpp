#include <cmath>
#include <iostream>
#include <sstream>
#include "Defines.h"

using namespace std;

#define NSEC_PER_SEC 1000000000L

enum {SINGLE, DUAL};

struct IOptr {
	FILE *in;
	FILE *out;
};

struct GameResults {
	timespec whiteTime;
	timespec blackTime;
	int ply;
	char winner;

	GameResults()
	{
		ply = 0;
		whiteTime = {0, 0};
		blackTime = {0, 0};
	}
};

struct AsColor {
	timespec time;
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
		aswhite = {{0, 0}, 0, 0, 0, 0};
		asblack = {{0, 0}, 0, 0, 0, 0};
	}
};

EngineResults eng1res, eng2res;

IOptr engine1, engine2;

int numGames, testType;

string eng1Bin, eng2Bin;

char buff[256];

timespec operator-(const timespec l, const timespec r)
{
	timespec ret = l;

	if (l.tv_nsec < r.tv_nsec) {
		ret.tv_sec--;
		ret.tv_nsec += NSEC_PER_SEC;
	}
	ret.tv_sec = l.tv_sec - r.tv_sec;
	ret.tv_nsec = l.tv_nsec - r.tv_nsec;

	return ret;
}

timespec& operator+=(timespec &l, const timespec r)
{
	l.tv_nsec += r.tv_nsec;

	while (l.tv_nsec >= NSEC_PER_SEC) {
		l.tv_sec++;
		l.tv_nsec -= NSEC_PER_SEC;
	}
	l.tv_sec += r.tv_sec;

	return l;
}

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

double LOS(AsColor a)
{
	int num = a.win - a.lose, den = a.win + a.lose;
	double ratio = double(num) / double(den);

	return 100.0 * erf(ratio / sqrt(2.0)) / 2.0;
}

void printStats()
{
	AsColor w, x, y, z;

	w.win = eng1res.aswhite.win + eng1res.asblack.win;
	w.lose = eng1res.aswhite.lose + eng1res.asblack.lose;
	x.win = eng2res.aswhite.win + eng2res.asblack.win;
	x.lose = eng2res.aswhite.lose + eng2res.asblack.lose;
	y.win = eng1res.aswhite.win + eng2res.aswhite.win;
	y.lose = eng1res.aswhite.lose + eng2res.aswhite.lose;
	z.win = eng1res.asblack.win + eng2res.asblack.win;
	z.lose = eng1res.asblack.lose + eng2res.asblack.lose;

	double w1 = LOS(eng1res.aswhite), b1 = LOS(eng1res.asblack),
		w2 = LOS(eng2res.aswhite), b2 = LOS(eng2res.asblack),
		wb1 = LOS(w), wb2 = LOS(x), w12 = LOS(y), b12 = LOS(z),
		color = (w1 + w12 + w2) / 3.0, engine = (w1 + wb1 + b1) / 3.0;

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
	printf("one\t%+4.2f\t%+4.2f\t%+4.2f\n", w1, wb1, b1);
	printf("\t%+4.2f\t\t%+4.2f\n", w12, b12);
	printf("two\t%+4.2f\t%+4.2f\t%+4.2f\n\n", w2, wb2, b2);
	printf("Color: %+4.2f\tEngine: %+4.2f\n", color, engine);
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
		res.whiteTime = tm2 - tm1;

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
		res.blackTime = tm2 - tm1;

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
