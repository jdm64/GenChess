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

#ifndef __POSITION_H__
#define __POSITION_H__

#include "Move.h"
#include "MoveLookup.h"

#define EE64(x) (((x) >> 1) | ((x) & 7))
#define EE64F(x) (((x) & 7) + ((7 - ((x) >> 4)) << 3))
#define SF88(x) (((x) & ~7) + (x))
#define SFF88(x) (((x) & 7) + ((7 - ((x) >> 3)) << 4))

const int8 stype[20] = {
	EMPTY,          EMPTY,          BLACK_KING,     WHITE_BISHOP,   EMPTY,
	BLACK_KNIGHT,   EMPTY,          BLACK_PAWN,     BLACK_QUEEN,    BLACK_ROOK,
	EMPTY,          EMPTY,          WHITE_KING,     EMPTY,          BLACK_BISHOP,
	WHITE_KNIGHT,   EMPTY,          WHITE_PAWN,     WHITE_QUEEN,    WHITE_ROOK};

const int8 InitRegPiece[32] = {
        A7, B7, C7, D7, E7, F7, G7, H7,
        B8, G8, C8, F8, A8, H8, D8, E8,
        A2, B2, C2, D2, E2, F2, G2, H2,
        B1, G1, C1, F1, A1, H1, D1, E1};

template<class MoveType>
class Position : public MoveLookup<MoveType>
{
protected:
	void parseReset();

	bool setPiece(const int loc, const int type);

	void setMaxPly();

public:
	bool incheck(const int color) const;

	bool parseFen(const string &st);

	bool parseZfen(const string &st);

	string printFen() const;

	string printZfen() const;

	int parseFen_BoardStm(const string &st)
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
				if (!setPiece(SFF88(loc), stype[st[n] % 21]))
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

	int parseZfen_Board(const string &st)
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
				if (!setPiece(SFF88(loc), stype[st[n] % 21]))
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

	void printFen_BoardStm(ostringstream &buf, string &fen) const
	{
		for (int i = 0, empty = 0; i < 64; i++) {
			// convert cordinate system
			int n = SFF88(i);
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
			if (BB::square[n] == EMPTY) {
				empty++;
				continue;
			}
			if (empty) {
				buf.str(string());
				buf << empty;
				fen += buf.str();
				empty = 0;
			}
			if (BB::square[n] > EMPTY)
				fen += pieceSymbol[BB::square[n]];
			else
				fen += tolower(pieceSymbol[-BB::square[n]]);
		}
		fen += ' ';
		fen += (BB::ply % 2)? 'b':'w';
		fen += ' ';
	}

	void printZfen_Board(ostringstream &buf, string &fen) const
	{
		for (int i = 0, empty = 0; i < 64; i++) {
			// convert cordinate system
			int n = SFF88(i);
			if (BB::square[n] == EMPTY) {
				empty++;
				continue;
			} else if (empty) {
				buf.str(string());
				buf << empty;
				fen += buf.str();
			}
			fen += (BB::square[n] > EMPTY)?
					pieceSymbol[BB::square[n]] :
					tolower(pieceSymbol[-BB::square[n]]);
			empty = 0;
		}
		fen += ':';
	}
};

typedef Position<GenMove> GenPosition;
typedef Position<RegMove> RegPosition;

#endif
