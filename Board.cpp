/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Board class functions
 */

#include "Board.h"

using namespace std;

const int Board::pieceValues[16] = {11, 11, 11, 11, 11, 11, 11, 11, 34, 34, 56, 56, 90, 90, 146, 4752};

const char Board::pieceSymbol[7] = {' ', 'P', 'N', 'B', 'R', 'Q', 'K'};

Board::Board()
{
	newGame();
}

char Board::currentPlayer()
{
	return curr;
}

void Board::quitGame()
{
	curr = QUIT_GAME;
}

void Board::newGame()
{
	pieces = {
		{PLACEABLE, BLACK_PAWN},	{PLACEABLE, BLACK_PAWN},
		{PLACEABLE, BLACK_PAWN},	{PLACEABLE, BLACK_PAWN},
		{PLACEABLE, BLACK_PAWN},	{PLACEABLE, BLACK_PAWN},
		{PLACEABLE, BLACK_PAWN},	{PLACEABLE, BLACK_PAWN},
		{PLACEABLE, BLACK_KNIGHT},	{PLACEABLE, BLACK_KNIGHT},
		{PLACEABLE, BLACK_BISHOP},	{PLACEABLE, BLACK_BISHOP},
		{PLACEABLE, BLACK_ROOK},	{PLACEABLE, BLACK_ROOK},
		{PLACEABLE, BLACK_QUEEN},	{PLACEABLE, BLACK_KING},

		{PLACEABLE, WHITE_PAWN},	{PLACEABLE, WHITE_PAWN},
		{PLACEABLE, WHITE_PAWN},	{PLACEABLE, WHITE_PAWN},
		{PLACEABLE, WHITE_PAWN},	{PLACEABLE, WHITE_PAWN},
		{PLACEABLE, WHITE_PAWN},	{PLACEABLE, WHITE_PAWN},
		{PLACEABLE, WHITE_KNIGHT},	{PLACEABLE, WHITE_KNIGHT},
		{PLACEABLE, WHITE_BISHOP},	{PLACEABLE, WHITE_BISHOP},
		{PLACEABLE, WHITE_ROOK},	{PLACEABLE, WHITE_ROOK},
		{PLACEABLE, WHITE_QUEEN},	{PLACEABLE, WHITE_KING},
	};
	board = {0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0};
	curr = WHITE;
	hcounter = 0;
	MoveLookup::setBoard(board);
	history.clear();
}

int Board::pieceIndex(int loc)
{
	for (int i = 0; i < 32; i++)
		if (pieces[i].loc == loc)
			return i;
	return NONE;
}

int Board::pieceIndex(int loc, int type)
{
	static int offset[] = {-1, 0, 8, 10, 12, 14, 15, 16};
	int start = ((type < 0)? 0 : 16) + offset[abs(type)],
		end = ((type < 0)? 0 : 16) + offset[abs(type) + 1];

	for (int i = start; i < end; i++)
		if (pieces[i].loc == loc)
			return i;
	return NONE;
}

bool Board::inCheck(char color)
{
	MoveLookup ml;
	int king = (color == WHITE)? 31:15;

	return ml.setLoc(pieces[king].loc)? ml.isAttacked() : false;
}

void Board::doMove(Move move)
{
	// update board information
	board[move.to] = pieces[move.index].type;
	if (move.from != PLACEABLE)
		board[move.from] = EMPTY;
	// update piece information
	pieces[move.index].loc = move.to;
	if (move.xindex != NONE)
		pieces[move.xindex].loc = DEAD;

	curr ^= -2;
	hcounter++;
}

void Board::undo(Move move)
{
	// TODO could this function fail?
	pieces[move.index].loc = move.from;
	if (move.xindex == NONE) {
		board[move.to] = EMPTY;
	} else {
		board[move.to] = pieces[move.xindex].type;
		pieces[move.xindex].loc = move.to;
	}
	if (move.from != PLACEABLE)
		board[move.from] = pieces[move.index].type;

	curr ^= -2;
	hcounter--;
}

int Board::validateMove(Move move, char color)
{
	MoveLookup moveLookup;
	int loc;

	// are we moving our own piece?
	if (pieces[move.index].type * color < 0)
		return DONT_OWN;

	// must place king first
	if (history.size() < 2 && abs(pieces[move.index].type) != KING)
		return KING_FIRST;

	// are we placing on an empty square?
	if (move.from == PLACEABLE) {
		if (board[move.to] != EMPTY)
			return NON_EMPTY_PLACE;
		return VALID_MOVE;
	} else if (board[move.to] * color > 0) {
		return CAPTURE_OWN;
	}
	// check for correct piece movement
	moveLookup.setPiece(move.from);
	while ((loc = moveLookup.nextAttack()) != NONE) {
		if (move.to == loc)
			return VALID_MOVE;
	}
	return INVALID_MOVEMENT;
}

int Board::doMove(Move move, char color)
{
	int valid = validateMove(move, color);

	if (valid != VALID_MOVE)
		return valid;
	doMove(move);
	if (inCheck(color)) {
		undo(move);
		return IN_CHECK;
	}
	if (move.from == PLACEABLE && inCheck(color ^ -2)) {
		undo(move);
		return IN_CHECK_PLACE;
	}
	// save move to history
	history.push_back(move);
	return VALID_MOVE;
}

bool Board::undo()
{
	if (!history.size())
		return false;
	Move move = history.back();

	history.pop_back();

	undo(move);

	return true;
}

int Board::CalcScore()
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 15; b++, w++) {
		switch (pieces[b].loc) {
		case PLACEABLE:
		default:
			black += pieceValues[b];
			break;
		case DEAD:
			black -= pieceValues[b];
			break;
		}
		switch (pieces[w].loc) {
		case PLACEABLE:
		default:
			white += pieceValues[b];
			break;
		case DEAD:
			white -= pieceValues[b];
			break;
		}
	}
	// Score for check is added in getMovesList
	white -= black;
	return (curr == WHITE)? white : -white;
}

int Board::getNumMoves(char color)
{
	MoveLookup moveLookup;
	int num = 0, start = (color == BLACK)? 0:16, end = (color == BLACK)? 16:32;
	Move move;

	// we must place king first
	if (hcounter < 2) {
		int idx = pieceIndex(PLACEABLE, KING * color);

		for (int loc = 0; loc < 64; loc++) {
			if (board[loc] != EMPTY)
				continue;
			move.to = loc;
			move.index = idx;
			move.xindex = NONE;
			move.from = PLACEABLE;

			doMove(move);
			// place moves are only valid if neither side is inCheck
			if (!inCheck(color) && !inCheck(color ^ -2))
				num++;
			undo(move);
		}
		return num;
	}
	// generate piece moves
	for (int idx = start, loc; idx < end; idx++) {
		if (pieces[idx].loc == PLACEABLE || pieces[idx].loc == DEAD)
			continue;
		moveLookup.setPiece(pieces[idx].loc);
		while ((loc = moveLookup.nextAttack()) != NONE) {
			move.xindex = (board[loc] == EMPTY)? NONE : pieceIndex(loc, board[loc]);
			move.to = loc;
			move.from = pieces[idx].loc;
			move.index = idx;

			doMove(move);
			if (!inCheck(color))
				num++;
			undo(move);
		}
	}
	// generate piece place moves
	for (int type = PAWN, idx; type <= KING; type++) {
		idx = pieceIndex(PLACEABLE, type * color);
		if (idx == NONE)
			continue;
		for (int loc = 0; loc < 64; loc++) {
			if (board[loc] != EMPTY)
				continue;
			move.index = idx;
			move.to = loc;
			move.xindex = NONE;
			move.from = PLACEABLE;

			doMove(move);
			// place moves are only valid if neither side is inCheck
			if (!inCheck(color) && !inCheck(color ^ -2))
				num++;
			undo(move);
		}
	}
	return num;
}

MovesPly* Board::getMovesList(char color)
{
	// TODO list might work better as a stl::list, or initialize to prev size
	int start = (color == BLACK)? 0:16, end = (color == BLACK)? 16:32;
	MovesPly *data = new MovesPly;
	MoveLookup moveLookup;
	MoveScores item;

	data->size = 0;
	// we must place king first
	if (hcounter < 2) {
		int idx = pieceIndex(PLACEABLE, KING * color);

		for (int loc = 0; loc < 64; loc++) {
			if (board[loc] != EMPTY)
				continue;
			item.move.to = loc;
			item.move.index = idx;
			item.move.xindex = NONE;
			item.move.from = PLACEABLE;

			doMove(item.move);
			// place moves are only valid if neither side is inCheck
			if (!inCheck(color) && !inCheck(color ^ -2)) {
				item.check = false;
				item.score = -CalcScore();
				data->list[data->size++] = item;
			}
			undo(item.move);
		}
		return data;
	}
	// generate piece moves
	for (int idx = start, loc; idx < end; idx++) {
		if (pieces[idx].loc == PLACEABLE || pieces[idx].loc == DEAD)
			continue;
		moveLookup.setPiece(pieces[idx].loc);
		while ((loc = moveLookup.nextAttack()) != NONE) {
			item.move.xindex = (board[loc] == EMPTY)? NONE : pieceIndex(loc, board[loc]);
			item.move.to = loc;
			item.move.from = pieces[idx].loc;
			item.move.index = idx;

			doMove(item.move);
			if (!inCheck(color)) {
				item.check = inCheck(color ^ -2);
				item.score = -CalcScore();
				item.score += -(item.check) & pieceValues[15];
				data->list[data->size++] = item;
			}
			undo(item.move);
		}
	}
	// generate piece place moves
	for (int type = PAWN, idx; type <= KING; type++) {
		idx = pieceIndex(PLACEABLE, type * color);
		if (idx == NONE)
			continue;
		for (int loc = 0; loc < 64; loc++) {
			if (board[loc] != EMPTY)
				continue;
			item.move.index = idx;
			item.move.to = loc;
			item.move.xindex = NONE;
			item.move.from = PLACEABLE;

			doMove(item.move);
			// place moves are only valid if neither side is inCheck
			if (!inCheck(color) && !inCheck(color ^ -2)) {
				item.check = false;
				item.score = -CalcScore();
				data->list[data->size++] = item;
			}
			undo(item.move);
		}
	}
	return data;
}

string Board::printLoc(const char loc)
{
	string s;

	if (loc > PLACEABLE) {
		s += (char)('a' + (loc % 8));
		s += (char)('8' - (loc / 8));
		return s;
	} else if (loc == PLACEABLE) {
		return "aval";
	} else {
		return "dead";
	}
}

string Board::printSquare(int index)
{
	string tmp;

	if (!board[index])
		return "  ";
	tmp = { pieceSymbol[abs(board[index])],
		(board[index] > 0)? ' ':'*', '\0' };
	return tmp;
}

string Board::printMove(Move move)
{
	string out;

	if (move.from == PLACEABLE)
		out += pieceSymbol[abs(pieces[move.index].type)];
	else
		out = printLoc(move.from);
	out += printLoc(move.to);
	return out.c_str();
}

void Board::printBoard()
{
	int rank = 8;
	cout << "  / - + - + - + - + - + - + - + - \\\n";
	for (int i = 0; i < 64;) {
		cout << rank-- << " |";
		for (int j = 0; j < 8; j++)
			cout << " " << printSquare(i++) << "|";
		if (i == 64)
			break;
		cout << "\n  + - + - + - + - + - + - + - + - +\n";
	}
	cout << "\n  \\ - + - + - + - + - + - + - + - /\n"
		<< "    a   b   c   d   e   f   g   h\n";
}

void Board::printPieceList()
{
	string tmp;

	cout << "White:";
	for (int i = 16; i < 32; i++) {
		if (!(i % 8))
			cout << "\n\t";
		cout << pieceSymbol[pieces[i].type] << "(";
		tmp = printLoc(pieces[i].loc);
		if (tmp.length() == 2)
			cout << ' ' << tmp << ' ';
		else
			cout << tmp;
		cout << ") ";
	}
	cout << "\nBlack:";
	for (int i = 0; i < 16; i++) {
		if (!(i % 8))
			cout << "\n\t";
		cout << pieceSymbol[-pieces[i].type] << "(";
		tmp = printLoc(pieces[i].loc);
		if (tmp.length() == 2)
			cout << ' ' << tmp << ' ';
		else
			cout << tmp;
		cout << ") ";
	}
	cout << endl;
}
