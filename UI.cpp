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

#include "UI.h"

const string GenUI::moveError[NUM_MOVE_ERRORS] = {
	"", "That's an invalid move format",
	"That move doesn't reference a valid piece",
	"That is not your piece", "You must place your king first",
	"Can't place piece on another piece", "Can't capture your own piece",
	"That piece can't move like that", "That move places you in check",
	"Can't check by placing a piece"
};

GenUI::GenUI()
{
	again = true;

	game = new GenGame(&board);
	engine = new GenEngine(&board);
	gtt = new GenTransTable(32);
}

GenUI::~GenUI()
{
	delete game;
	delete engine;
	delete gtt;
}

// --- Start Regular Chess ---

const string RegUI::moveError[NUM_MOVE_ERRORS] = {
	"", "That's an invalid move format",
	"That move doesn't reference a valid piece",
	"That is not your piece", "You must place your king first",
	"Can't place piece on another piece", "Can't capture your own piece",
	"That piece can't move like that", "That move places you in check",
	"Can't check by placing a piece"
};

RegUI::RegUI()
{
	again = true;

	game = new RegGame(&board);
	engine = new RegEngine(&board);
	rtt = new RegTransTable(32);
}

RegUI::~RegUI()
{
	delete game;
	delete engine;
	delete gtt;
}
