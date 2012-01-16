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

template<>
void Board<GenMove>::reset()
{
	memset(square, EMPTY, 64);
	memset(piece, PLACEABLE, 32);
	copy(InitPieceType, InitPieceType + 32, piecetype);
#ifdef TT_ENABLED
	key = startHash;
	key += hashBox[WTM_HASH];
	for (int i = HOLD_START; i < HOLD_END; i++)
		key += hashBox[i];
#endif
	stm = WHITE;
	ply = 0;
}

template<>
Board<GenMove>::Board()
{
	reset();
}

template<>
void Board<GenMove>::rebuildHash()
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

template<>
void Board<GenMove>::setBoard(const GenPosition &pos)
{
	copy(pos.square, pos.square + 64, square);
	copy(pos.piece, pos.piece + 32, piece);

	ply = pos.ply;
	stm = pos.stm;

	rebuildHash();
}

template<>
int Board<GenMove>::pieceIndex(const int loc, const int type) const
{
	static const int offset[] = {-1, 0, 8, 10, 12, 14, 15, 16};
	const int start = ((type < 0)? 0 : 16) + offset[ABS(type)],
		end = ((type < 0)? 0 : 16) + offset[ABS(type) + 1];

	for (int i = start; i < end; i++)
		if (piece[i] == loc)
			return i;
	return NONE;
}

template<>
void Board<GenMove>::make(const GenMove &move)
{
#ifdef DEBUG_MAKE_MOVE
	assert(piecetype[move.index] * stm > 0);
	assert(piece[move.index] == move.from);
	if (move.from != PLACEABLE)
		assert(square[move.from] == piecetype[move.index]);
	if (move.xindex == NONE) {
		assert(square[move.to] == EMPTY);
	} else {
		assert(square[move.to] == piecetype[move.xindex]);
		assert(piece[move.xindex] == move.to);
	}
#endif

	// update board information
	square[move.to] = piecetype[move.index];
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
	assert(piecetype[move.index] * stm < 0);
	assert(piece[move.index] == move.to);
	assert(square[move.to] == piecetype[move.index]);
	if (move.from != PLACEABLE)
		assert(square[move.from] == EMPTY);
	if (move.xindex != NONE)
		assert(piece[move.xindex] == DEAD);
#endif
}

template<>
void Board<GenMove>::unmake(const GenMove &move)
{
#ifdef DEBUG_MAKE_MOVE
	assert(piecetype[move.index] * stm < 0);
	assert(piece[move.index] == move.to);
	assert(square[move.to] == piecetype[move.index]);
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
		square[move.to] = piecetype[move.xindex];
		piece[move.xindex] = move.to;
	}
	if (move.from != PLACEABLE)
		square[move.from] = piecetype[move.index];

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
	assert(piecetype[move.index] * stm > 0);
	assert(piece[move.index] == move.from);
	if (move.from != PLACEABLE)
		assert(square[move.from] == piecetype[move.index]);
	if (move.xindex == NONE) {
		assert(square[move.to] == EMPTY);
	} else {
		assert(square[move.to] == piecetype[move.xindex]);
		assert(piece[move.xindex] == move.to);
	}
#endif
}

template<>
bool Board<GenMove>::anyMoves(const int color)
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

template<>
int Board<GenMove>::isMate()
{
	if (anyMoves(stm))
		return NOTMATE;
	else if (incheck(stm))
		return (stm == WHITE)? BLACK_CHECKMATE : WHITE_CHECKMATE;
	else
		return STALEMATE;
}

template<>
bool Board<GenMove>::validMove(const GenMove &moveIn, GenMove &move)
{
	move = moveIn;

	if ((move.index = pieceIndex(move.from, piecetype[move.index])) == NONE)
		return false;
	if (piecetype[move.index] * stm <= 0)
		return false;
	if (move.xindex != NONE) {
		if ((move.xindex = pieceIndex(move.to, piecetype[move.xindex])) == NONE)
			return false;
	} else if (square[move.to] != EMPTY) {
		return false;
	}

	if (move.from != PLACEABLE && !fromto(move.from, move.to))
			return false;
	if (ply < 2 && ABS(piecetype[move.index]) != KING)
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

template<>
int Board<GenMove>::validMove(const string &smove, const int color, GenMove &move)
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
	if (ply < 2 && ABS(piecetype[move.index]) != KING)
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

template<>
int Board<GenMove>::eval() const
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 16; b++, w++) {
		switch (piece[b]) {
		default:
			black += genLocValue[piecetype[w]][piece[b]];
		case PLACEABLE:
			black += genPieceValue[b];
			break;
		case DEAD:
			black -= genPieceValue[b];
			break;
		}
		switch (piece[w]) {
		default:
			white += genLocValue[piecetype[w]][piece[w]];
		case PLACEABLE:
			white += genPieceValue[b];
			break;
		case DEAD:
			white -= genPieceValue[b];
			break;
		}
	}
	white -= black;
	return (stm == WHITE)? -white : white;
}

template<>
void Board<GenMove>::getPlaceMoveList(GenMoveList* const data, const int pieceType)
{
	const int idx = pieceIndex(PLACEABLE, pieceType);
	const int color = pieceType / ABS(pieceType);
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

template<>
void Board<GenMove>::getMoveList(GenMoveList* const data, const int color, const MoveClass movetype)
{
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	GenMoveNode item;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == PLACEABLE || piece[idx] == DEAD)
			continue;

		int8 *loc;
		switch (movetype) {
		case MoveClass::ALL:
		default:
			loc = genAll(piece[idx]);
			break;
		case MoveClass::CAPTURE:
			loc = genCapture(piece[idx]);
			break;
		case MoveClass::MOVE:
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

template<>
GenMoveList* Board<GenMove>::getMoveList(const int color, const MoveClass movetype)
{
	GenMoveList* const data = new GenMoveList;
	data->size = 0;

	switch (movetype) {
	case MoveClass::ALL:
		if (ply < 2) {
			getPlaceMoveList(data, KING * color);
			break;
		}
		getMoveList(data, color, MoveClass::ALL);
		for (int type = QUEEN; type >= PAWN; type--)
			getPlaceMoveList(data, type * color);
		break;
	case MoveClass::PLACE:
		if (ply < 2) {
			getPlaceMoveList(data, KING * color);
			break;
		}
		for (int type = QUEEN; type >= PAWN; type--)
			getPlaceMoveList(data, type * color);
		break;
	case MoveClass::CAPTURE:
	case MoveClass::MOVE:
		getMoveList(data, color, movetype);
		break;
	}
	return data;
}

template<>
void Board<GenMove>::dumpDebug() const
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

template<>
void Board<RegMove>::reset()
{
	copy(InitRegBoard, InitRegBoard + 64, square);
	copy(InitRegPiece, InitRegPiece + 32, piece);
	copy(InitPieceType, InitPieceType + 32, piecetype);
#ifdef TT_ENABLED
	key = startHash;
#endif
	stm = WHITE;
	ply = 0;
	flags.reset();
}

template<>
Board<RegMove>::Board()
{
	reset();
}

template<>
void Board<RegMove>::rebuildHash()
{
#ifdef TT_ENABLED
	key = startHash;
	key ^= (stm == WHITE)? hashBox[WTM_HASH] : 0;

	for (int i = 0; i < 32; i++) {
		if (piece[i] != DEAD)
			key ^= hashBox[12 * piece[i] + piecetype[i]];
	}
	key ^= (flags.bits & CAN_EP)? hashBox[ENPASSANT_HASH] : 0;
	key ^= (flags.bits & WK_CASTLE)? hashBox[CASTLE_HASH + WHITE] : 0;
	key ^= (flags.bits & BK_CASTLE)? hashBox[CASTLE_HASH + BLACK] : 0;
	key ^= (flags.bits & WQ_CASTLE)? hashBox[CASTLE_HASH + 2 * WHITE] : 0;
	key ^= (flags.bits & BQ_CASTLE)? hashBox[CASTLE_HASH + 2 * BLACK] : 0;
#endif
}

template<>
void Board<RegMove>::setBoard(const RegPosition &pos)
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

template<>
int Board<RegMove>::pieceIndex(const int loc, const int type) const
{
	const int start = (type > 0)? 16:0, end = (type > 0)? 32:16;

	for (int i = start; i < end; i++)
		if (piece[i] == loc && piecetype[i] == type)
			return i;
	return NONE;
}

template<>
void Board<RegMove>::validateBoard(const RegMove &move) const
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
		if (ABS(piecetype[i]) < 1 || ABS(piecetype[i]) > 6) {
			cpt = 4;
			goto error;
		}
		if (square[piece[i]] == piecetype[i])
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
		cerr << (int) piece[i] << "," << (int) piecetype[i] << " ";
	cerr << endl;
	assert(0);
}

template<>
void Board<RegMove>::make(const RegMove &move)
{
	const int isWhite = (move.index > 15), color = isWhite? WHITE : BLACK;
#ifdef TT_ENABLED
	key ^= hashBox[13 * move.from + piecetype[move.index] + 6];
#endif
	if (move.getCastle()) {
		bool left = (move.getCastle() == CASTLE_QS);
		int castleTo = move.to + (left? 1 : -1);
		int castleI = pieceIndex(move.to - (move.to & EP_FILE) + (left? 0 : 7), color * ROOK);
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[castleI] + piecetype[castleI] + 6];
		key ^= hashBox[13 * castleTo + piecetype[castleI] + 6];
		if (flags.canKingCastle(color))
			key ^= hashBox[CASTLE_HASH + color];
		if (flags.canQueenCastle(color))
			key ^= hashBox[CASTLE_HASH + color * 2];
#endif
		square[castleTo] = piecetype[castleI];
		square[piece[castleI]] = EMPTY;
		piece[castleI] = castleTo;
		flags.clearCastle(color);
	} else if (ABS(piecetype[move.index]) == ROOK) {
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
	} else if (ABS(piecetype[move.index]) == KING && flags.canCastle(color)) {
#ifdef TT_ENABLED
		if (flags.canKingCastle(color))
			key ^= hashBox[CASTLE_HASH + color];
		if (flags.canQueenCastle(color))
			key ^= hashBox[CASTLE_HASH + color * 2];
#endif
		flags.clearCastle(color);
	} else if (move.getPromote()) {
		piecetype[move.index] = move.getPromote() * color;
	}
#ifdef TT_ENABLED
	key ^= hashBox[13 * move.to + piecetype[move.index] + 6];
#endif

	if (flags.canEnPassant()) {
		flags.clearEnPassant();
#ifdef TT_ENABLED
		key ^= hashBox[ENPASSANT_HASH];
#endif
	}

	// update board information
	square[move.to] = piecetype[move.index];
	square[move.from] = EMPTY;
	// update piece information
	piece[move.index] = move.to;
	if (move.xindex != NONE) {
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[move.xindex] + piecetype[move.xindex] + 6];
#endif
		if (move.getEnPassant())
			square[piece[move.xindex]] = EMPTY;
		piece[move.xindex] = DEAD;
	} else if (ABS(piecetype[move.index]) == PAWN && ABS(move.to - move.from) == 16) {
		flags.setEnPassant(move.to & EP_FILE);
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

template<>
void Board<RegMove>::unmake(const RegMove &move, const MoveFlags &undoFlags)
{
	const int isWhite = (move.index > 15), color = isWhite? WHITE : BLACK;

#ifdef TT_ENABLED
	const int bits = flags.bits ^ undoFlags.bits;
	key ^= (bits & ((color == WHITE)? WK_CASTLE : BK_CASTLE))? hashBox[CASTLE_HASH + color] : 0;
	key ^= (bits & ((color == WHITE)? WQ_CASTLE : BQ_CASTLE))? hashBox[CASTLE_HASH + 2 * color] : 0;
	key ^= (bits & CAN_EP)? hashBox[ENPASSANT_HASH] : 0;
	key ^= hashBox[13 * move.to + piecetype[move.index] + 6];
#endif

	if (move.getCastle()) {
		bool left = (move.from - move.to > 0);
		int castleFrom = move.to - (move.to & EP_FILE) + (left? 0 : 7);
		int castleI = pieceIndex(move.to + (left? 1 : -1), isWhite? WHITE_ROOK : BLACK_ROOK);
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[castleI] + piecetype[castleI] + 6];
		key ^= hashBox[13 * castleFrom + piecetype[castleI] + 6];
#endif
		square[piece[castleI]] = EMPTY;
		square[castleFrom] = piecetype[castleI];
		piece[castleI] = castleFrom;
	} else if (move.getPromote()) {
		piecetype[move.index] = PAWN * color;
	}
#ifdef TT_ENABLED
	key ^= hashBox[13 * move.from + piecetype[move.index] + 6];
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
			square[move.to] = piecetype[move.xindex];
		}
#ifdef TT_ENABLED
		key ^= hashBox[13 * piece[move.xindex] + piecetype[move.xindex] + 6];
#endif
	}
	square[move.from] = piecetype[move.index];
#ifdef TT_ENABLED
	key ^= hashBox[WTM_HASH];
#endif
	flags = undoFlags;
	stm ^= -2;
	ply--;
}

template<>
bool Board<RegMove>::anyMoves(const int color)
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
	const int eps = eps_file + ((color == WHITE)? A5 : A4),
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

template<>
int Board<RegMove>::isMate()
{
	if (anyMoves(stm))
		return NOTMATE;
	else if (incheck(stm))
		return (stm == WHITE)? BLACK_CHECKMATE : WHITE_CHECKMATE;
	else
		return STALEMATE;
}

template<>
int Board<RegMove>::validCastle(RegMove &move, const int color)
{
	// can we castle on that side
	if (!(flags.canCastle(color) && move.getCastle()))
		return CANT_CASTLE;
	// can't castle while in check
	if (incheck(color))
		return CANT_CASTLE;

	const int king = (color == WHITE)? E1 : E8;

	// king side
	if (move.getCastle() == CASTLE_KS && square[king + 1] == EMPTY && square[king + 2] == EMPTY &&
	!isAttacked(king + 1, color) && !isAttacked(king + 2, color) &&
	ABS(square[((color == WHITE)? H1:H8)]) == ROOK) {
		move.index = (color == WHITE)? 31 : 15;
		move.xindex = NONE;
		move.from = king;
		move.to = king + 2;
		return VALID_MOVE;
	} else if (move.getCastle() == CASTLE_QS && square[king - 1] == EMPTY && square[king - 2] == EMPTY &&
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
template<>
int Board<RegMove>::validEnPassant(RegMove &move, const int color)
{
	const MoveFlags undoFlags = flags;
	const int ep = flags.enPassantFile() + ((color == WHITE)? A5 : A4),
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

template<>
bool Board<RegMove>::validMove(const RegMove &moveIn, RegMove &move)
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

template<>
int Board<RegMove>::validMove(const string &smove, const int color, RegMove &move)
{
	const MoveFlags undoFlags = flags;

	// pre-setup move
	if (!move.parse(smove))
		return INVALID_FORMAT;

	// if castle flag is set, move must a castle to be valid
	if (move.getCastle())
		return validCastle(move, color);

	move.index = pieceIndex(move.from, square[move.from]);

	switch (ABS(piecetype[move.index])) {
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
			move.setCastle((move.from > move.to)? CASTLE_QS : CASTLE_KS);
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

template<>
int Board<RegMove>::eval() const
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 16; b++, w++) {
		if (piece[b] != DEAD) {
			black += regLocValue[-piecetype[b]][piece[b]];
			black += regPieceValue[-piecetype[b]];
		} else {
			black -= regPieceValue[-piecetype[b]];
		}
		if (piece[w] != DEAD) {
			white += regLocValue[piecetype[w]][piece[w]];
			white += regPieceValue[piecetype[w]];
		} else {
			white -= regPieceValue[piecetype[w]];
		}
	}
	white -= black;
	return (stm == WHITE)? -white : white;
}

template<>
void Board<RegMove>::getMoveList(RegMoveList *data, const int color, const MoveClass movetype)
{
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	const MoveFlags undoFlags = flags;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == DEAD)
			continue;

		int8 *loc;
		switch (movetype) {
		case MoveClass::ALL:
		default:
			loc = genAll(piece[idx]);
			break;
		case MoveClass::CAPTURE:
			loc = genCapture(piece[idx]);
			break;
		case MoveClass::MOVE:
			loc = genMove(piece[idx]);
			break;
		}

		for (int n = 0; loc[n] != -1; n++) {
			RegMoveNode item;

			item.move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			item.move.to = loc[n];
			item.move.from = piece[idx];
			item.move.index = idx;

			if (ABS(piecetype[idx]) == PAWN && isPromote(item.move, color)) {
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

template<>
void Board<RegMove>::getCastleMoveList(RegMoveList *data, const int color)
{
	// can't castle while in check
	if (incheck(color))
		return;

	const int king = (color == WHITE)? E1 : E8;
	const int kindex = (color == WHITE)? 31 : 15;
	RegMoveNode item;

	// King Side
	if (flags.canKingCastle(color) && square[king + 1] == EMPTY && square[king + 2] == EMPTY &&
	!isAttacked(king + 1, color) && !isAttacked(king + 2, color) &&
	ABS(square[((color == WHITE)? H1:H8)]) == ROOK) {
		item.move.xindex = NONE;
		item.move.to = king + 2;
		item.move.from = king;
		item.move.index = kindex;
		item.move.setCastle(CASTLE_KS);
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
		item.move.setCastle(CASTLE_QS);
		item.score = eval();
		item.check = incheck(color ^ -2);

		data->list[data->size++] = item;
	}
}

template<>
void Board<RegMove>::getEnPassantMoveList(RegMoveList *data, const int color)
{
	if (!flags.canEnPassant())
		return;

	const int eps_file = flags.enPassantFile();
	const int eps = eps_file + ((color == WHITE)? A5 : A4),
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

template<>
RegMoveList* Board<RegMove>::getMoveList(const int color, const MoveClass movetype)
{
	RegMoveList *data = new RegMoveList;
	data->size = 0;

	switch (movetype) {
	case MoveClass::ALL:
	default:
		getMoveList(data, color, movetype);
		getCastleMoveList(data, color);
		getEnPassantMoveList(data, color);
		break;
	case MoveClass::CAPTURE:
		getMoveList(data, color, movetype);
		getEnPassantMoveList(data, color);
		break;
	case MoveClass::MOVE:
		getMoveList(data, color, movetype);
		getCastleMoveList(data, color);
		break;
	}
	return data;
}
