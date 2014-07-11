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
#include <sstream>
#include "Position.h"

template<>
void Position<GenMove>::parseReset()
{
	memset(square, EMPTY, 128);
	memset(piece, DEAD, 32);
}

template<>
void Position<GenMove>::setMaxPly()
{
	int tply = 0;
	for (int i = 0; i < 32; i++) {
		if (piece[i] == DEAD)
			tply += 2;
		else if (piece[i] != PLACEABLE)
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
bool Position<GenMove>::setPiece(const int loc, const int type)
{
	static const int offset[] = {-1, 0, 8, 10, 12, 14, 15, 16};
	const int start = ((type < 0)? 0 : 16) + offset[ABS(type)],
		end = ((type < 0)? 0 : 16) + offset[ABS(type) + 1];

	for (int i = start; i < end; i++) {
		if (piece[i] == DEAD) {
			piece[i] = loc;
			if (loc != PLACEABLE)
				square[loc] = type;
			return true;
		}
	}
	return false;
}

template<>
bool Position<GenMove>::incheck(const int color) const
{
	const int king = (color == WHITE)? 31:15;

	return (piece[king] != PLACEABLE)? isAttacked(piece[king]) : false;
}

template<>
bool Position<GenMove>::parseFen(const string &st)
{
	int n = parseFen_BoardStm(st);

	// check if board parsing failed
	if (n <= 0)
		return false;

	// parse placeable pieces
	for (;; n++) {
		if (st[n] == ' ') {
			n++;
			break;
		} else if (st[n] == '-') {
			n += 2;
			break;
		} else if (!isalpha(st[n])) {
			return false;
		} else if (!setPiece(PLACEABLE, stype[st[n] % 21])) {
			return false;
		}
	}

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
bool Position<GenMove>::parseZfen(const string &st)
{
	int n = parseZfen_Board(st);

	// check if board parsing failed
	if (n <= 0)
		return false;

	// parse placeable pieces
	for (;; n++) {
		if (st[n] == ':') {
			n++;
			break;
		} else if (!isalpha(st[n])) {
			return false;
		} else if (!setPiece(PLACEABLE, stype[st[n] % 21])) {
			return false;
		}
	}

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
string Position<GenMove>::printFen() const
{
	ostringstream buf;
	string fen;

	printFen_BoardStm(buf, fen);

	for (int i = 0; i < 16; i++) {
		if (piece[i] == PLACEABLE)
			fen += tolower(pieceSymbol[-piecetype[i]]);
	}
	for (int i = 16; i < 32; i++) {
		if (piece[i] == PLACEABLE)
			fen += pieceSymbol[piecetype[i]];
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
string Position<GenMove>::printZfen() const
{
	ostringstream buf;
	string fen;

	printZfen_Board(buf, fen);

	for (int i = 0; i < 16; i++) {
		if (piece[i] == PLACEABLE)
			fen += tolower(pieceSymbol[-piecetype[i]]);
	}
	for (int i = 16; i < 32; i++) {
		if (piece[i] == PLACEABLE)
			fen += pieceSymbol[piecetype[i]];
	}
	fen += ':';

	buf.str(string());
	buf << ply;
	fen += buf.str();

	return fen;
}
