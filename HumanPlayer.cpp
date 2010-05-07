#include "HumanPlayer.h"

using namespace std;

string HumanPlayer::errorMessage[NUM_ERRORS] = {
	"", "That's an invalid move format",
	"That move doesn't reference a valid piece",
	"That is not your piece", "You must place your king first",
	"Can't place piece on another piece", "Can't capture your own piece",
	"That piece can't move like that", "That move places you in check",
	"Can't check by placing a piece"
};

int HumanPlayer::parseCommand(string cmd)
{
	if (cmd == "quit") {
		board->quitGame();
		return IS_COMMAND;
	} else if (cmd == "list") {
		board->printPieceList();
		return IS_COMMAND;
	} else if (cmd == "new") {
		board->newGame();
		return IS_COMMAND;
	} else if (cmd == "help") {
		cout << "list\tshows where each piece is located\n"
			<< "new\tstarts a new game\n"
			<< "quit\texits the game\n"
			<< "help\tdisplays this help message\n";
		return IS_COMMAND;
	}
	return NOT_COMMAND;
}

int HumanPlayer::parseMove(string s)
{
	int piece;
	Move move;
	bool place = true;

	// determin move type by first char
	switch(s[0]) {
	case 'a':	case 'b':
	case 'c':	case 'd':
	case 'e':	case 'f':
	case 'g':	case 'h':
		place = false;
		break;
	case 'P':
		piece = PAWN;
		break;
	case 'N':
		piece = KNIGHT;
		break;
	case 'B':
		piece = BISHOP;
		break;
	case 'R':
		piece = ROOK;
		break;
	case 'Q':
		piece = QUEEN;
		break;
	case 'K':
		piece = KING;
		break;
	default:
		return NOT_MOVE;
	}
	// parse placement move
	if (place) {
		if (s[1] < 'a' || s[1] > 'h' || s[2] < '0' || s[2] > '9')
			return INVALID_FORMAT;
		move.to = s[1] - 'a';
		move.to += 8 * (8 - (s[2] - '0'));
		move.index = board->pieceIndex(PLACEABLE, color * piece);
	} else {
	// parse movement move
		if (s[0] < 'a' || s[0] > 'h' ||
				s[1] < '0' || s[1] > '9' ||
				s[2] < 'a' || s[2] > 'h' ||
				s[3] < '0' || s[3] > '9')
			return INVALID_FORMAT;
		move.from = s[0] - 'a';
		move.from += 8 * (8 - (s[1] - '0'));
		move.to = s[2] - 'a';
		move.to += 8 * (8 - (s[3] - '0'));
		move.index = board->pieceIndex(move.from);
	}
	// are we talking about a valid piece?
	if (move.index == NONE)
		return NOPIECE_ERROR;
	move.xindex = board->pieceIndex(move.to);
	// return the code for what happends when trying to do the move
	return board->doMove(move, color);
}

void HumanPlayer::think()
{
	int tmp;
	string comd;

	if (!xMode)
		board->printBoard();

	while (true) {
		if (!xMode)
			cout << color_str << "> ";
		cin >> comd;

		switch (tmp = parseMove(comd)) {
		case VALID_MOVE:
			cout << ".\n";
			return;
		case NOPIECE_ERROR:
		case DONT_OWN:
		case KING_FIRST:
		case NON_EMPTY_PLACE:
		case CAPTURE_OWN:
		case INVALID_MOVEMENT:
		case IN_CHECK:
		case IN_CHECK_PLACE:
			cout << "? " << errorMessage[tmp] << endl;
			continue;
		case INVALID_FORMAT:
			break;
		}
		if ((tmp = parseCommand(comd)) == NOT_COMMAND)
			cout << "? That's a valid comman\n";
		return;
	}
}
