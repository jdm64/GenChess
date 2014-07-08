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

const int regPieceValue[7] = {0, 224, 336, 560, 896, 1456, 0};

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
/* Pawn */	  5,   5,   5,   5,   5,   5,   5,   5,
		 -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,
		  5,   5,  10,  10,  10,  10,   5,   5,
		  5,   5,  10,  10,  10,  10,   5,   5,
		 -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,
		  5,   5,   5,   5,   5,   5,   5,   5,
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
	{	-20, -10,  20,  10,  20,  10,  20, -20,
/* King */	-10, -10,  -5,  -5,  -5,  -5, -10, -10,
		-10,  -5,  10,  10,  10,  10,  -5, -10,
		-10,  -5,  10,  20,  20,  10,  -5, -10,
		-10,  -5,  10,  20,  20,  10,  -5, -10,
		-10,  -5,  10,  10,  10,  10,  -5, -10,
		-10, -10,  -5,  -5,  -5,  -5, -10, -10,
		-20, -10,  20,  10,  20,  10,  20, -20}
	};

template<>
MoveListPool<RegMove>* Board<RegMove>::pool = new MoveListPool<RegMove>;

template<>
void Board<RegMove>::rebuildScore()
{
	int white = 0, black = 0;
	for (int b = 0, w = 16; b < 16; b++, w++) {
		int wloc = EE64F(piece[w]);
		int bloc = EE64F(piece[b]);

		if (piece[b] != DEAD) {
			black += regLocValue[-piecetype[b]][bloc];
			black += regPieceValue[-piecetype[b]];
		} else {
			black -= regPieceValue[-piecetype[b]];
		}
		if (piece[w] != DEAD) {
			white += regLocValue[piecetype[w]][wloc];
			white += regPieceValue[piecetype[w]];
		} else {
			white -= regPieceValue[piecetype[w]];
		}
	}
	mscore = white - black;
}

template<>
void Board<RegMove>::reset()
{
	memset(square, EMPTY, 128);
	copy(InitRegPiece, InitRegPiece + 32, piece);
	copy(InitPieceType, InitPieceType + 32, piecetype);

	for (int i = 0; i < 32; i++)
		square[piece[i]] = piecetype[i];

	key = startHash;

	stm = WHITE;
	ply = 0;
	flags.reset();
	rebuildScore();
}

template<>
void Board<RegMove>::rebuildHash()
{
	key = startHash;
	key ^= (stm == WHITE)? hashBox[WTM_HASH] : 0;

	for (int i = 0; i < 32; i++) {
		if (piece[i] != DEAD)
			key ^= hashBox[12 * EE64(piece[i]) + piecetype[i]];
	}
	key ^= (flags.bits & CAN_EP)? hashBox[ENPASSANT_HASH] : 0;
	key ^= (flags.bits & WK_CASTLE)? hashBox[CASTLE_HASH + WHITE] : 0;
	key ^= (flags.bits & BK_CASTLE)? hashBox[CASTLE_HASH + BLACK] : 0;
	key ^= (flags.bits & WQ_CASTLE)? hashBox[CASTLE_HASH + 2 * WHITE] : 0;
	key ^= (flags.bits & BQ_CASTLE)? hashBox[CASTLE_HASH + 2 * BLACK] : 0;
}

template<>
void Board<RegMove>::setBoard(const RegPosition &pos)
{
	copy(pos.square, pos.square + 128, square);
	copy(pos.piece, pos.piece + 32, piece);
	copy(pos.piecetype, pos.piecetype + 32, piecetype);

	flags = pos.flags;
	ply = pos.ply;
	stm = pos.stm;

	rebuildHash();
	rebuildScore();
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
void Board<RegMove>::make(const RegMove &move)
{
	const int isWhite = (move.index > 15), color = isWhite? WHITE : BLACK;

	key ^= hashBox[13 * EE64(move.from) + piecetype[move.index] + 6];

	if (move.getCastle()) {
		bool left = (move.getCastle() == CASTLE_QS);
		int castleTo = move.to + (left? 1 : -1);
		int castleI = pieceIndex(move.to - (move.to & EP_FILE) + (left? 0 : 7), color * ROOK);

		key ^= hashBox[13 * EE64(piece[castleI]) + piecetype[castleI] + 6];
		key ^= hashBox[13 * EE64(castleTo) + piecetype[castleI] + 6];
		if (flags.canKingCastle(color))
			key ^= hashBox[CASTLE_HASH + color];
		if (flags.canQueenCastle(color))
			key ^= hashBox[CASTLE_HASH + color * 2];

		mscore += stm * regLocValue[ROOK][EE64(castleTo)];
		mscore -= stm * regLocValue[ROOK][EE64(castleI)];
		square[castleTo] = piecetype[castleI];
		square[piece[castleI]] = EMPTY;
		piece[castleI] = castleTo;
		flags.clearCastle(color);
	} else if (ABS(piecetype[move.index]) == ROOK) {
		if (move.from == (isWhite? H1:H8) && flags.canKingCastle(color)) {
			flags.clearKingCastle(color);
			key ^= hashBox[CASTLE_HASH + color];
		} else if (move.from == (isWhite? A1:A8) && flags.canQueenCastle(color)) {
			flags.clearQueenCastle(color);
			key ^= hashBox[CASTLE_HASH + color * 2];
		}
	} else if (ABS(piecetype[move.index]) == KING && flags.canCastle(color)) {
		if (flags.canKingCastle(color))
			key ^= hashBox[CASTLE_HASH + color];
		if (flags.canQueenCastle(color))
			key ^= hashBox[CASTLE_HASH + color * 2];
		flags.clearCastle(color);
	} else if (move.getPromote()) {
		piecetype[move.index] = move.getPromote() * color;
	}
	key ^= hashBox[13 * EE64(move.to) + piecetype[move.index] + 6];

	if (flags.canEnPassant()) {
		flags.clearEnPassant();
		key ^= hashBox[ENPASSANT_HASH];
	}

	// update board information
	square[move.to] = piecetype[move.index];
	mscore += stm * regLocValue[ABS(square[move.to])][EE64F(move.to)];
	mscore -= stm * regLocValue[ABS(square[move.from])][EE64F(move.from)];
	square[move.from] = EMPTY;
	// update piece information
	piece[move.index] = move.to;
	if (move.xindex != NONE) {
		key ^= hashBox[13 * EE64(piece[move.xindex]) + piecetype[move.xindex] + 6];

		mscore += stm * regLocValue[ABS(piecetype[move.xindex])][EE64F(piece[move.xindex])];
		mscore += stm * regPieceValue[ABS(piecetype[move.xindex])];

		if (move.getEnPassant())
			square[piece[move.xindex]] = EMPTY;
		piece[move.xindex] = DEAD;
	} else if (ABS(piecetype[move.index]) == PAWN && ABS(move.to - move.from) == 32) {
		flags.setEnPassant(move.to & EP_FILE);
		key ^= hashBox[ENPASSANT_HASH];
	}
	key ^= hashBox[WTM_HASH];

	stm ^= -2;
	ply++;
}

template<>
void Board<RegMove>::unmake(const RegMove &move, const MoveFlags &undoFlags)
{
	const int isWhite = (move.index > 15), color = isWhite? WHITE : BLACK;

	const int bits = flags.bits ^ undoFlags.bits;
	key ^= (bits & ((color == WHITE)? WK_CASTLE : BK_CASTLE))? hashBox[CASTLE_HASH + color] : 0;
	key ^= (bits & ((color == WHITE)? WQ_CASTLE : BQ_CASTLE))? hashBox[CASTLE_HASH + 2 * color] : 0;
	key ^= (bits & CAN_EP)? hashBox[ENPASSANT_HASH] : 0;
	key ^= hashBox[13 * EE64(move.to) + piecetype[move.index] + 6];

	if (move.getCastle()) {
		bool left = (move.from - move.to > 0);
		int castleFrom = move.to - (move.to & EP_FILE) + (left? 0 : 7);
		int castleI = pieceIndex(move.to + (left? 1 : -1), isWhite? WHITE_ROOK : BLACK_ROOK);

		key ^= hashBox[13 * EE64(piece[castleI]) + piecetype[castleI] + 6];
		key ^= hashBox[13 * EE64(castleFrom) + piecetype[castleI] + 6];

		mscore += stm * regLocValue[ROOK][EE64(castleFrom)];
		mscore -= stm * regLocValue[ROOK][EE64(castleI)];
		square[piece[castleI]] = EMPTY;
		square[castleFrom] = piecetype[castleI];
		piece[castleI] = castleFrom;
	} else if (move.getPromote()) {
		piecetype[move.index] = PAWN * color;
	}
	key ^= hashBox[13 * EE64(move.from) + piecetype[move.index] + 6];

	piece[move.index] = move.from;
	mscore += stm * regLocValue[ABS(square[move.to])][EE64F(move.to)];
	if (move.xindex == NONE) {
		square[move.to] = EMPTY;
	} else {
		if (move.getEnPassant()) {
			piece[move.xindex] = move.to + -16 * color;
			square[piece[move.xindex]] = PAWN * -color;
			square[move.to] = EMPTY;
		} else {
			piece[move.xindex] = move.to;
			square[move.to] = piecetype[move.xindex];
		}
		mscore += stm * regLocValue[ABS(piecetype[move.xindex])][EE64F(piece[move.xindex])];
		mscore += stm * regPieceValue[ABS(piecetype[move.xindex])];

		key ^= hashBox[13 * EE64(piece[move.xindex]) + piecetype[move.xindex] + 6];
	}
	square[move.from] = piecetype[move.index];
	mscore -= stm * regLocValue[ABS(square[move.from])][EE64F(move.from)];

	key ^= hashBox[WTM_HASH];

	flags = undoFlags;
	stm ^= -2;
	ply--;
}

template<>
bool Board<RegMove>::incheckMove(const RegMove &move, const int color, const bool stmCk) const
{
	const int king = (color == WHITE)? 31:15;
	if (stmCk || move.index == king)
		return incheck(color);
	else
		return (attackLine(piece[king], move.from) || attackLine(piece[king], move.to));
}

template<>
bool Board<RegMove>::anyMoves(const int color)
{
	// generate piece moves
	RegMove move;
	const bool stmCk = incheck(color);
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	const MoveFlags undoFlags = flags;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == DEAD)
			continue;

		auto loc = genAll(piece[idx]);
		for (int n = 0; loc[n] != -1; n++) {
			move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			move.to = loc[n];
			move.from = piece[idx];
			move.index = idx;

			// nothing special for promotion
			make(move);
			if (!incheckMove(move, color, stmCk)) {
				unmake(move, undoFlags);
				return true;
			}
			unmake(move, undoFlags);
		}
	}

	// can't castle while in check
	const int king = (color == WHITE)? E1 : E8;

	// King Side
	if (!stmCk && flags.canKingCastle(color) && square[king + 1] == EMPTY &&
	square[king + 2] == EMPTY && !isAttacked(king + 1, color) &&
	!isAttacked(king + 2, color) && ABS(square[((color == WHITE)? H1:H8)]) == ROOK) {
		return true;
	}
	// Queen Side
	if (!stmCk && flags.canQueenCastle(color) && square[king - 1] == EMPTY &&
	square[king - 2] == EMPTY && square[king - 3] == EMPTY &&
	!isAttacked(king - 1, color) && !isAttacked(king - 2, color) &&
	ABS(square[((color == WHITE)? A1:A8)]) == ROOK) {
		return true;
	}

	if (!flags.canEnPassant())
		return false;

	const int eps_file = flags.enPassantFile();
	const int eps = eps_file + ((color == WHITE)? A5 : A4),
		your_pawn = (color == WHITE)? WHITE_PAWN : BLACK_PAWN,
		opp_pawn = your_pawn * -1;

	// en passant to left
	if (eps_file != 0 && square[eps - 1] == your_pawn) {
		move.xindex = pieceIndex(eps, opp_pawn);
		move.to = eps + 8 * opp_pawn;
		move.from = eps - 1;
		move.index = pieceIndex(eps - 1, your_pawn);
		move.setEnPassant();

		make(move);
		if (!incheck(color)) {
			unmake(move, undoFlags);
			return true;
		}
		unmake(move, undoFlags);
	}
	// en passant to right
	if (eps_file != 7 && square[eps + 1] == your_pawn) {
		move.xindex = pieceIndex(eps, opp_pawn);
		move.to = eps + 8 * opp_pawn;
		move.from = eps + 1;
		move.index = pieceIndex(eps + 1, your_pawn);
		move.setEnPassant();

		make(move);
		if (!incheck(color)) {
			unmake(move, undoFlags);
			return true;
		}
		unmake(move, undoFlags);
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
		ep_to = ep + ((color == WHITE)? 16 : -16);

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

	bool ret = true;
	const MoveFlags undoFlags = flags;

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
	const MoveFlags undoFlags = flags;

	make(move);
	// stm is opponent after make
	if (incheck(stm ^ -2))
		ret = IN_CHECK;
	unmake(move, undoFlags);

	return ret;
}

template<>
void Board<RegMove>::getMoveList(RegMoveList *data, const int color, const MoveClass movetype)
{
	const bool stmCk = incheck(color);
	const int start = (color == WHITE)? 31:15, end = (color == WHITE)? 16:0;
	const MoveFlags undoFlags = flags;

	for (int idx = start; idx >= end; idx--) {
		if (piece[idx] == DEAD)
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
			RegMoveNode item;
			item.move.xindex = (square[loc[n]] == EMPTY)? NONE : pieceIndex(loc[n], square[loc[n]]);
			item.move.to = loc[n];
			item.move.from = piece[idx];
			item.move.index = idx;

			if (ABS(piecetype[idx]) == PAWN && isPromote(item.move, color)) {
				item.move.setPromote(QUEEN);

				make(item.move);
				if (incheckMove(item.move, color, stmCk)) {
					unmake(item.move, undoFlags);
					item.move.flags = 0;
					continue;
				}
				unmake(item.move, undoFlags);

				data->add(item);
				for (int i = ROOK; i > PAWN; i--) {
					item.move.setPromote(i);

					make(item.move);
					item.check = incheckMove(item.move, color ^ -2, false);
					item.score = eval();
					unmake(item.move, undoFlags);

					data->add(item);
				}
				item.move.flags = 0;
			} else {
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
		item.check = false;

		data->add(item);
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
		item.check = false;

		data->add(item);
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
	if (eps_file != 7 && square[eps + 1] == your_pawn) {
		item.move.xindex = pieceIndex(eps, opp_pawn);
		item.move.to = eps + 16 * color;
		item.move.from = eps + 1;
		item.move.index = pieceIndex(eps + 1, your_pawn);
		item.move.setEnPassant();

		make(item.move);
		if (!incheck(color)) {
			item.check = incheck(color ^ -2);
			item.score = eval();
			data->add(item);
		}
		unmake(item.move, undoFlags);
	}
	// en passant to right
	if (eps_file != 0 && square[eps - 1] == your_pawn) {
		item.move.xindex = pieceIndex(eps, opp_pawn);
		item.move.to = eps + 16 * color;
		item.move.from = eps - 1;
		item.move.index = pieceIndex(eps - 1, your_pawn);
		item.move.setEnPassant();

		make(item.move);
		if (!incheck(color)) {
			item.check = incheck(color ^ -2);
			item.score = eval();
			data->add(item);
		}
		unmake(item.move, undoFlags);
	}
}

template<>
RegMoveList* Board<RegMove>::getMoveList(const int color, const MoveClass movetype)
{
	RegMoveList* const data = pool->get();

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
