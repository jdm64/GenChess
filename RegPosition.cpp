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

#include <cstring>
#include <iostream>
#include <sstream>
#include "Position.h"

template<>
void Position<RegMove>::parseReset()
{
	memset(square, EMPTY, 128);
	memset(piece, DEAD, 32);
	copy(InitPieceType, InitPieceType + 32, piecetype);
	flags.reset();
}

template<>
void Position<RegMove>::setMaxPly()
{
	int tply = 0;
	for (int i = 0; i < 32; i++) {
		if (piece[i] == DEAD)
			tply += 2;
		else if (piece[i] != InitRegPiece[i])
			tply++;
	}
	ply = max(ply, tply);

	if (stm == WHITE) {
		if (ply % 2)
			ply++;
	} else if (ply % 2 == 0) {
		ply++;
	}
}

template<>
bool Position<RegMove>::setPiece(const int loc, const int type)
{
	static const int offset[] = {-1, 0, 8, 10, 12, 14, 15, 16};
	const int start = ((type < 0)? 0 : 16) + offset[ABS(type)],
		end = ((type < 0)? 0 : 16) + offset[ABS(type) + 1];

	// first try for setting non promoted pieces
	for (int i = start; i < end; i++) {
		if (piece[i] == DEAD) {
			piece[i] = loc;
			square[loc] = type;
			return true;
		}
	}

	// piece might be a promote
	if (ABS(type) == PAWN || ABS(type) == KING)
		return false;

	const int pstart = (type > 0)? 16:0, pend = (type > 0)? 24:8;
	for (int i = pstart; i < pend; i++) {
		if (piece[i] == DEAD) {
			piece[i] = loc;
			piecetype[i] = type;
			square[loc] = type;
			return true;
		}
	}
	return false;
}

template<>
bool Position<RegMove>::incheck(const int color) const
{
	const int king = (color == WHITE)? 31:15;
	return isAttacked(piece[king]);
}

template<>
bool Position<RegMove>::parseFen(const string &st)
{
	int n = parseFen_BoardStm(st);

	// check if board parsing failed
	if (n <= 0)
		return false;

	// castle rights
	int castle = 0;
	for (; st[n] != ' '; n++) {
		switch (st[n]) {
		case 'K':
			castle |= WK_CASTLE;
			break;
		case 'Q':
			castle |= WQ_CASTLE;
			break;
		case 'k':
			castle |= BK_CASTLE;
			break;
		case 'q':
			castle |= BQ_CASTLE;
			break;
		case '-':
			break;
		}
	}
	flags.setCastle(castle);

	// en passant square
	n++;
	if (st[n] != '-') {
		int eps = (st[n++] - 'a');
		eps += 16 * (st[n++] - '1');
		flags.setEnPassant(eps & EP_FILE);
	}
	n++;

	// pick up half-ply count
	string num;
	while (isdigit(st[n])) {
		num += st[n];
		n++;
	}
	ply = atoi(num.c_str());
	n++;

	// pick up full ply count (starts at 1)
	num = "";
	while (isdigit(st[n])) {
		num += st[n];
		n++;
	}
	ply = atoi(num.c_str());
	ply = 2 * ply - 2 + ((stm == BLACK)? 1:0);

	setMaxPly();

	// check if color not on move is in check
	if (incheck(stm ^ -2))
		return false;
	return true;
}

template<>
bool Position<RegMove>::parseZfen(const string &st)
{
	int n = parseZfen_Board(st);

	// check if board parsing failed
	if (n <= 0)
		return false;

	// parse castle rights
	int castle = 0;
	for (; st[n] != ':'; n++) {
		switch (st[n]) {
		case 'K':
			castle |= WK_CASTLE;
			break;
		case 'Q':
			castle |= WQ_CASTLE;
			break;
		case 'k':
			castle |= BK_CASTLE;
			break;
		case 'q':
			castle |= BQ_CASTLE;
			break;
		}
	}
	flags.setCastle(castle);

	// parse en passant
	n++;
	if (st[n] != ':') {
		int eps = (st[n++] - 'a');
		eps += 16 * (st[n++] - '1');
		flags.setEnPassant(eps & EP_FILE);
	}
	n++;

	// parse half-ply
	string num = "";
	while (isdigit(st[n])) {
		num += st[n];
		n++;
	}
	ply = atoi(num.c_str());
	stm = (ply % 2)? BLACK : WHITE;

	setMaxPly();

	// check if color not on move is in check
	if (incheck(stm ^ -2))
		return false;
	return true;
}

template<>
string Position<RegMove>::printFen() const
{
	ostringstream buf;
	string fen;

	printFen_BoardStm(buf, fen);

	// print castle rights
	if (flags.bits & 0xf0) {
		if (flags.canKingCastle(WHITE))
			fen += 'K';
		if (flags.canQueenCastle(WHITE))
			fen += 'Q';
		if (flags.canKingCastle(BLACK))
			fen += 'k';
		if (flags.canQueenCastle(BLACK))
			fen += 'q';
	} else {
		fen += '-';
	}
	fen += ' ';

	if (flags.canEnPassant()) {
		fen += 'a' + flags.enPassantFile();
		fen += (ply % 2)? '3':'6';
	} else {
		fen += '-';
	}
	fen += ' ';
	fen += '0';
	fen += ' ';

	buf.str(string());
	buf << (ply + 2 - ((ply % 2)? 1:0)) / 2;
	fen += buf.str();

	return fen;
}

template<>
string Position<RegMove>::printZfen() const
{
	ostringstream buf;
	string fen;

	printZfen_Board(buf, fen);

	// print castle rights
	if (flags.bits & 0xf0) {
		if (flags.canKingCastle(WHITE))
			fen += 'K';
		if (flags.canQueenCastle(WHITE))
			fen += 'Q';
		if (flags.canKingCastle(BLACK))
			fen += 'k';
		if (flags.canQueenCastle(BLACK))
			fen += 'q';
	}
	fen += ':';

	if (flags.canEnPassant()) {
		fen += 'a' + flags.enPassantFile();
		fen += (ply % 2)? '3':'6';
	}
	fen += ':';

	buf.str(string());
	buf << ply;
	fen += buf.str();

	return fen;
}
