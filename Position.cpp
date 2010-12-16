#include <sstream>
#include "Position.h"

const char type[] = {
	EMPTY,		EMPTY,		BLACK_KING,	WHITE_BISHOP,	EMPTY,
	BLACK_KNIGHT,	EMPTY,		BLACK_PAWN,	BLACK_QUEEN,	BLACK_ROOK,
	EMPTY,		EMPTY,		WHITE_KING,	EMPTY,		BLACK_BISHOP,
	WHITE_KNIGHT,	EMPTY,		WHITE_PAWN,	WHITE_QUEEN,	WHITE_ROOK};

void Position::reset()
{
	memset(square, EMPTY, 64);
	memset(piece, DEAD, 32);
}

bool Position::setPiece(const char loc, const char type)
{
	static const int offset[] = {-1, 0, 8, 10, 12, 14, 15, 16};
	int start = ((type < 0)? 0 : 16) + offset[abs(type)],
		end = ((type < 0)? 0 : 16) + offset[abs(type) + 1];

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

bool Position::incheck(const char color)
{
	MoveLookup ml(square);
	int king = (color == WHITE)? 31:15;

	return (piece[king] != PLACEABLE)? ml.isAttacked(piece[king]) : false;
}

bool Position::parseFen(string st)
{
	reset();

	// index counter for st
	int i = 0;
	while (isspace(st[i]))
		i++;

	// parse board
	for (int loc = 0, cont = true; cont; i++) {
		switch (st[i]) {
		case 'p':	case 'b':	case 'r':
		case 'n':	case 'q':	case 'k':
		case 'P':	case 'B':	case 'R':
		case 'N':	case 'Q':	case 'K':
			if (!setPiece(loc, type[st[i] % 21]))
				return false;
			loc++;
			break;
		case '1':	case '2':	case '3':
		case '4':	case '5':	case '6':
		case '7':	case '8':
			loc += st[i] - '0';
			break;
		case '/':
			break;
		case ' ':
			cont = false;
			break;
		default:
			return false;
		}
	}
	// pick up color-to-move
	char ctm = (st[i] == 'w')? WHITE : BLACK;
	i += 2;

	// parse placeable pieces
	for (;; i++) {
		if (st[i] == ' ') {
			i++;
			break;
		} else if (st[i] == '-') {
			i += 2;
			break;
		} else if (!isalpha(st[i])) {
			return false;
		} else if (!setPiece(PLACEABLE, type[st[i] % 21])) {
			return false;
		}
	}

	// pick up half-ply count
	string tmp;
	while (isdigit(st[i])) {
		tmp += st[i];
		i++;
	}
	int hply = atoi(tmp.c_str());
	i++;

	// pick up full ply count (starts at 1)
	tmp = "";
	while (isdigit(st[i])) {
		tmp += st[i];
		i++;
	}
	ply = 2 * atoi(tmp.c_str()) - 2 + ((ctm == BLACK)? 1:0);

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
	if (incheck(ctm ^ -2))
		return false;
	return true;
}

bool Position::parseZfen(string st)
{
	reset();

	// index counter for st
	int i = 0;

	// parse board
	string num = "";
	for (int loc = 0, act = false; true; i++) {
		if (isdigit(st[i])) {
			num += st[i];
			act = true;
		} else if (isalpha(st[i])) {
			if (act) {
				loc += atoi(num.c_str());
				num = "";
				act = false;
			}
			if (!setPiece(loc, type[st[i] % 21]))
				return false;
			loc++;
		} else if (st[i] == ':') {
			i++;
			break;
		} else {
			return false;
		}
	}

	// parse placeable pieces
	for (;; i++) {
		if (st[i] == ':') {
			i++;
			break;
		} else if (!isalpha(st[i])) {
			return false;
		} else if (!setPiece(PLACEABLE, type[st[i] % 21])) {
			return false;
		}
	}

	// parse half-ply
	num = "";
	while (isdigit(st[i])) {
		num += st[i];
		i++;
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

	// check if color not on move is in check
	char ctm = (ply % 2)? BLACK : WHITE;
	if (incheck(ctm ^ -2))
		return false;
	return true;
}

string Position::printFen()
{
	ostringstream buf;
	string fen;

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
		if (square[i] == EMPTY) {
			empty++;
			continue;
		}
		if (empty) {
			buf.str(string());
			buf << empty;
			fen += buf.str();
			empty = 0;
		}
		if (square[i] > EMPTY)
			fen += pieceSymbol[square[i]];
		else
			fen += tolower(pieceSymbol[-square[i]]);
	}
	fen += ' ';
	fen += (ply % 2)? 'b':'w';
	fen += ' ';

	for (int i = 0; i < 16; i++) {
		if (piece[i] == PLACEABLE)
			fen += tolower(pieceSymbol[-pieceType[i]]);
	}
	for (int i = 16; i < 32; i++) {
		if (piece[i] == PLACEABLE)
			fen += pieceSymbol[pieceType[i]];
	}
	fen += ' ';
	fen += '0';
	fen += ' ';

	buf.str(string());
	buf << (ply + 2 - ((ply % 2)? 1:0)) / 2;
	fen += buf.str();

	return fen;
}

string Position::printZfen()
{
	stringstream buf;
	string fen;

	for (int i = 0, empty = 0; i < 64; i++) {
		if (square[i] == EMPTY) {
			empty++;
			continue;
		}
		if (empty) {
			buf.str(string());
			buf << empty;
			fen += buf.str();
		}
		if (square[i] > EMPTY)
			fen += pieceSymbol[square[i]];
		else
			fen += tolower(pieceSymbol[-square[i]]);
		empty = 0;
	}
	fen += ':';

	for (int i = 0; i < 16; i++) {
		if (piece[i] == PLACEABLE)
			fen += tolower(pieceSymbol[-pieceType[i]]);
	}
	for (int i = 16; i < 32; i++) {
		if (piece[i] == PLACEABLE)
			fen += pieceSymbol[pieceType[i]];
	}
	fen += ':';

	buf.str(string());
	buf << ply;
	fen += buf.str();;

	return fen;
}