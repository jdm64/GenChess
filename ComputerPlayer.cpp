/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      ComputerPlayer class functions
 */

#include "ComputerPlayer.h"

#define DEBUG_SCORES
#define RANDOM_MOVE_ORDER

void ComputerPlayer::printList(MoveList *ptr)
{
	for (int i = 0; i < ptr->size; i++)
		cout << board->printMove(ptr->list[i].move) << "[" <<
			ptr->list[i].score << "] ";
}

void ComputerPlayer::getKillerMoves(MoveList *ptr, int depth)
{
	for (int i = 1, n = 0; i < ptr->size; i++) {
		if (ptr->list[i].move == killer1[depth])
			swap(ptr->list[i], ptr->list[n++]);
		else if (ptr->list[i].move == killer2[depth])
			swap(ptr->list[i], ptr->list[n++]);
		else if (n >= 2)
			return;
	}
}

void ComputerPlayer::setKillerMoves(Move move, int depth)
{
	if (move == killer1[depth])
		return;
	else if (move == killer2[depth])
		return;
	killer2[depth] = killer1[depth];
	killer1[depth] = move;
}

void ComputerPlayer::pickMove(MoveList *ptr)
{
	stable_sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpHiLow);
#ifdef RANDOM_MOVE_ORDER
	int score = ptr->list[0].score;
	vector<int> match(1, 0);
	for (int i = 1; i < ptr->size; i++) {
		if (ptr->list[i].score != score)
			break;
		match.push_back(i);
	}
	int x = rand() % match.size();
	swap(ptr->list[0], ptr->list[match[x]]);
#endif
}

int ComputerPlayer::getMaxScore(MoveList *ptr)
{
	int max = ptr->list[0].score;

	for (int i = 1; i < ptr->size; i++) {
		if (ptr->list[i].score > max)
			max = ptr->list[i].score;
	}
	return max;
}

int ComputerPlayer::Quiescence(MoveList *&ptr, int alpha, int beta, int depth)
{
	if (!ptr)
		ptr = board->getMovesList(board->currentPlayer());

	int score = getMaxScore(ptr);
	if (score >= beta)
		return score;
	alpha = max(alpha, score);
	if (depth >= maxNg)
		return alpha;
	stable_sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpCapture);

	for (int n = 0; n < ptr->size; n++) {
		if (ptr->list[n].move.xindex == NONE && !ptr->list[n].check)
			break;
		board->doMove(ptr->list[n].move);
		score = -Quiescence(ptr->list[n].next, -beta, -alpha, depth + 1);
		board->undo(ptr->list[n].move);

		delete ptr->list[n].next;
		ptr->list[n].next = NULL;
		if (score >= beta)
			return score;
		alpha = max(alpha, score);
	}
	return alpha;
}

int ComputerPlayer::NegaScout(MoveList *&ptr, int alpha, int beta, int depth, int limit)
{
	if (depth >= limit) {
		if (!tactical.back())
			return -board->CalcScore();
		else
			limit++;
	}
	if (!ptr)
		ptr = board->getMovesList(board->currentPlayer());
	if (!ptr->size)
		return -(INT_MAX - 2);
	stable_sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpHiLow);

	int b = beta, bestScore = -INT_MAX;
	for (int n = 0; n < ptr->size; n++) {
		tactical.push_back(ptr->list[n].check);
		board->doMove(ptr->list[n].move);
		ptr->list[n].score = -NegaScout(ptr->list[n].next, -b, -alpha, depth + 1, limit);

		if (ptr->list[n].score > alpha && ptr->list[n].score < beta && n > 0)
			ptr->list[n].score = -NegaScout(ptr->list[n].next, -beta, -alpha, depth + 1, limit);
		board->undo(ptr->list[n].move);
		tactical.pop_back();

		delete ptr->list[n].next;
		ptr->list[n].next = NULL;

		if (ptr->list[n].score > bestScore) {
			bestScore = ptr->list[n].score;
			alpha = max(bestScore, alpha);
			if (alpha >= beta)
				return alpha;
		}
		b = alpha + 1;
	}
	return bestScore;
}

void ComputerPlayer::think()
{
	srand(time(NULL));
	tactical.clear();
	tactical.push_back(board->inCheck(board->currentPlayer()));
	for (int depth = 0; depth <= maxNg; depth++)
		NegaScout(curr, -INT_MAX, INT_MAX, 0, depth);
	pickMove(curr);

#ifdef DEBUG_SCORES
	debugTree();
#endif
	assert(board->doMove(curr->list[0].move, color) == VALID_MOVE);
	cout << board->printMove(curr->list[0].move) << endl;
	delete curr;
	curr = NULL;
}

bool ComputerPlayer::parseMove(string s, Move &move)
{
	int piece;
	bool place = true;

	// determin move type by first char
	switch(s[0]) {
	case 'a':	case 'b':
	case 'c':	case 'd':
	case 'e':	case 'f':
	case 'g':	case 'h':
		place = false;
		break;
	case 'P':
		piece = PAWN;
		break;
	case 'N':
		piece = KNIGHT;
		break;
	case 'B':
		piece = BISHOP;
		break;
	case 'R':
		piece = ROOK;
		break;
	case 'Q':
		piece = QUEEN;
		break;
	case 'K':
		piece = KING;
		break;
	default:
		return NOT_MOVE;
	}
	// parse placement move
	if (place) {
		if (s[1] < 'a' || s[1] > 'h' || s[2] < '0' || s[2] > '9')
			return false;
		move.to = s[1] - 'a';
		move.to += 8 * (8 - (s[2] - '0'));
		move.from = PLACEABLE;
		move.index = board->pieceIndex(PLACEABLE, color * piece);
	} else {
	// parse movement move
		if (s[0] < 'a' || s[0] > 'h' ||
				s[1] < '0' || s[1] > '9' ||
				s[2] < 'a' || s[2] > 'h' ||
				s[3] < '0' || s[3] > '9')
			return false;
		move.from = s[0] - 'a';
		move.from += 8 * (8 - (s[1] - '0'));
		move.to = s[2] - 'a';
		move.to += 8 * (8 - (s[3] - '0'));
		move.index = board->pieceIndex(move.from);
	}
	// are we talking about a valid piece?
	if (move.index == NONE)
		return false;
	move.xindex = board->pieceIndex(move.to);
	// return the code for what happends when trying to do the move
	return true;
}

void ComputerPlayer::debugTree()
{
	MoveList *ptr = new MoveList(*curr);
	string cmd;
	int mDepth = maxNg, cDepth = 0;
	Move move;
	vector<Move> mstack;

	while (true) {
		printList(ptr);

		cout << "<" << cDepth << "/" << mDepth << "> ";
		cin >> cmd;

		if (cmd == "quit") {
			break;
		} else if (cmd == "up") {
			board->undo(mstack.back());
			mstack.pop_back();
			cDepth--;
			mDepth++;
			delete ptr;
			ptr = NULL;
			NegaScout(ptr, -INT_MAX, INT_MAX, 0, mDepth);
		} else if (cmd == "down") {
			cin >> cmd;
			if (!parseMove(cmd, move)) {
				cout << "error\n";
			} else {
				mstack.push_back(move);
				board->doMove(move);
				cDepth++;
				mDepth--;
				delete ptr;
				ptr = NULL;
				NegaScout(ptr, -INT_MAX, INT_MAX, 0, mDepth);
			}
		} else {
			cout << "error\n";
		}
	}
	for (int i = 0; i < mstack.size(); i++) {
		board->undo(mstack.back());
                mstack.pop_back();
	}
	delete ptr;
}
