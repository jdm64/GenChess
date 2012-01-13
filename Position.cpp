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

#include <cstring>
#include <iostream>
#include <sstream>
#include "Position.h"

const int8 stype[] = {
	EMPTY,		EMPTY,		BLACK_KING,	WHITE_BISHOP,	EMPTY,
	BLACK_KNIGHT,	EMPTY,		BLACK_PAWN,	BLACK_QUEEN,	BLACK_ROOK,
	EMPTY,		EMPTY,		WHITE_KING,	EMPTY,		BLACK_BISHOP,
	WHITE_KNIGHT,	EMPTY,		WHITE_PAWN,	WHITE_QUEEN,	WHITE_ROOK};

template<>
void Position<GenMove>::parseReset()
{
	memset(square, EMPTY, 64);
	memset(piece, DEAD, 32);
}

template<>
bool Position<GenMove>::setPiece(const int8 loc, const int8 type)
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
bool Position<GenMove>::incheck(const int8 color) const
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
	ply = 2 * atoi(num.c_str()) - 2 + ((stm == BLACK)? 1:0);

	// verify if ply is reasonable
	int mply = 0;
	for (int i = 0; i < 32; i++) {
		switch (piece[i]) {
		case DEAD:
			mply += 2;
			break;
		case PLACEABLE:
			break;
		default:
			mply += 1;
			break;
		}
	}
	if (ply < mply)
		return false;

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

	int mply = 0;
	for (int i = 0; i < 32; i++) {
		switch (piece[i]) {
		case DEAD:
			mply += 2;
			break;
		case PLACEABLE:
			break;
		default:
			mply += 1;
			break;
		}
	}
	if (ply < mply)
		return false;

	stm = (ply % 2)? BLACK : WHITE;

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

// --- Start Regular Chess ---

template<>
void Position<RegMove>::parseReset()
{
	memset(square, EMPTY, 64);
	memset(piece, DEAD, 32);
	copy(InitPieceType, InitPieceType + 32, piecetype);
	flags.reset();
}

template<>
bool Position<RegMove>::setPiece(const int8 loc, const int8 type)
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
bool Position<RegMove>::incheck(const int8 color) const
{
	const int king = (color == WHITE)? 31:15;

	return isAttacked(piece[king], color);
}

template<>
bool Position<RegMove>::parseFen(const string &st)
{
	int n = parseFen_BoardStm(st);

	// check if board parsing failed
	if (n <= 0)
		return false;

	// castle rights
	int8 castle = 0;
	for (; st[n] != ' '; n++) {
		switch (st[n]) {
		case 'K':
			castle |= 0x10;
			break;
		case 'Q':
			castle |= 0x20;
			break;
		case 'k':
			castle |= 0x40;
			break;
		case 'q':
			castle |= 0x80;
			break;
		case '-':
			break;
		}
	}
	flags.setCastle(castle);

	// en passant square
	n++;
	if (st[n] != '-') {
		int eps = st[n++] - 'a';
		eps += 8 * (8 - (st[n++] - '0'));
		flags.setEnPassant(eps & 0x7);
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
	int tply = atoi(num.c_str());
	if (tply < 1)
		tply = 1;
	ply = 2 * tply - 2 + ((stm == BLACK)? 1:0);

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
	int8 castle = 0;
	for (; st[n] != ':'; n++) {
		switch (st[n]) {
		case 'K':
			castle |= 0x10;
			break;
		case 'Q':
			castle |= 0x20;
			break;
		case 'k':
			castle |= 0x40;
			break;
		case 'q':
			castle |= 0x80;
			break;
		}
	}
	flags.setCastle(castle);

	// parse en passant
	n++;
	if (st[n] != ':') {
		int eps = st[n++] - 'a';
		eps += 8 * (8 - (st[n++] - '0'));
		flags.setEnPassant(eps & 0x7);
	}
	n++;

	// parse half-ply
	string num = "";
	while (isdigit(st[n])) {
		num += st[n];
		n++;
	}
	int tply = atoi(num.c_str());
	ply = (tply >= 0)? tply:0;

	// check if color not on move is in check
	stm = (ply % 2)? BLACK : WHITE;
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

template<class MoveType>
int Position<MoveType>::parseFen_BoardStm(const string &st)
{
	parseReset();

	// index counter for st
	int n = 0;
	while (isspace(st[n]))
		n++;

	// parse board
	for (int loc = 0, cont = true; cont; n++) {
		switch (st[n]) {
		case 'p':	case 'b':	case 'r':
		case 'n':	case 'q':	case 'k':
		case 'P':	case 'B':	case 'R':
		case 'N':	case 'Q':	case 'K':
			if (!setPiece(loc, stype[st[n] % 21]))
				return -1;
			loc++;
			break;
		case '1':	case '2':	case '3':
		case '4':	case '5':	case '6':
		case '7':	case '8':
			loc += st[n] - '0';
			break;
		case '/':
			break;
		case ' ':
			cont = false;
			break;
		default:
			return -1;
		}
	}
	// pick up color-to-move
	BB::stm = (st[n] == 'w')? WHITE : BLACK;
	n += 2;

	return n;
}

template<class MoveType>
int Position<MoveType>::parseZfen_Board(const string &st)
{
	parseReset();

	// index counter for st
	int n = 0;

	// parse board
	string num = "";
	for (int loc = 0, act = false; true; n++) {
		if (isdigit(st[n])) {
			num += st[n];
			act = true;
		} else if (isalpha(st[n])) {
			if (act) {
				loc += atoi(num.c_str());
				num = "";
				act = false;
			}
			if (!setPiece(loc, stype[st[n] % 21]))
				return -1;
			loc++;
		} else if (st[n] == ':') {
			n++;
			break;
		} else {
			return -1;
		}
	}
	return n;
}

template<class MoveType>
void Position<MoveType>::printFen_BoardStm(ostringstream &buf, string &fen) const
{
	for (int i = 0, empty = 0; i < 65; i++) {
		if (i && i % 8 == 0) {
			if (empty) {
				buf.str(string());
				buf << empty;
				fen += buf.str();
			}
			if (i == 64)
				break;
			empty = 0;
			fen += '/';
		}
		if (BB::square[i] == EMPTY) {
			empty++;
			continue;
		}
		if (empty) {
			buf.str(string());
			buf << empty;
			fen += buf.str();
			empty = 0;
		}
		if (BB::square[i] > EMPTY)
			fen += pieceSymbol[BB::square[i]];
		else
			fen += tolower(pieceSymbol[-BB::square[i]]);
	}
	fen += ' ';
	fen += (BB::ply % 2)? 'b':'w';
	fen += ' ';
}

template<class MoveType>
void Position<MoveType>::printZfen_Board(ostringstream &buf, string &fen) const
{
	for (int i = 0, empty = 0; i < 64; i++) {
		if (BB::square[i] == EMPTY) {
			empty++;
			continue;
		} else if (empty) {
			buf.str(string());
			buf << empty;
			fen += buf.str();
		}
		fen += (BB::square[i] > EMPTY)?
				pieceSymbol[BB::square[i]] :
				tolower(pieceSymbol[-BB::square[i]]);
		empty = 0;
	}
	fen += ':';
}
