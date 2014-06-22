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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <iostream>
#include <climits>
#include <vector>
#include "Array.h"
#include "Util.h"
#include "TransTable.h"

#define MAX_DEPTH		10
#define MIN_SCORE		-(INT_MAX - 4)
#define MAX_SCORE		(INT_MAX - 4)
#define CHECKMATE_SCORE		-(INT_MAX - 4)
#define STALEMATE_SCORE		0

template<class MoveType>
class Engine
{
private:
	MoveList<MoveType> *curr;

	Board<MoveType> *board;
	TransTable<MoveType> *tt;

	Array<MoveType> captureKiller;
	Array<MoveType> moveKiller;
	Array<MoveType> placeKiller;
	Array<MoveType> pvMove;

	int maxNg;

	Array<bool> tactical;
	Array<bool> ismate;

	ScoreSort<MoveType> cmpScore;

	void pickRandomMove();

	int Quiescence(int alpha, int beta, int depth);

	bool NegaMoveType(int &alpha, const int beta, int &best, const int depth, const int limit, Array<MoveType> &killer, const MoveClass type);

	int NegaScout(int alpha, int beta, int depth, int limit);

	void search(int alpha, int beta, int depth, int limit);

public:
	Engine(Board<MoveType> *_Board, TransTable<MoveType> *TT) : curr(NULL), board(_Board), tt(TT)
	{
		maxNg = 5;
	}

	~Engine()
	{
	}

	MoveType think()
	{
		timeval t1, t2;

		srand(time(NULL));
		curr = NULL;

		gettimeofday(&t1, NULL);
		for (int depth = 1; depth <= maxNg; depth++) {
			search(MIN_SCORE, MAX_SCORE, 0, depth);
			gettimeofday(&t2, NULL);
			cout << "stats depth " << depth << " time " << time_in_msec(t2 - t1) << endl;
		}

#ifdef PRINT_HASH_STATS
		sixInt st = tt->stats();
		cout << "stats total=" << st.one + st.two << " hits=" << 100 * st.one / double(st.one + st.two)
			<< "% scorehits=" << 100 * st.three / double(st.three + st.four)
			<< "% movehits=" << 100 * st.five / double(st.five + st.six) << "%" << endl;
		tt->clearStats();
#endif

		// Randomize opening
		if (board->getPly() < 7)
			pickRandomMove();

		delete curr;
		return pvMove[0];
	}
};

typedef Engine<GenMove> GenEngine;
typedef Engine<RegMove> RegEngine;

#endif
