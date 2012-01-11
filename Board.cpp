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

#include <iostream>
#include <cstring>
#include <sstream>
#include "Board.h"
#include "TransTable.h"

const int genPieceValue[16] = {224, 224, 224, 224, 224, 224, 224, 224, 336, 336, 560, 560, 896, 896, 1456, 0};

const int genLocValue[7][64] = {
	{	0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0},
	{	-5, 0, 0, 0, 0, 0, 0, -5,
/* Pawn */	 0, 5, 5, 5, 5, 5, 5,  0,
		 0, 5, 5, 5, 5, 5, 5,  0,
		 0, 5, 5, 5, 5, 5, 5,  0,
		 0, 5, 5, 5, 5, 5, 5,  0,
		 0, 5, 5, 5, 5, 5, 5,  0,
		 0, 5, 5, 5, 5, 5, 5,  0,
		-5, 0, 0, 0, 0, 0, 0, -5},
	{	-10, -5,  0,  0,  0,  0, -5, -10,
/* Knight */	 -5,  0, 10, 10, 10, 10,  0,  -5,
		  0, 10, 20, 20, 20, 20, 10,   0,
		  0, 10, 20, 20, 20, 20, 10,   0,
		  0, 10, 20, 20, 20, 20, 10,   0,
		  0, 10, 20, 20, 20, 20, 10,   0,
		 -5,  0, 10, 10, 10, 10,  0,  -5,
		-10, -5,  0,  0,  0,  0, -5, -10},
	{	-10, -10, -10, -10, -10, -10, -10, -10,
/* Bishop */	-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10, -10, -10, -10, -10, -10, -10, -10},
	{	-10, -10, -10, -10, -10, -10, -10, -10,
/* Rook */	-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10, -10, -10, -10, -10, -10, -10, -10},
	{	-10, -10, -10, -10, -10, -10, -10, -10,
/* Queen */	-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10, -10, -10, -10, -10, -10, -10, -10},
	{	-10,  0,  0,  0,  0,  0,  0, -10,
/* King */	  0, 15, 15, 15, 15, 15, 15,   0,
		  0, 15, 15, 15, 15, 15, 15,   0,
		  0, 15, 15, 15, 15, 15, 15,   0,
		  0, 15, 15, 15, 15, 15, 15,   0,
		  0, 15, 15, 15, 15, 15, 15,   0,
		  0, 15, 15, 15, 15, 15, 15,   0,
		-10,  0,  0,  0,  0,  0,  0, -10}
	};

void GenBoard::reset()
{
	memset(square, EMPTY, 64);
	memset(piece, PLACEABLE, 32);
	copy(InitPieceType, InitPieceType + 32, pieceType);
#ifdef TT_ENABLED
	key = startHash;
	key += hashBox[WTM_HASH];
	for (int i = HOLD_START; i < HOLD_END; i++)
		key += hashBox[i];
#endif
	stm = WHITE;
	ply = 0;
}

GenBoard::GenBoard()
{
	reset();
}

void GenBoard::rebuildHash()
{
#ifdef TT_ENABLED
	key = startHash;
	key += (stm == WHITE)? hashBox[WTM_HASH] : 0;

	for (int i = 0; i < 32; i++) {
		switch (piece[i]) {
		default:
			key -= hashBox[HOLD_START + typeLookup[i]];
			key += hashBox[12 * piece[i] + typeLookup[i]];
			break;
		case DEAD:
			key -= hashBox[HOLD_START + typeLookup[i]];
			break;
		case PLACEABLE:
			break;
		}
	}
#endif
}

void GenBoard::setBoard(const GenPosition &pos)
{
	copy(pos.square, pos.square + 64, square);
	copy(pos.piece, pos.piece + 32, piece);

	ply = pos.ply;
	stm = pos.stm;

	rebuildHash();
}

int GenBoard::pieceIndex(const int8 loc, const int8 type) const
{
	static const int offset[] = {-1, 0, 8, 10, 12, 14, 15, 16};
	const int start = ((type < 0)? 0 : 16) + offset[ABS(type)],
		end = ((type < 0)? 0 : 16) + offset[ABS(type) + 1];

	for (int i = start; i < end; i++)
		if (piece[i] == loc)
			return i;
	return NONE;
}

void GenBoard::make(const GenMove &move)
{
#ifdef DEBUG_MAKE_MOVE
	assert(pieceType[move.index] * stm > 0);
	assert(piece[move.index] == move.from);
	if (move.from != PLACEABLE)
		assert(square[move.from] == pieceType[move.index]);
	if (move.xindex == NONE) {
		assert(square[move.to] == EMPTY);
	} else {
		assert(square[move.to] == pieceType[move.xindex]);
		assert(piece[move.xindex] == move.to);
	}
#endif

	// update board information
	square[move.to] = pieceType[move.index];
	if (move.from != PLACEABLE)
		square[move.from] = EMPTY;
	// update piece information
	piece[move.index] = move.to;
	if (move.xindex != NONE)
		piece[move.xindex] = DEAD;

#ifdef TT_ENABLED
	key += (stm == WHITE)? -hashBox[WTM_HASH] : hashBox[WTM_HASH];
	key += hashBox[12 * move.to + typeLookup[move.index]];
	if (move.from != PLACEABLE)
		key -= hashBox[12 * move.from + typeLookup[move.index]];
	else
		key -= hashBox[HOLD_START + typeLookup[move.index]];
	if (move.xindex != NONE)
		key -= hashBox[12 * move.to + typeLookup[move.xindex]];
#endif
	stm ^= -2;
	ply++;

#ifdef DEBUG_MAKE_MOVE
	assert(pieceType[move.index] * stm < 0);
	assert(piece[move.index] == move.to);
	assert(square[move.to] == pieceType[move.index]);
	if (move.from != PLACEABLE)
		assert(square[move.from] == EMPTY);
	if (move.xindex != NONE)
		assert(piece[move.xindex] == DEAD);
#endif
}

void GenBoard::unmake(const GenMove &move)
{
#ifdef DEBUG_MAKE_MOVE
	assert(pieceType[move.index] * stm < 0);
	assert(piece[move.index] == move.to);
	assert(square[move.to] == pieceType[move.index]);
	if (move.from != PLACEABLE)
		assert(square[move.from] == EMPTY);
	if (move.xindex != NONE)
		assert(piece[move.xindex] == DEAD);
#endif

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

#ifdef TT_ENABLED
	key += (stm == WHITE)? -hashBox[WTM_HASH] : hashBox[WTM_HASH];
	key -= hashBox[12 * move.to + typeLookup[move.index]];
	if (move.from != PLACEABLE)
		key += hashBox[12 * move.from + typeLookup[move.index]];
	else
		key += hashBox[HOLD_START + typeLookup[move.index]];
	if (move.xindex != NONE)
		key += hashBox[12 * move.to + typeLookup[move.xindex]];
#endif
	stm ^= -2;
	ply--;

#ifdef DEBUG_MAKE_MOVE
	assert(pieceType[move.index] * stm > 0);
	assert(piece[move.index] == move.from);
	if (move.from != PLACEABLE)
		assert(square[move.from] == pieceType[move.index]);
	if (move.xindex == NONE) {
		assert(square[move.to] == EMPTY);
	} else {
		assert(square[move.to] == pieceType[move.xindex]);
		assert(piece[move.xindex] == move.to);
	}
#endif
}

bool GenBoard::anyMoves(const int8 color)
{
	GenMove move;

	// shortest games takes 5 half-moves
	if (ply < 4)
		return true;

	// generate piece moves
	const int start = (color == BLACK)? 0:16, end = (color == BLACK)? 16:32;
	for (int idx = start; idx < end; idx++) {
		if (piece[idx] == PLACEABLE || piece[idx] == DEAD)
			continue;

		const int8* const loc = genAll(piece[idx]);
		for (int n = 0; loc[n] != -1; n++) {
			move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			move.to = loc[n];
			move.from = piece[idx];
			move.index = idx;

			make(move);
			if (!incheck(color)) {
				delete[] loc;
				unmake(move);
				return true;
			}
			unmake(move);
		}
		delete[] loc;
	}
	// generate piece place moves
	for (int type = PAWN; type <= KING; type++) {
		const int idx = pieceIndex(PLACEABLE, type * color);
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
			if (!incheck(color) && !incheck(color ^ -2)) {
				unmake(move);
				return true;
			}
			unmake(move);
		}
	}
	return false;
}

int GenBoard::isMate()
{
	if (anyMoves(stm))
		return NOTMATE;
	else if (incheck(stm))
		return (stm == WHITE)? BLACK_CHECKMATE : WHITE_CHECKMATE;
	else
		return STALEMATE;
}

bool GenBoard::validMove(const GenMove &moveIn, GenMove &move)
{
	move = moveIn;

	if ((move.index = pieceIndex(move.from, pieceType[move.index])) == NONE)
		return false;
	if (pieceType[move.index] * stm <= 0)
		return false;
	if (move.xindex != NONE) {
		if ((move.xindex = pieceIndex(move.to, pieceType[move.xindex])) == NONE)
			return false;
	} else if (square[move.to] != EMPTY) {
		return false;
	}

	if (move.from != PLACEABLE && !fromto(move.from, move.to))
			return false;
	if (ply < 2 && ABS(pieceType[move.index]) != KING)
		return false;

	bool ret = true;

	make(move);
	if (incheck(stm ^ -2))
		ret = false;
	if (move.from == PLACEABLE && incheck(stm))
		ret = false;
	unmake(move);

	return ret;
}

int GenBoard::validMove(const string &smove, const int8 color, GenMove &move)
{
	// pre-setup move
	if (!move.parse(smove))
		return INVALID_FORMAT;

	// setup move.(x)index
	if (move.from == PLACEABLE) {
		move.index = pieceIndex(PLACEABLE, move.index * color);
		if (move.index == NONE)
			return NOPIECE_ERROR;
		move.xindex = pieceIndex(move.to, square[move.to]);
		if (move.xindex != NONE)
			return NON_EMPTY_PLACE;
	} else {
		move.index = pieceIndex(move.from, square[move.from]);
		if (move.index == NONE)
			return NOPIECE_ERROR;
		else if (square[move.from] * color < 0)
			return DONT_OWN;
		move.xindex = pieceIndex(move.to, square[move.to]);
		if (move.xindex != NONE && square[move.to] * color > 0)
			return CAPTURE_OWN;
	}
	// must place king first
	if (ply < 2 && ABS(pieceType[move.index]) != KING)
		return KING_FIRST;
	if (move.from != PLACEABLE && !fromto(move.from, move.to))
			return INVALID_MOVEMENT;
	int ret = VALID_MOVE;

	make(move);
	// stm is opponent after make
	if (incheck(stm ^ -2))
		ret = IN_CHECK;
	if (move.from == PLACEABLE && incheck(stm))
		ret = IN_CHECK_PLACE;
	unmake(move);

	return ret;
}

int GenBoard::eval() const
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 16; b++, w++) {
		switch (piece[b]) {
		default:
			black += regLocValue[pieceType[w]][piece[b]];
		case PLACEABLE:
			black += regPieceValue[b];
			break;
		case DEAD:
			black -= regPieceValue[b];
			break;
		}
		switch (piece[w]) {
		default:
			white += regLocValue[pieceType[w]][piece[w]];
		case PLACEABLE:
			white += regPieceValue[b];
			break;
		case DEAD:
			white -= regPieceValue[b];
			break;
		}
	}
	white -= black;
	return (stm == WHITE)? -white : white;
}

void GenBoard::getPlaceMoveList(GenMoveList* const data, const int8 pieceType)
{
	const int idx = pieceIndex(PLACEABLE, pieceType);
	const int8 color = pieceType / ABS(pieceType);
	GenMoveNode item;

	if (idx == NONE)
		return;
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
			// item.check initialized to false
			item.score = eval();
			data->list[data->size++] = item;
		}
		unmake(item.move);
	}
}

void GenBoard::getMoveList(GenMoveList* const data, const int8 color, const MoveType movetype)
{
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	GenMoveNode item;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == PLACEABLE || piece[idx] == DEAD)
			continue;

		int8 *loc;
		switch (movetype) {
		case MoveType::ALL:
		default:
			loc = genAll(piece[idx]);
			break;
		case MoveType::CAPTURE:
			loc = genCapture(piece[idx]);
			break;
		case MoveType::MOVE:
			loc = genMove(piece[idx]);
			break;
		}

		for (int n = 0; loc[n] != -1; n++) {
			item.move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			item.move.to = loc[n];
			item.move.from = piece[idx];
			item.move.index = idx;

			make(item.move);
			if (!incheck(color)) {
				item.check = incheck(color ^ -2);
				item.score = eval();
				data->list[data->size++] = item;
			}
			unmake(item.move);
		}
		delete[] loc;
	}
}

GenMoveList* GenBoard::getMoveList(const int8 color, const MoveType movetype)
{
	GenMoveList* const data = new GenMoveList;
	data->size = 0;

	switch (movetype) {
	case MoveType::ALL:
		if (ply < 2) {
			getPlaceMoveList(data, KING * color);
			break;
		}
		getMoveList(data, color, MoveType::ALL);
		for (int type = QUEEN; type >= PAWN; type--)
			getPlaceMoveList(data, type * color);
		break;
	case MoveType::PLACE:
		if (ply < 2) {
			getPlaceMoveList(data, KING * color);
			break;
		}
		for (int type = QUEEN; type >= PAWN; type--)
			getPlaceMoveList(data, type * color);
		break;
	case MoveType::CAPTURE:
	case MoveType::MOVE:
		getMoveList(data, color, movetype);
		break;
	}
	return data;
}

string GenBoard::printPieceList() const
{
	stringstream buff;
	string tmp;

	buff << "White:\t";
	for (int i = 16; i < 32; i++) {
		if (i != 16 && !(i % 8))
			buff << "\n\t";
		buff << pieceSymbol[pieceType[i]] << "(";
		tmp = printLoc(piece[i]);
		if (tmp.length() == 2)
			buff << ' ' << tmp << ' ';
		else
			buff << tmp;
		buff << ") ";
	}
	buff << "\nBlack:\t";
	for (int i = 0; i < 16; i++) {
		if (i && !(i % 8))
			buff << "\n\t";
		buff << pieceSymbol[-pieceType[i]] << "(";
		tmp = printLoc(piece[i]);
		if (tmp.length() == 2)
			buff << ' ' << tmp << ' ';
		else
			buff << tmp;
		buff << ") ";
	}
	return buff.str();
}

void GenBoard::dumpDebug() const
{
	cout << "hash:" << key << " stm:" << (int)stm << " ply:" << ply << endl;
	cout << printBoard();
	cout << printPieceList() << endl;
}

// --- Start Regular Chess ---

const int regPieceValue[7] = {0, 224, 336, 560, 896, 1456, 0};

const int8 InitRegBoard[64] = {
	BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK,
	BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,
	EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
	EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
	EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
	EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
	WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,
	WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK};

const int8 InitRegPiece[32] = {
	A7, B7, C7, D7, E7, F7, G7, H7,
	B8, G8, C8, F8, A8, H8, D8, E8,
	A2, B2, C2, D2, E2, F2, G2, H2,
	B1, G1, C1, F1, A1, H1, D1, E1};

const int regLocValue[7][64] = {
	{	0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0},
	{	  0,   0,   0,   0,   0,   0,   0,   0,
/* Pawn */	 10,  10,  10,  10,  10,  10,  10,  10,
		  5,   5,   5,   5,   5,   5,   5,   5,
		  0,   0,   5,   5,   5,   5,   0,   0,
		  0,   0,  -5,  -5,  -5,  -5,   0,   0,
		 -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,
		-10, -10, -10, -10, -10, -10, -10, -10,
		  0,   0,   0,   0,   0,   0,   0,   0},
	{	-10, -5,  0,  0,  0,  0, -5, -10,
/* Knight */	 -5,  0, 10, 10, 10, 10,  0,  -5,
		  0, 10, 20, 20, 20, 20, 10,   0,
		  0, 10, 20, 20, 20, 20, 10,   0,
		  0, 10, 20, 20, 20, 20, 10,   0,
		  0, 10, 20, 20, 20, 20, 10,   0,
		 -5,  0, 10, 10, 10, 10,  0,  -5,
		-10, -5,  0,  0,  0,  0, -5, -10},
	{	-10, -10, -10, -10, -10, -10, -10, -10,
/* Bishop */	-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10, -10, -10, -10, -10, -10, -10, -10},
	{	-10, -10, -10, -10, -10, -10, -10, -10,
/* Rook */	-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10, -10, -10, -10, -10, -10, -10, -10},
	{	-10, -10, -10, -10, -10, -10, -10, -10,
/* Queen */	-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  20,  20,  10,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10, -10, -10, -10, -10, -10, -10, -10},
	{	  0, -5, -10, -15, -20, -15, -10, -5,
/* King */	  0, -5, -10, -15, -15, -15, -10, -5,
		  0, -5, -10, -10, -10, -10, -10, -5,
		  0, -5,  -5,  -5,  -5,  -5,  -5, -5,
		  0,  5,   5,   5,   5,   5,   5,  5,
		  0,  5,  10,  10,  10,  10,  10,  5,
		  0,  5,  10,  15,  15,  15,  10,  5,
		  0,  5,  10,  15,  20,  15,  10,  5}
	};

void RegBoard::reset()
{
	copy(InitRegBoard, InitRegBoard + 64, square);
	copy(InitRegPiece, InitRegPiece + 32, piece);
	copy(InitPieceType, InitPieceType + 32, pieceType);
#ifdef TT_ENABLED
	key = startHash;
#endif
	stm = WHITE;
	ply = 0;
	flags.reset();
}

RegBoard::RegBoard()
{
	reset();
}

void RegBoard::rebuildHash()
{
#ifdef TT_ENABLED
	key = startHash;
	key ^= (stm == WHITE)? hashBox[WTM_HASH] : 0;

	for (int i = 0; i < 32; i++) {
		if (piece[i] != DEAD)
			key ^= hashBox[12 * piece[i] + pieceType[i]];
	}
	key ^= (flags.bits & 0x08)? hashBox[ENPASSANT_HASH] : 0;
	key ^= (flags.bits & 0x10)? hashBox[CASTLE_HASH + WHITE] : 0;
	key ^= (flags.bits & 0x40)? hashBox[CASTLE_HASH + BLACK] : 0;
	key ^= (flags.bits & 0x20)? hashBox[CASTLE_HASH + 2 * WHITE] : 0;
	key ^= (flags.bits & 0x80)? hashBox[CASTLE_HASH + 2 * BLACK] : 0;
#endif
}

void RegBoard::setBoard(RegPosition pos)
{
	copy(pos.square, pos.square + 64, square);
	copy(pos.piece, pos.piece + 32, piece);

	flags = pos.flags;
	ply = pos.ply;
	stm = pos.stm;
#ifdef TT_ENABLED
	rebuildHash();
#endif
}

int RegBoard::pieceIndex(const int8 loc, const int8 type) const
{
	const int start = (type > 0)? 16:0, end = (type > 0)? 32:16;

	for (int i = start; i < end; i++)
		if (piece[i] == loc && pieceType[i] == type)
			return i;
	return NONE;
}

void RegBoard::validateBoard(const RegMove &move) const
{
	int cpt = 0;

	if (square[move.from] * square[move.to] > 0)
		goto error;
	for (int i = A1; i <= H1; i++)
		if (ABS(square[i]) == PAWN) {
			cpt = 1;
			goto error;
		}
	for (int i = A8; i <= H8; i++) {
		if (ABS(square[i]) == PAWN) {
			cpt = 2;
			goto error;
		}
	}
	for (int i = 0; i < 32; i++) {
		if (piece[i] == DEAD)
			continue;
		if (piece[i] < 0 || piece[i] > 63) {
			cpt = 3;
			goto error;
		}
		if (ABS(pieceType[i]) < 1 || ABS(pieceType[i]) > 6) {
			cpt = 4;
			goto error;
		}
		if (square[piece[i]] == pieceType[i])
			continue;
		cerr << " " << i;
		cpt = 5;
		goto error;
	}
	return;
error:
	cerr << "E:" << move.dump() << " " << printZfen() << " "
	<< (int)square[move.from] << " " << (int)square[move.to] << " " << cpt << endl;
	for (int i = 0; i < 32; i++)
		cerr << (int) piece[i] << "," << (int) pieceType[i] << " ";
	cerr << endl;
	assert(0);
}

void RegBoard::make(const RegMove &move)
{
	const int isWhite = (move.index > 15), color = isWhite? WHITE : BLACK;
#ifdef TT_ENABLED
	key ^= hashBox[13 * move.from + pieceType[move.index] + 6];
#endif
	if (move.getCastle()) {
		bool left = (move.getCastle() == 0x20);
		int castleTo = move.to + (left? 1 : -1);
		int castleI = pieceIndex(move.to - (move.to & 0x7) + (left? 0 : 7), color * ROOK);
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[castleI] + pieceType[castleI] + 6];
		key ^= hashBox[13 * castleTo + pieceType[castleI] + 6];
		if (flags.canKingCastle(color))
			key ^= hashBox[CASTLE_HASH + color];
		if (flags.canQueenCastle(color))
			key ^= hashBox[CASTLE_HASH + color * 2];
#endif
		square[castleTo] = pieceType[castleI];
		square[piece[castleI]] = EMPTY;
		piece[castleI] = castleTo;
		flags.clearCastle(color);
	} else if (ABS(pieceType[move.index]) == ROOK) {
		if (move.from == (isWhite? H1:H8) && flags.canKingCastle(color)) {
			flags.clearKingCastle(color);
#ifdef TT_ENABLED
			key ^= hashBox[CASTLE_HASH + color];
#endif
		} else if (move.from == (isWhite? A1:A8) && flags.canQueenCastle(color)) {
			flags.clearQueenCastle(color);
#ifdef TT_ENABLED
			key ^= hashBox[CASTLE_HASH + color * 2];
#endif
		}
	} else if (ABS(pieceType[move.index]) == KING && flags.canCastle(color)) {
#ifdef TT_ENABLED
		if (flags.canKingCastle(color))
			key ^= hashBox[CASTLE_HASH + color];
		if (flags.canQueenCastle(color))
			key ^= hashBox[CASTLE_HASH + color * 2];
#endif
		flags.clearCastle(color);
	} else if (move.getPromote()) {
		pieceType[move.index] = move.getPromote() * color;
	}
#ifdef TT_ENABLED
	key ^= hashBox[13 * move.to + pieceType[move.index] + 6];
#endif

	if (flags.canEnPassant()) {
		flags.clearEnPassant();
#ifdef TT_ENABLED
		key ^= hashBox[ENPASSANT_HASH];
#endif
	}

	// update board information
	square[move.to] = pieceType[move.index];
	square[move.from] = EMPTY;
	// update piece information
	piece[move.index] = move.to;
	if (move.xindex != NONE) {
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[move.xindex] + pieceType[move.xindex] + 6];
#endif
		if (move.getEnPassant())
			square[piece[move.xindex]] = EMPTY;
		piece[move.xindex] = DEAD;
	} else if (ABS(pieceType[move.index]) == PAWN && ABS(move.to - move.from) == 16) {
		flags.setEnPassant(move.to & 0x7);
#ifdef TT_ENABLED
		key ^= hashBox[ENPASSANT_HASH];
#endif
	}
#ifdef TT_ENABLED
	key ^= hashBox[WTM_HASH];
#endif
	stm ^= -2;
	ply++;
}

void RegBoard::unmake(const RegMove &move, const MoveFlags &undoFlags)
{
	const int isWhite = (move.index > 15), color = isWhite? WHITE : BLACK;

#ifdef TT_ENABLED
	const int8 bits = flags.bits ^ undoFlags.bits;
	key ^= (bits & ((color == WHITE)? 0x10 : 0x40))? hashBox[CASTLE_HASH + color] : 0;
	key ^= (bits & ((color == WHITE)? 0x20 : 0x80))? hashBox[CASTLE_HASH + 2 * color] : 0;
	key ^= (bits & 0x8)? hashBox[ENPASSANT_HASH] : 0;
	key ^= hashBox[13 * move.to + pieceType[move.index] + 6];
#endif

	if (move.getCastle()) {
		bool left = (move.from - move.to > 0);
		int castleFrom = move.to - (move.to & 0x7) + (left? 0 : 7);
		int castleI = pieceIndex(move.to + (left? 1 : -1), isWhite? WHITE_ROOK : BLACK_ROOK);
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[castleI] + pieceType[castleI] + 6];
		key ^= hashBox[13 * castleFrom + pieceType[castleI] + 6];
#endif
		square[piece[castleI]] = EMPTY;
		square[castleFrom] = pieceType[castleI];
		piece[castleI] = castleFrom;
	} else if (move.getPromote()) {
		pieceType[move.index] = PAWN * color;
	}
#ifdef TT_ENABLED
	key ^= hashBox[13 * move.from + pieceType[move.index] + 6];
#endif

	piece[move.index] = move.from;
	if (move.xindex == NONE) {
		square[move.to] = EMPTY;
	} else {
		if (move.getEnPassant()) {
			piece[move.xindex] = move.to + ((move.from - move.to > 0)? 8 : -8);
			square[piece[move.xindex]] = PAWN * -color;
			square[move.to] = EMPTY;
		} else {
			piece[move.xindex] = move.to;
			square[move.to] = pieceType[move.xindex];
		}
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[move.xindex] + pieceType[move.xindex] + 6];
#endif
	}
	square[move.from] = pieceType[move.index];
#ifdef TT_ENABLED
	key ^= hashBox[WTM_HASH];
#endif
	flags = undoFlags;
	stm ^= -2;
	ply--;
}

bool RegBoard::anyMoves(const int8 color)
{
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	const MoveFlags undoFlags = flags;
	RegMoveNode item;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == DEAD)
			continue;

		const int8* const loc = genAll(piece[idx]);

		for (int n = 0; loc[n] != -1; n++) {
			item.move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			item.move.to = loc[n];
			item.move.from = piece[idx];
			item.move.index = idx;

			// nothing special for promotion
			make(item.move);
			if (!incheck(color)) {
				unmake(item.move, undoFlags);
				return true;
			}
			unmake(item.move, undoFlags);
		}
		delete[] loc;
	}

	// can't castle while in check
	const bool inCheck = incheck(color);
	const int king = (color == WHITE)? E1 : E8;

	// King Side
	if (!inCheck && flags.canKingCastle(color) && square[king + 1] == EMPTY &&
	square[king + 2] == EMPTY && !isAttacked(king + 1, color) &&
	!isAttacked(king + 2, color) && ABS(square[((color == WHITE)? H1:H8)]) == ROOK) {
		item.move.xindex = NONE;
		item.move.to = king + 1;
		item.move.from = king;
		item.move.index = (color == WHITE)? 31:15;

		make(item.move);
		if (!incheck(color)) {
			unmake(item.move, undoFlags);
			return true;
		}
		unmake(item.move, undoFlags);
	}
	// Queen Side
	if (!inCheck && flags.canQueenCastle(color) && square[king - 1] == EMPTY &&
	square[king - 2] == EMPTY && square[king - 3] == EMPTY &&
	!isAttacked(king - 1, color) && !isAttacked(king - 2, color) &&
	ABS(square[((color == WHITE)? A1:A8)]) == ROOK) {
		item.move.xindex = NONE;
		item.move.to = king - 1;
		item.move.from = king;
		item.move.index = (color == WHITE)? 31:15;

		make(item.move);
		if (!incheck(color)) {
			unmake(item.move, undoFlags);
			return true;
		}
		unmake(item.move, undoFlags);
	}

	if (!flags.canEnPassant())
		return false;

	const int eps_file = flags.enPassantFile();
	const int8 eps = eps_file + ((color == WHITE)? A5 : A4),
		your_pawn = (color == WHITE)? WHITE_PAWN : BLACK_PAWN,
		opp_pawn = your_pawn * -1;

	// en passant to left
	if (eps_file != 0 && square[eps - 1] == your_pawn) {
		item.move.xindex = pieceIndex(eps, opp_pawn);
		item.move.to = eps + 8 * opp_pawn;
		item.move.from = eps - 1;
		item.move.index = pieceIndex(eps - 1, your_pawn);
		item.move.setEnPassant();

		make(item.move);
		if (!incheck(color)) {
			unmake(item.move, undoFlags);
			return true;
		}
		unmake(item.move, undoFlags);
	}
	// en passant to right
	if (eps_file != 7 && square[eps + 1] == your_pawn) {
		item.move.xindex = pieceIndex(eps, opp_pawn);
		item.move.to = eps + 8 * opp_pawn;
		item.move.from = eps + 1;
		item.move.index = pieceIndex(eps + 1, your_pawn);
		item.move.setEnPassant();

		make(item.move);
		if (!incheck(color)) {
			unmake(item.move, undoFlags);
			return true;
		}
		unmake(item.move, undoFlags);
	}
	return false;
}

int RegBoard::isMate()
{
	if (anyMoves(stm))
		return NOTMATE;
	else if (incheck(stm))
		return (stm == WHITE)? BLACK_CHECKMATE : WHITE_CHECKMATE;
	else
		return STALEMATE;
}

int RegBoard::validCastle(RegMove &move, const int8 color)
{
	// can we castle on that side
	if (!(flags.canCastle(color) && move.getCastle()))
		return CANT_CASTLE;
	// can't castle while in check
	if (incheck(color))
		return CANT_CASTLE;

	const int king = (color == WHITE)? E1 : E8;

	// king side
	if (move.getCastle() == 0x10 && square[king + 1] == EMPTY && square[king + 2] == EMPTY &&
	!isAttacked(king + 1, color) && !isAttacked(king + 2, color) &&
	ABS(square[((color == WHITE)? H1:H8)]) == ROOK) {
		move.index = (color == WHITE)? 31 : 15;
		move.xindex = NONE;
		move.from = king;
		move.to = king + 2;
		return VALID_MOVE;
	} else if (move.getCastle() == 0x20 && square[king - 1] == EMPTY && square[king - 2] == EMPTY &&
	square[king - 3] == EMPTY && !isAttacked(king - 1, color) && !isAttacked(king - 2, color) &&
	ABS(square[((color == WHITE)? A1:A8)]) == ROOK) {
		move.index = (color == WHITE)? 31 : 15;
		move.xindex = NONE;
		move.from = king;
		move.to = king - 2;
		return VALID_MOVE;
	}
	return CANT_CASTLE;
}

// Board flag must have enpassant enabled
int RegBoard::validEnPassant(RegMove &move, const int8 color)
{
	const MoveFlags undoFlags = flags;
	const int8 ep = flags.enPassantFile() + ((color == WHITE)? A5 : A4),
		ep_to = ep + ((color == WHITE)? -8 : 8);

	if (move.to == ep_to && ABS(ep - move.from) == 1) {
		move.index = pieceIndex(move.from, square[move.from]);
		move.xindex = pieceIndex(ep, square[ep]);
		move.setEnPassant();

		int ret = VALID_MOVE;

		make(move);
		// stm is opponent after make
		if (incheck(stm ^ -2))
			ret = IN_CHECK;
		unmake(move, undoFlags);
		return ret;
	}
	return INVALID_MOVEMENT;
}

bool RegBoard::validMove(const RegMove &moveIn, RegMove &move)
{
	if (moveIn.from == moveIn.to)
		return false;

	const MoveFlags undoFlags = flags;
	move = moveIn;

	move.index = pieceIndex(move.from, square[move.from]);
	if (move.index == NONE)
		return false;
	else if (square[move.from] * stm < 0)
		return false;
	move.xindex = pieceIndex(move.to, square[move.to]);
	if (move.xindex != NONE && square[move.to] * stm > 0)
		return false;

	if (move.getCastle()) {
		return validCastle(move, stm) == VALID_MOVE;
	} else if (move.getEnPassant() && flags.canEnPassant()) {
		return validEnPassant(move, stm) == VALID_MOVE;
	} else if (isPromote(move, stm) && ABS(square[move.from]) == PAWN) {
		if  (!move.getPromote())
			move.setPromote(QUEEN);
	} else {
		move.flags = 0;
	}

	if (!fromto(move.from, move.to))
		return false;

	int ret = true;

	make(move);
	// stm is opponent after make
	if (incheck(stm ^ -2))
		ret = false;
	unmake(move, undoFlags);

	return ret;
}

int RegBoard::validMove(const string &smove, const int8 color, RegMove &move)
{
	const MoveFlags undoFlags = flags;

	// pre-setup move
	if (!move.parse(smove))
		return INVALID_FORMAT;

	// if castle flag is set, move must a castle to be valid
	if (move.getCastle())
		return validCastle(move, color);

	move.index = pieceIndex(move.from, square[move.from]);

	switch (ABS(pieceType[move.index])) {
	case PAWN:
		// en passant
		if (flags.canEnPassant() && validEnPassant(move, color) == VALID_MOVE)
			return VALID_MOVE;

		if (!isPromote(move, color)) {
			move.flags = 0;
			break;
		} else if (!move.getPromote()) {
			// manualy set to queen if not specified
			move.setPromote(QUEEN);
		}
		break;
	case KING:
		// manual castling without proper O-O/O-O-O notation
		if (ABS(move.from - move.to) == 2) {
			move.setCastle((move.from > move.to)? 0x20 : 0x10);
			return validCastle(move, color);
		}
	default:
		// move can't be special, so clear flags
		move.flags = 0;
	}

	if (move.index == NONE)
		return NOPIECE_ERROR;
	else if (square[move.from] * color < 0)
		return DONT_OWN;
	move.xindex = pieceIndex(move.to, square[move.to]);
	if (move.xindex != NONE && square[move.to] * color > 0)
		return CAPTURE_OWN;

	if (!fromto(move.from, move.to))
		return INVALID_MOVEMENT;

	int ret = VALID_MOVE;

	make(move);
	// stm is opponent after make
	if (incheck(stm ^ -2))
		ret = IN_CHECK;
	unmake(move, undoFlags);

	return ret;
}

int RegBoard::eval() const
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 16; b++, w++) {
		if (piece[b] != DEAD) {
			int mod = (pieceType[b] == BLACK_PAWN || pieceType[b] == BLACK_KING)? -1:1;
			black += mod * regLocValue[-pieceType[b]][piece[b]];
			black += regPieceValue[-pieceType[b]];
		} else {
			black -= regPieceValue[-pieceType[b]];
		}
		if (piece[w] != DEAD) {
			white += regLocValue[pieceType[w]][piece[w]];
			white += regPieceValue[pieceType[w]];
		} else {
			white -= regPieceValue[pieceType[w]];
		}
	}
	white -= black;
	return (stm == WHITE)? -white : white;
}

void RegBoard::getMoveList(RegMoveList *data, const int8 color, const MoveType movetype)
{
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	const MoveFlags undoFlags = flags;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == DEAD)
			continue;

		int8 *loc;
		switch (movetype) {
		case MoveType::ALL:
		default:
			loc = genAll(piece[idx]);
			break;
		case MoveType::CAPTURE:
			loc = genCapture(piece[idx]);
			break;
		case MoveType::MOVE:
			loc = genMove(piece[idx]);
			break;
		}

		for (int n = 0; loc[n] != -1; n++) {
			RegMoveNode item;

			item.move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			item.move.to = loc[n];
			item.move.from = piece[idx];
			item.move.index = idx;

			if (ABS(pieceType[idx]) == PAWN && isPromote(item.move, color)) {
				item.move.setPromote(QUEEN);

				make(item.move);
				if (incheck(color)) {
					unmake(item.move, undoFlags);
					item.move.flags = 0;
					continue;
				}
				unmake(item.move, undoFlags);

				data->list[data->size++] = item;
				for (int i = ROOK; i > PAWN; i--) {
					item.move.setPromote(i);

					make(item.move);
					item.check = incheck(color ^ -2);
					item.score = eval();
					unmake(item.move, undoFlags);

					data->list[data->size++] = item;
				}
				item.move.flags = 0;
			} else {
				make(item.move);
				if (!incheck(color)) {
					item.check = incheck(color ^ -2);
					item.score = eval();
					data->list[data->size++] = item;
				}
				unmake(item.move, undoFlags);
			}
		}
		delete[] loc;
	}
}

void RegBoard::getCastleMoveList(RegMoveList *data, const int8 color)
{
	// can't castle while in check
	if (incheck(color))
		return;

	const int king = (color == WHITE)? E1 : E8;
	const int8 kindex = (color == WHITE)? 31 : 15;
	RegMoveNode item;

	// King Side
	if (flags.canKingCastle(color) && square[king + 1] == EMPTY && square[king + 2] == EMPTY &&
	!isAttacked(king + 1, color) && !isAttacked(king + 2, color) &&
	ABS(square[((color == WHITE)? H1:H8)]) == ROOK) {
		item.move.xindex = NONE;
		item.move.to = king + 2;
		item.move.from = king;
		item.move.index = kindex;
		item.move.setCastle(0x10);
		item.score = eval();
		item.check = incheck(color ^ -2);

		data->list[data->size++] = item;
	}
	// Queen Side
	if (flags.canQueenCastle(color) && square[king - 1] == EMPTY && square[king - 2] == EMPTY &&
	square[king - 3] == EMPTY && !isAttacked(king - 1, color) && !isAttacked(king - 2, color) &&
	ABS(square[((color == WHITE)? A1:A8)]) == ROOK) {
		item.move.xindex = NONE;
		item.move.to = king - 2;
		item.move.from = king;
		item.move.index = kindex;
		item.move.setCastle(0x20);
		item.score = eval();
		item.check = incheck(color ^ -2);

		data->list[data->size++] = item;
	}
}

void RegBoard::getEnPassantMoveList(RegMoveList *data, const int8 color)
{
	if (!flags.canEnPassant())
		return;

	const int eps_file = flags.enPassantFile();
	const int8 eps = eps_file + ((color == WHITE)? A5 : A4),
		your_pawn = (color == WHITE)? WHITE_PAWN : BLACK_PAWN,
		opp_pawn = -your_pawn;
	const MoveFlags undoFlags = flags;
	RegMoveNode item;

	// en passant to left
	if (eps_file != 0 && square[eps - 1] == your_pawn) {
		item.move.xindex = pieceIndex(eps, opp_pawn);
		item.move.to = eps + 8 * opp_pawn;
		item.move.from = eps - 1;
		item.move.index = pieceIndex(eps - 1, your_pawn);
		item.move.setEnPassant();

		make(item.move);
		if (!incheck(color)) {
			item.check = incheck(color ^ -2);
			item.score = eval();
			data->list[data->size++] = item;
		}
		unmake(item.move, undoFlags);
	}
	// en passant to right
	if (eps_file != 7 && square[eps + 1] == your_pawn) {
		item.move.xindex = pieceIndex(eps, opp_pawn);
		item.move.to = eps + 8 * opp_pawn;
		item.move.from = eps + 1;
		item.move.index = pieceIndex(eps + 1, your_pawn);
		item.move.setEnPassant();

		make(item.move);
		if (!incheck(color)) {
			item.check = incheck(color ^ -2);
			item.score = eval();
			data->list[data->size++] = item;
		}
		unmake(item.move, undoFlags);
	}
}

RegMoveList* RegBoard::getMoveList(const int8 color, const MoveType movetype)
{
	RegMoveList *data = new RegMoveList;
	data->size = 0;

	switch (movetype) {
	case MoveType::ALL:
	default:
		getMoveList(data, color, movetype);
		getCastleMoveList(data, color);
		getEnPassantMoveList(data, color);
		break;
	case MoveType::CAPTURE:
		getMoveList(data, color, movetype);
		getEnPassantMoveList(data, color);
		break;
	case MoveType::MOVE:
		getMoveList(data, color, movetype);
		getCastleMoveList(data, color);
		break;
	}
	return data;
}

string RegBoard::printPieceList() const
{
	stringstream buff;
	string tmp;

	buff << "White:\t";
	for (int i = 16; i < 32; i++) {
		if (i != 16 && !(i % 8))
			buff << "\n\t";
		buff << pieceSymbol[pieceType[i]] << "(";
		tmp = printLoc(piece[i]);
		if (tmp.length() == 2)
			buff << ' ' << tmp << ' ';
		else
			buff << tmp;
		buff << ") ";
	}
	buff << "\nBlack:\t";
	for (int i = 0; i < 16; i++) {
		if (i && !(i % 8))
			buff << "\n\t";
		buff << pieceSymbol[-pieceType[i]] << "(";
		tmp = printLoc(piece[i]);
		if (tmp.length() == 2)
			buff << ' ' << tmp << ' ';
		else
			buff << tmp;
		buff << ") ";
	}
	return buff.str();
}
