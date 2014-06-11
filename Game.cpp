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

#include "Game.h"

template<>
void Game<GenMove>::newGame()
{
	board->reset();
	history.clear();
}

template<>
void Game<GenMove>::doMove(const GenMove &move)
{
	const HistoryNode<GenMove> item = {move};

	board->make(move);
	history.push_back(item);
}

template<>
bool Game<GenMove>::undoMove()
{
	if (history.empty())
		return false;

	HistoryNode<GenMove> node = history.back();
	board->unmake(node.move);
	history.pop_back();

	return true;
}

// --- Start Regular Chess ---

template<>
void Game<RegMove>::newGame()
{
	board->reset();
	history.clear();
}

template<>
void Game<RegMove>::doMove(const RegMove &move)
{
	const HistoryNode<RegMove> item = {move, board->getMoveFlags()};

	board->make(move);
	history.push_back(item);
}

template<>
bool Game<RegMove>::undoMove()
{
	if (history.empty())
		return false;

	HistoryNode<RegMove> node = history.back();
	board->unmake(node.move, node.flags);
	history.pop_back();

	return true;
}
