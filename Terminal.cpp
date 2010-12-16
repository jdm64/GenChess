#include "Terminal.h"

const string Terminal::moveError[NUM_MOVE_ERRORS] = {
	"", "That's an invalid move format",
	"That move doesn't reference a valid piece",
	"That is not your piece", "You must place your king first",
	"Can't place piece on another piece", "Can't capture your own piece",
	"That piece can't move like that", "That move places you in check",
	"Can't check by placing a piece"
};


const string Terminal::PS[3] = {"black", "", "white"};

Terminal::Terminal(const int white, const int black)
{
	PT[0] = black;
	PT[2] = white;
	playerType = PT + 1;
	playerString = PS + 1;

	again = true;
	game = new Game(&board);
	engine = new ComputerPlayer(&board);
	tt = new TransTable(32);

	board.rebuildHash();
}

Terminal::~Terminal()
{
	delete game;
	delete engine;
	delete tt;
}

bool Terminal::mainMenu()
{
	string cmd;

	cout << "main> ";
	cin >> cmd;
	if (cmd == "new") {
		game->newGame();
	} else if (cmd == "quit") {
		again = false;
	} else if (cmd == "undo") {
		game->undoMove();
		// recheck player, print error if not undo-able
	} else if (cmd == "dundo") {
		game->undoMove();
		game->undoMove();
	} else if (cmd == "list") {
		board.printPieceList();
		return true;
	} else if (cmd == "help") {
		displayHelp();
		return true;
	} else {
		cout << "Error: \"" << cmd << "\" not recognized\n";
		return true;
	}
	return false;
}

bool Terminal::playerCmd()
{
	string cmd;

	cout << playerString[board.currPlayer()] << "> ";
	cin >> cmd;
	if (cmd == "move") {
		string mv;
		Move move;

		cin >> mv;
		int status = board.validMove(mv, board.currPlayer(), move);
		if (status != VALID_MOVE) {
			cout << moveError[status] << endl;
			return true;
		}
		game->doMove(move);
	} else if (cmd == "new") {
		game->newGame();
	} else if (cmd == "quit") {
		again = false;
	} else if (cmd == "undo") {
		game->undoMove();
		// recheck player, print error if not undo-able
	} else if (cmd == "dundo") {
		game->undoMove();
		game->undoMove();
	} else if (cmd == "list") {
		board.printPieceList();
		return true;
	} else if (cmd == "help") {
		displayHelp();
		return true;
	} else {
		cout << "Error: \"" << cmd << "\" not recognized\n";
		return true;
	}
	return false;
}

void Terminal::displayHelp()
{
	cout << "help messages comming soon...";
}

void Terminal::run()
{
	board.printBoard();

	while (again) {
		if (playerType[board.currPlayer()] == HUMAN) {
			while (playerCmd());
		} else {
			Move move = engine->think();
			game->doMove(move);
			cout << "My move is: " << move.toString() << endl;
		}
		if (again)
			board.printBoard();

		switch (board.isMate()) {
		case NOT_MATE:
		default:
			break;
		case CHECK_MATE:
			cout << playerString[board.currPlayer() ^ -2] << " got checkmate\n";
			while (mainMenu());
			break;
		case STALE_MATE:
			cout << "game is stalemate\n";
			while (mainMenu());
			break;
		}
	}
}
