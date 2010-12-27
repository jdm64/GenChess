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

#ifndef __COMPUTER_PLAYER_H__
#define __COMPUTER_PLAYER_H__

#include <vector>
#include "Array.h"
#include "TransTable.h"

#define MAX_DEPTH		10
#define MIN_SCORE		-(INT_MAX - 4)
#define MAX_SCORE		(INT_MAX - 4)
#define CHECKMATE_SCORE		-(INT_MAX - 4)
#define STALEMATE_SCORE		0

class ComputerPlayer {
private:
	MoveList *curr;

	Board *board;

	Array<Move> captureKiller;
	Array<Move> moveKiller;
	Array<Move> placeKiller;
	Array<Move> pvMove;

	int maxNg;

	Array<bool> tactical;
	Array<bool> ismate;

	ScoreSort cmpScore;

public:
	ComputerPlayer(Board* Board) : curr(NULL), board(Board)
	{
		maxNg = 6;
	}

	~ComputerPlayer()
	{
	}

	int Quiescence(int alpha, int beta, int depth);

	bool NegaMoveType(int &alpha, const int beta, int &best, int depth, int limit, Array<Move> &killer, const int type);

	int NegaScout(int alpha, int beta, int depth, int limit);

	void search(int alpha, int beta, int depth, int limit);

	Move think();

	void debugTree();
};

#endif
