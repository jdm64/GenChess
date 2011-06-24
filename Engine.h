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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <vector>
#include "Array.h"
#include "TransTable.h"

#define MAX_DEPTH		10
#define MIN_SCORE		-(INT_MAX - 4)
#define MAX_SCORE		(INT_MAX - 4)
#define CHECKMATE_SCORE		-(INT_MAX - 4)
#define STALEMATE_SCORE		0

class GenEngine {
private:
	GenMoveList *curr;

	GenBoard *board;

	Array<GenMove> captureKiller;
	Array<GenMove> moveKiller;
	Array<GenMove> placeKiller;
	Array<GenMove> pvMove;

	int maxNg;

	Array<bool> tactical;
	Array<bool> ismate;

	GenScoreSort cmpScore;

public:
	GenEngine(GenBoard* GenBoard) : curr(NULL), board(GenBoard)
	{
		maxNg = 5;
	}

	~GenEngine()
	{
	}

	void pickRandomMove();

	int Quiescence(int alpha, int beta, int depth);

	bool NegaMoveType(int &alpha, const int beta, int &best, int depth, int limit, Array<GenMove> &killer, const int type);

	int NegaScout(int alpha, int beta, int depth, int limit);

	void search(int alpha, int beta, int depth, int limit);

	GenMove think();

	void debugTree();
};

// --- Start Regular Chess ---

class RegEngine {
private:
	RegMoveList *curr;

	RegBoard *board;

	Array<RegMove> captureKiller;
	Array<RegMove> moveKiller;
	Array<RegMove> pvMove;

	int maxNg;

	Array<bool> tactical;
	Array<bool> ismate;

	RegScoreSort cmpScore;

public:
	RegEngine(RegBoard* Board) : curr(NULL), board(Board)
	{
		maxNg = 7;
	}

	~RegEngine()
	{
	}

	void pickRandomMove();

	int Quiescence(int alpha, int beta, int depth);

	bool NegaMoveType(int &alpha, const int beta, int &best, int depth, int limit, Array<RegMove> &killer, const int type);

	int NegaScout(int alpha, int beta, int depth, int limit);

	void search(int alpha, int beta, int depth, int limit);

	RegMove think();

	void debugTree();
};

#endif
