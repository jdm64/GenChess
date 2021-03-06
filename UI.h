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

#ifndef __UI_H__
#define __UI_H__

#include "Engine.h"
#include "Game.h"

class BaseUI
{
public:
	virtual void run() { }

	virtual uint64 perft(int depth) = 0;

	virtual ~BaseUI() { }
};

template<class MoveType>
class UI : public BaseUI
{
protected:
	static const string moveError[NUM_MOVE_ERRORS];

	Board<MoveType> board;
	
	Engine<MoveType> *engine;

	TransTable<MoveType> *tt;

	Game<MoveType> *game;

	bool again;

public:
	UI()
	{
		again = true;

		game = new Game<MoveType>(&board);
		tt = new TransTable<MoveType>(32);
		engine = new Engine<MoveType>(&board, tt);
	}

	~UI()
	{
		delete game;
		delete engine;
		delete tt;
	}

	/*              genesis   regular
	 * depth=1           64       20
	 * depth=2         3612      400
	 * depth=3       953632     8902
	 * depth=4    248188772   197281
	 * depth=5  64518625428  4865609
	 */
	uint64 perft(int depth)
	{
		if (depth == 0)
			return 1;

		uint64 nodes = 0;
		const MoveFlags undoFlags = board.getMoveFlags();
		auto *list = board.getMoveList(board.getStm(), MoveClass::ALL);

		for (auto& item : *list) {
			board.make(item.move);
			nodes += perft(depth - 1);
			board.unmake(item.move, undoFlags);
		}
		board.recycle(list);
		return nodes;
	}
};

template<class MoveType>
const string UI<MoveType>::moveError[NUM_MOVE_ERRORS] = {
	"", "That's an invalid move format",
	"That move doesn't reference a valid piece",
	"That is not your piece", "You must place your king first",
	"Can't place piece on another piece", "Can't capture your own piece",
	"That piece can't move like that", "That move places you in check",
	"Can't check by placing a piece"
};

#endif
