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

#include <iostream>
#include <cstring>
#include "Board.h"
#include "TransTable.h"

const int genPieceValue[7] = {0, 224, 336, 560, 896, 1456, 0};

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
MoveListPool<GenMove>* Board<GenMove>::pool = new MoveListPool<GenMove>;

template<>
void Board<GenMove>::rebuildScore()
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 16; b++, w++) {
		int wloc = EE64F(piece[w]);
		int bloc = EE64F(piece[b]);

		switch (piece[b]) {
		default:
			black += genLocValue[-piecetype[b]][bloc];
		case PLACEABLE:
			black += genPieceValue[-piecetype[b]];
			break;
		case DEAD:
			black -= genPieceValue[-piecetype[b]];
			break;
		}
		switch (piece[w]) {
		default:
			white += genLocValue[piecetype[w]][wloc];
		case PLACEABLE:
			white += genPieceValue[piecetype[w]];
			break;
		case DEAD:
			white -= genPieceValue[piecetype[w]];
			break;
		}
	}
	mscore = white - black;
}

template<>
void Board<GenMove>::reset()
{
	memset(square, EMPTY, 128);
	memset(piece, PLACEABLE, 32);
	copy(InitPieceType, InitPieceType + 32, piecetype);

	key = startHash;
	key += hashBox[WTM_HASH];
	for (int i = HOLD_START; i < HOLD_END; i++)
		key += hashBox[i];

	stm = WHITE;
	ply = 0;
	rebuildScore();
}

template<>
void Board<GenMove>::rebuildHash()
{
	key = startHash;
	key += (stm == WHITE)? hashBox[WTM_HASH] : 0;

	for (int i = 0; i < 32; i++) {
		switch (piece[i]) {
		default:
			key -= hashBox[HOLD_START + typeLookup[i]];
			key += hashBox[12 * EE64(piece[i]) + typeLookup[i]];
			break;
		case DEAD:
			key -= hashBox[HOLD_START + typeLookup[i]];
			break;
		case PLACEABLE:
			break;
		}
	}
}

template<>
void Board<GenMove>::setBoard(const GenPosition &pos)
{
	copy(pos.square, pos.square + 128, square);
	copy(pos.piece, pos.piece + 32, piece);

	ply = pos.ply;
	stm = pos.stm;

	rebuildHash();
	rebuildScore();
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
	// update board information
	square[move.to] = piecetype[move.index];
	mscore += stm * genLocValue[ABS(square[move.to])][EE64F(move.to)];
	if (move.from != PLACEABLE) {
		mscore -= stm * genLocValue[ABS(square[move.from])][EE64F(move.from)];
		square[move.from] = EMPTY;
	}
	// update piece information
	piece[move.index] = move.to;
	if (move.xindex != NONE) {
		mscore += stm * genLocValue[ABS(piecetype[move.xindex])][EE64F(move.to)];
		mscore += stm * genPieceValue[ABS(piecetype[move.xindex])];
		piece[move.xindex] = DEAD;
	}

	int to = EE64(move.to);
	int from = EE64(move.from);

	key += (stm == WHITE)? -hashBox[WTM_HASH] : hashBox[WTM_HASH];
	key += hashBox[12 * to + typeLookup[move.index]];
	if (move.from != PLACEABLE)
		key -= hashBox[12 * from + typeLookup[move.index]];
	else
		key -= hashBox[HOLD_START + typeLookup[move.index]];
	if (move.xindex != NONE)
		key -= hashBox[12 * to + typeLookup[move.xindex]];

	stm ^= -2;
	ply++;
}

template<>
void Board<GenMove>::unmake(const GenMove &move, const MoveFlags &undoFlags)
{
	piece[move.index] = move.from;
	mscore += stm * genLocValue[ABS(square[move.to])][EE64F(move.to)];
	if (move.xindex == NONE) {
		square[move.to] = EMPTY;
	} else {
		square[move.to] = piecetype[move.xindex];
		piece[move.xindex] = move.to;
		mscore += stm * genLocValue[ABS(piecetype[move.xindex])][EE64F(move.to)];
		mscore += stm * genPieceValue[ABS(piecetype[move.xindex])];
	}
	if (move.from != PLACEABLE) {
		square[move.from] = piecetype[move.index];
		mscore -= stm * genLocValue[ABS(square[move.from])][EE64F(move.from)];
	}

	int to = EE64(move.to);
	int from = EE64(move.from);

	key += (stm == WHITE)? -hashBox[WTM_HASH] : hashBox[WTM_HASH];
	key -= hashBox[12 * to + typeLookup[move.index]];
	if (move.from != PLACEABLE)
		key += hashBox[12 * from + typeLookup[move.index]];
	else
		key += hashBox[HOLD_START + typeLookup[move.index]];
	if (move.xindex != NONE)
		key += hashBox[12 * to + typeLookup[move.xindex]];

	stm ^= -2;
	ply--;
}


template<>
bool Board<GenMove>::anyMoves(const int color)
{
	// shortest games takes 5 half-moves
	if (ply < 4)
		return true;

	// generate piece moves
	GenMove move;
	const bool stmCk = incheck(color);
	const int start = (color == BLACK)? 0:16, end = (color == BLACK)? 16:32;
	const MoveFlags undoFlags = flags;

	for (int idx = start; idx < end; idx++) {
		if (piece[idx] == PLACEABLE || piece[idx] == DEAD)
			continue;

		auto loc = genAll(piece[idx]);
		for (int n = 0; loc[n] != -1; n++) {
			move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			move.to = loc[n];
			move.from = piece[idx];
			move.index = idx;

			make(move);
			if (!incheckMove(move, color, stmCk)) {
				unmake(move, undoFlags);
				return true;
			}
			unmake(move, undoFlags);
		}
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
			if (!incheckMove(move, color, stmCk) && !incheckMove(move, color ^ -2, false)) {
				unmake(move, undoFlags);
				return true;
			}
			unmake(move, undoFlags);
		}
	}
	return false;
}

template<>
bool Board<GenMove>::validMove(const GenMove &moveIn, GenMove &move)
{
	move = moveIn;
	move.index = pieceIndex(move.from, piecetype[move.index]);
	if (move.index == NONE)
		return false;
	if (piecetype[move.index] * stm <= 0)
		return false;
	if (move.xindex != NONE) {
		move.xindex = pieceIndex(move.to, piecetype[move.xindex]);
		if (move.xindex == NONE)
			return false;
	} else if (square[move.to] != EMPTY) {
		return false;
	}

	if (move.from != PLACEABLE && !fromto(move.from, move.to))
			return false;
	if (ply < 2 && ABS(piecetype[move.index]) != KING)
		return false;

	bool ret = true;
	const MoveFlags undoFlags = flags;

	make(move);
	// stm is opponent after make
	if (incheck(stm ^ -2))
		ret = false;
	if (move.from == PLACEABLE && incheck(stm))
		ret = false;
	unmake(move, undoFlags);

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
	const MoveFlags undoFlags = flags;

	make(move);
	// stm is opponent after make
	if (incheck(stm ^ -2))
		ret = IN_CHECK;
	if (move.from == PLACEABLE && incheck(stm))
		ret = IN_CHECK_PLACE;
	unmake(move, undoFlags);

	return ret;
}

template<>
void Board<GenMove>::getPlaceMoveList(GenMoveList* const data, const int pieceType)
{
	const int idx = pieceIndex(PLACEABLE, pieceType);
	const int color = pieceType / ABS(pieceType);
	const MoveFlags undoFlags = flags;
	GenMoveNode item;

	if (idx == NONE)
		return;

	bool stmCk = incheck(color);
	for (int loc = 0x77; loc >= 0; loc--) {
		if (loc & 0x88) {
			loc -= 7;
			continue;
		} else if (square[loc] != EMPTY) {
			continue;
		}
		item.move.index = idx;
		item.move.to = loc;
		item.move.xindex = NONE;
		item.move.from = PLACEABLE;

		make(item.move);
		// place moves are only valid if neither side is inCheck
		if (!incheckMove(item.move, color, stmCk) && !incheckMove(item.move, color ^ -2, false)) {
			// item.check initialized to false
			item.score = eval();
			data->add(item);
		}
		unmake(item.move, undoFlags);
	}
}

template<>
void Board<GenMove>::getMoveList(GenMoveList* const data, const int color, const MoveClass movetype)
{
	const bool stmCk = incheck(color);
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	const MoveFlags undoFlags = flags;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == PLACEABLE || piece[idx] == DEAD)
			continue;

		array<int8,28> loc;
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
			GenMoveNode item;
			item.move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			item.move.to = loc[n];
			item.move.from = piece[idx];
			item.move.index = idx;

			make(item.move);
			if (!incheckMove(item.move, color, stmCk)) {
				item.check = incheckMove(item.move, color ^ -2, false);
				item.score = eval();
				data->add(item);
			}
			unmake(item.move, undoFlags);
		}
	}
}

template<>
GenMoveList* Board<GenMove>::getMoveList(const int color, const MoveClass movetype)
{
	GenMoveList* const data = pool->get();

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
