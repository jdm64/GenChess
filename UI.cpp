#include "UI.h"

const string UI::moveError[NUM_MOVE_ERRORS] = {
	"", "That's an invalid move format",
	"That move doesn't reference a valid piece",
	"That is not your piece", "You must place your king first",
	"Can't place piece on another piece", "Can't capture your own piece",
	"That piece can't move like that", "That move places you in check",
	"Can't check by placing a piece"
};

UI::UI()
{
	again = true;

	game = new Game(&board);
	engine = new ComputerPlayer(&board);
	tt = new TransTable(32);

	board.rebuildHash();
}

UI::~UI()
{
	delete game;
	delete engine;
	delete tt;
}
