/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      Board class functions
 */

#include "Board.h"
#include "MoveLookup.h"
#include "TransTable.h"
#include <iostream>

using namespace std;

const int pieceValue[16] = {224, 224, 224, 224, 224, 224, 224, 224, 337, 337, 560, 560, 896, 896, 1456, 6834};

Board::Board()
{
	reset();
}

void Board::reset()
{
	square = {
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
	};
	piece = {
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
		PLACEABLE, PLACEABLE, PLACEABLE, PLACEABLE,
	};
	key = startHash;
	curr = WHITE;
	ply = 0;
}

void Board::initHash(uint64 hash)
{
	key = hash;
}

int Board::pieceIndex(const int loc) const
{
	for (int i = 0; i < 32; i++)
		if (piece[i] == loc)
			return i;
	return NONE;
}

int Board::pieceIndex(const int loc, const int type) const
{
	static int offset[] = {-1, 0, 8, 10, 12, 14, 15, 16};
	int start = ((type < 0)? 0 : 16) + offset[abs(type)],
		end = ((type < 0)? 0 : 16) + offset[abs(type) + 1];

	for (int i = start; i < end; i++)
		if (piece[i] == loc)
			return i;
	return NONE;
}

bool Board::incheck(const char color)
{
	MoveLookup ml(square);
	int king = (color == WHITE)? 31:15;

	return (piece[king] == PLACEABLE)? false : ml.isAttacked(piece[king]);
}

int Board::isMate()
{
	if (getNumMoves(curr))
		return NOT_MATE;
	if (incheck(curr))
		return CHECK_MATE;
	else
		return STALE_MATE;
}

void Board::make(const Move move)
{
	// update board information
	square[move.to] = pieceType[move.index];
	if (move.from != PLACEABLE)
		square[move.from] = EMPTY;
	// update piece information
	piece[move.index] = move.to;
	if (move.xindex != NONE)
		piece[move.xindex] = DEAD;

	key += (curr == WHITE)? -hashBox[WTM_HASH] : hashBox[WTM_HASH];
	key += hashBox[12 * move.to + typeLookup[move.index]];
	if (move.from != PLACEABLE)
		key -= hashBox[12 * move.from + typeLookup[move.index]];
	else
		key -= hashBox[HOLD_START + typeLookup[move.index]];
	if (move.xindex != NONE)
		key -= hashBox[12 * move.to + typeLookup[move.xindex]];

	curr ^= -2;
	ply++;
}

void Board::unmake(const Move move)
{
	// TODO could this function fail?
	piece[move.index] = move.from;
	if (move.xindex == NONE) {
		square[move.to] = EMPTY;
	} else {
		square[move.to] = pieceType[move.xindex];
		piece[move.xindex] = move.to;
	}
	if (move.from != PLACEABLE)
		square[move.from] = pieceType[move.index];

	key += (curr == WHITE)? -hashBox[WTM_HASH] : hashBox[WTM_HASH];
	key -= hashBox[12 * move.to + typeLookup[move.index]];
	if (move.from != PLACEABLE)
		key += hashBox[12 * move.from + typeLookup[move.index]];
	else
		key += hashBox[HOLD_START + typeLookup[move.index]];
	if (move.xindex != NONE)
		key += hashBox[12 * move.to + typeLookup[move.xindex]];

	curr ^= -2;
	ply--;
}

bool Board::validMove(const Move move)
{
	// TODO: match by type not index
	if (piece[move.index] != move.from)
		return false;
	if (move.xindex != NONE && piece[move.xindex] != move.to)
		return false;
	else if (square[move.to] == EMPTY)
		return false;

	if (move.from != PLACEABLE) {
		MoveLookup ml(square);
		if (!ml.fromto(move.from, move.to))
			return false;
	}
	if (ply < 2 && abs(pieceType[move.index]) != KING)
		return false;
	make(move);
	if (incheck(curr ^ -2))
		return false;
	unmake(move);

	return true;
}

int Board::validMove(string smove, const char color, Move &move)
{
	// pre-setup move
	if (!move.parse(smove))
		return INVALID_FORMAT;

	// setup move.(x)index
	if (move.from == PLACEABLE) {
		move.index = pieceIndex(PLACEABLE, move.index * color);
		if (move.index == NONE)
			return NOPIECE_ERROR;
		move.xindex = pieceIndex(move.to);
		if (move.xindex != NONE)
			return NON_EMPTY_PLACE;
	} else {
		move.index = pieceIndex(move.from);
		if (move.index == NONE)
			return NOPIECE_ERROR;
		else if (square[move.from] * color < 0)
			return DONT_OWN;
		move.xindex = pieceIndex(move.to);
		if (move.xindex != NONE && square[move.to] * color > 0)
			return CAPTURE_OWN;
	}
	// must place king first
	if (ply < 2 && abs(pieceType[move.index]) != KING)
		return KING_FIRST;
	if (move.from != PLACEABLE) {
		MoveLookup ml(square);
		if (!ml.fromto(move.from, move.to))
			return INVALID_MOVEMENT;
	}
	make(move); // curr is opponent after make
	if (incheck(curr ^ -2))
		return IN_CHECK;
	if (move.from == PLACEABLE && incheck(curr))
		return IN_CHECK_PLACE;
	unmake(move);

	return VALID_MOVE;
}

int Board::eval() const
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 15; b++, w++) {
		switch (piece[b]) {
		case PLACEABLE:
		default:
			black += pieceValue[b];
			break;
		case DEAD:
			black -= pieceValue[b];
			break;
		}
		switch (piece[w]) {
		case PLACEABLE:
		default:
			white += pieceValue[b];
			break;
		case DEAD:
			white -= pieceValue[b];
			break;
		}
	}
	white -= black;
	return (curr == WHITE)? -white : white;
}

int Board::getNumMoves(const char color)
{
	MoveLookup movelookup(square);
	int num = 0;
	Move move;

	// we must place king first
	if (ply < 2) {
		int idx = pieceIndex(PLACEABLE, KING * color);

		for (int loc = 0; loc < 64; loc++) {
			if (square[loc] != EMPTY)
				continue;
			move.to = loc;
			move.index = idx;
			move.xindex = NONE;
			move.from = PLACEABLE;

			make(move);
			// place moves are only valid if neither side is inCheck
			if (!incheck(color) && !incheck(color ^ -2))
				num++;
			unmake(move);
		}
		return num;
	}
	// generate piece moves
	int start = (color == BLACK)? 0:16, end = (color == BLACK)? 16:32;
	for (int idx = start; idx < end; idx++) {
		if (piece[idx] == PLACEABLE || piece[idx] == DEAD)
			continue;
		int n = 0;
		char *loc = movelookup.genAll(piece[idx]);
		while (loc[n] != -1) {
			move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			move.to = loc[n];
			move.from = piece[idx];
			move.index = idx;

			make(move);
			if (!incheck(color))
				num++;
			unmake(move);

			n++;
		}
		delete[] loc;
	}
	// generate piece place moves
	for (int type = PAWN; type <= KING; type++) {
		int idx = pieceIndex(PLACEABLE, type * color);
		if (idx == NONE)
			continue;
		for (int loc = 0; loc < 64; loc++) {
			if (square[loc] != EMPTY)
				continue;
			move.index = idx;
			move.to = loc;
			move.xindex = NONE;
			move.from = PLACEABLE;

			make(move);
			// place moves are only valid if neither side is inCheck
			if (!incheck(color) && !incheck(color ^ -2))
				num++;
			unmake(move);
		}
	}
	return num;
}

MoveList* Board::getMovesList(const char color)
{
	// TODO list might work better as a stl::list, or initialize to prev size
	MoveList *data = new MoveList;
	MoveLookup movelookup(square);
	MoveNode item;

	data->size = 0;
	// we must place king first
	if (ply < 2) {
		int idx = pieceIndex(PLACEABLE, KING * color);

		for (int loc = 0; loc < 64; loc++) {
			if (square[loc] != EMPTY)
				continue;
			item.move.to = loc;
			item.move.index = idx;
			item.move.xindex = NONE;
			item.move.from = PLACEABLE;

			make(item.move);
			// place moves are only valid if neither side is inCheck
			if (!incheck(color) && !incheck(color ^ -2)) {
				item.check = false;
				item.score = tt->getScore(this);
				data->list[data->size++] = item;
			}
			unmake(item.move);
		}
		return data;
	}
	// generate piece moves
	int start = (color == BLACK)? 15:31, end = (color == BLACK)? 0:16;
	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == PLACEABLE || piece[idx] == DEAD)
			continue;
		char *loc = movelookup.genAll(piece[idx]);
		int n = 0;
		while (loc[n] != -1) {
			item.move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			item.move.to = loc[n];
			item.move.from = piece[idx];
			item.move.index = idx;

			make(item.move);
			if (!incheck(color)) {
				item.check = incheck(color ^ -2);
				item.score = tt->getScore(this);
				data->list[data->size++] = item;
			}
			unmake(item.move);
			n++;
		}
		delete[] loc;
	}
	// generate piece place moves
	for (int type = QUEEN; type >= PAWN; type--) {
		int idx = pieceIndex(PLACEABLE, type * color);
		if (idx == NONE)
			continue;
		for (int loc = 0; loc < 64; loc++) {
			if (square[loc] != EMPTY)
				continue;
			item.move.index = idx;
			item.move.to = loc;
			item.move.xindex = NONE;
			item.move.from = PLACEABLE;

			make(item.move);
			// place moves are only valid if neither side is inCheck
			if (!incheck(color) && !incheck(color ^ -2)) {
				item.check = false;
				item.score = tt->getScore(this);
				data->list[data->size++] = item;
			}
			unmake(item.move);
		}
	}
	return data;
}

string Board::printSquare(const int index) const
{
	string tmp;

	if (!square[index])
		return "  ";
	tmp = { pieceSymbol[abs(square[index])],
		(square[index] > 0)? ' ':'*', '\0' };
	return tmp;
}

void Board::printBoard() const
{
	cout << "  / - + - + - + - + - + - + - + - \\\n";
	for (int i = 0, rank = 8; ;) {
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
