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

#ifndef __UI_H__
#define __UI_H__

#include "Engine.h"
#include "Game.h"

template<class MoveType>
class UI
{
protected:
	static const string moveError[NUM_MOVE_ERRORS];

	Board<MoveType> board;
	
	Engine<MoveType> *engine;

	TransTable<MoveType> *tt;

	typename Game<MoveType>::type *game;

	bool again;

public:
	UI()
	{
		again = true;

		game = new typename Game<MoveType>::type(&board);
		tt = new TransTable<MoveType>(32);
		engine = new Engine<MoveType>(&board, tt);
	}

	virtual ~UI()
	{
		delete game;
		delete engine;
		delete tt;
	}

	virtual void run() = 0;
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
