#include "MoveLookup.h"

char MoveLookup::mailbox[120] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
	-1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
	-1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
	-1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
	-1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
	-1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
	-1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
	-1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

char MoveLookup::mailbox64[64] = {
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98};

char MoveLookup::offsets[7][8] = {
	{  0,   0,   0,  0,   0,  0,  0,  0},
	{-11,  -9,   9, 11, -10, -1,  1, 10},
	{-21, -19, -12, -8,   8, 12, 19, 21},
	{-11,  -9,   9, 11,   0,  0,  0,  0},
	{-10,  -1,   1, 10,   0,  0,  0,  0},
	{-11, -10,  -9, -1,   1,  9, 10, 11},
	{-11, -10,  -9, -1,   1,  9, 10, 11} };

char* MoveLookup::genAll(const char &from) const
{
	int type = abs(board[from]), next = 0, mfrom = mailbox64[from];
	char to, *list = new char[28], *offset = offsets[type];

	switch (type) {
	case PAWN:
		// captures
		for (int dir = 0; dir < 4; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(board[from], board[to]))
				list[next++] = to;
		}
		// moves
		for (int dir = 4; dir < 8; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (EMPTY_MOVE(board[from], board[to]))
				list[next++] = to;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (ANY_MOVE(board[from], board[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (EMPTY_MOVE(board[from], board[to])) {
					list[next++] = to;
					continue;
				} else if (CAPTURE_MOVE(board[from], board[to])) {
					list[next++] = to;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (EMPTY_MOVE(board[from], board[to])) {
					list[next++] = to;
					continue;
				} else if (CAPTURE_MOVE(board[from], board[to])) {
					list[next++] = to;
				}
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	list[next] = -1;
	return list;
}

char* MoveLookup::genCapture(const char &from) const
{
	int type = abs(board[from]), next = 0, mfrom = mailbox64[from];
	char to, *list = new char[28], *offset = offsets[type];

	switch (type) {
	case PAWN:
		// captures
		for (int dir = 0; dir < 4; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(board[from], board[to]))
				list[next++] = to;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (CAPTURE_MOVE(board[from], board[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1)
					break;
				else if (EMPTY_MOVE(board[from], board[to]))
					continue;
				else if (CAPTURE_MOVE(board[from], board[to]))
					list[next++] = to;
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1)
					break;
				else if (EMPTY_MOVE(board[from], board[to]))
					continue;
				else if (CAPTURE_MOVE(board[from], board[to]))
					list[next++] = to;
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	list[next] = -1;
	return list;
}

char* MoveLookup::genMove(const char &from) const
{
	int type = abs(board[from]), next = 0, mfrom = mailbox64[from];
	char to, *list = new char[28], *offset = offsets[type];

	switch (type) {
	case PAWN:
		// moves
		for (int dir = 4; dir < 8; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (EMPTY_MOVE(board[from], board[to]))
				list[next++] = to;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (EMPTY_MOVE(board[from], board[to]))
				list[next++] = to;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (EMPTY_MOVE(board[from], board[to])) {
					list[next++] = to;
					continue;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (EMPTY_MOVE(board[from], board[to])) {
					list[next++] = to;
					continue;
				}
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	list[next] = -1;
	return list;
}

bool MoveLookup::fromto(const char &From, const char &To) const
{
	int type = abs(board[From]), mfrom = mailbox64[From];
	char to, *offset = offsets[type];

	switch (type) {
	case PAWN:
		// captures
		for (int dir = 0; dir < 4; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (ANY_MOVE(board[From], board[to]) && to == To)
				return true;
		}
		// moves
		for (int dir = 4; dir < 8; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (EMPTY_MOVE(board[From], board[to]) && to == To)
				return true;
		}
		break;
	case KNIGHT:
	case KING:
		for (int dir = 0; dir < 8; dir++) {
			to = mailbox[mfrom + offset[dir]];
			if (to == -1)
				continue;
			else if (ANY_MOVE(board[From], board[to]) && to == To)
				return true;
		}
		break;
	case BISHOP:
	case ROOK:
		for (int dir = 0; dir < 4; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (EMPTY_MOVE(board[From], board[to])) {
					if (to == To)
						return true;
					continue;
				} else if (CAPTURE_MOVE(board[From], board[to])) {
					if (to == To)
						return true;
				}
				break;
			}
		}
		break;
	case QUEEN:
		for (int dir = 0; dir < 8; dir++) {
			for (int k = 1; k < 8; k++) {
				to = mailbox[mfrom + k * offset[dir]];
				if (to == -1) {
					break;
				} else if (EMPTY_MOVE(board[From], board[to])) {
					if (to == To)
						return true;
					continue;
				} else if (CAPTURE_MOVE(board[From], board[to])) {
					if (to == To)
						return true;
				}
				break;
			}
		}
		break;
	default:
		assert(0);
	}
	return false;
}

bool MoveLookup::isAttacked(const char &from) const
{
	int mfrom = mailbox64[from];
	char to, *offset;

	offset = offsets[ROOK];
	for (int dir = 0; dir < 4; dir++) {
		for (int k = 1; k < 8; k++) {
			to = mailbox[mfrom + k * offset[dir]];
			if (to == -1)
				break;
			else if (EMPTY_MOVE(board[from], board[to]))
				continue;
			else if (OWN_PIECE(board[from], board[to]))
				break;
			else if (abs(board[to]) == ROOK || abs(board[to]) == QUEEN)
				return true;
			else if (k == 1 && abs(board[to]) == KING)
				return true;
			break;
		}
	}

	offset = offsets[BISHOP];
	for (int dir = 0; dir < 4; dir++) {
		for (int k = 1; k < 8; k++) {
			to = mailbox[mfrom + k * offset[dir]];
			if (to == -1)
				break;
			else if (EMPTY_MOVE(board[from], board[to]))
				continue;
			else if (OWN_PIECE(board[from], board[to]))
				break;
			else if (abs(board[to]) == BISHOP || abs(board[to]) == QUEEN)
				return true;
			else if (k == 1 && (abs(board[to]) == PAWN || abs(board[to]) == KING))
				return true;
			break;
		}
	}

	offset = offsets[KNIGHT];
	for (int dir = 0; dir < 8; dir++) {
		to = mailbox[mfrom + offset[dir]];
		if (to == -1)
			continue;
		else if (NOT_CAPTURE(board[from], board[to]))
			continue;
		else if (abs(board[to]) == KNIGHT)
			return true;
	}
	return false;
}
