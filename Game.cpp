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

#include "Game.h"

void GenGame::newGame()
{
	board->reset();
	history.clear();
}

void GenGame::doMove(const GenMove &move)
{
	board->make(move);
	history.push_back(move);
}

bool GenGame::undoMove()
{
	if (!history.size())
		return false;
	board->unmake(history.back());
        history.pop_back();

        return true;
}

// --- Start Regular Chess ---

void RegGame::newGame()
{
	board->reset();
	history.clear();
}

void RegGame::doMove(const RegMove &move)
{
	const HistoryNode item = {move, board->getMoveFlags()};

	board->make(move);
	history.push_back(item);
}

bool RegGame::undoMove()
{
	if (!history.size())
		return false;
	HistoryNode node = history.back();
	board->unmake(node.move, node.flags);
        history.pop_back();

        return true;
}
