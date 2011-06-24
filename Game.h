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

#ifndef __GAME_H__
#define __GAME_H__

#include <vector>
#include "Board.h"

class GenGame {
private:
	GenBoard *board;

	vector<GenMove> history;

public:
	GenGame(GenBoard *Board) : board(Board) {}

	void newGame();

	void doMove(const GenMove &move);

	bool undoMove();
};

// --- Start Regular Chess ---

class RegGame {
private:
	RegBoard *board;

	vector<HistoryNode> history;

public:
	RegGame(RegBoard *Board) : board(Board) {}

	void newGame();

	void doMove(const RegMove &move);

	bool undoMove();
};

#endif
