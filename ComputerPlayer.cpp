/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      ComputerPlayer class functions
 */

#include "ComputerPlayer.h"
#include <climits>
#include <iostream>

//#define DEBUG_SCORES
#define RANDOM_MOVE_ORDER

void ComputerPlayer::printList(MoveList *ptr)
{
	for (int i = 0; i < ptr->size; i++)
		cout << ptr->list[i].move.toString() << "[" <<
			ptr->list[i].score << "] ";
	cout << "\n\n";
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
	stable_sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpScore);
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

int ComputerPlayer::Quiescence(int alpha, int beta, int depth)
{
	MoveList *ptr = board->getMovesList(board->currPlayer());

	int score = getMaxScore(ptr);
	if (score >= beta) {
		delete ptr;
		return score;
	}
	alpha = max(alpha, score);
	if (depth >= maxNg) {
		delete ptr;
		return alpha;
	}
	stable_sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpScore);

	for (int n = 0; n < ptr->size; n++) {
		if (ptr->list[n].move.xindex == NONE && !ptr->list[n].check)
			break;
		board->make(ptr->list[n].move);
		score = -Quiescence(-beta, -alpha, depth + 1);
		board->unmake(ptr->list[n].move);

		if (score >= beta) {
			delete ptr;
			return score;
		}
		alpha = max(alpha, score);
	}
	delete ptr;
	return alpha;
}

int ComputerPlayer::NegaScout(int alpha, int beta, int depth, int limit)
{
	if (depth >= limit) {
		if (!tactical[depth])
			return -board->eval();
		else
			limit++;
	}
	int bestScore = -INT_MAX;
	Move move;

	if (tt->getMove(board, move)) {
		if (!board->validMove(move))
			goto hashMiss;
		board->make(move);
		tactical[depth + 1] = board->incheck(board->currPlayer());

		bestScore = -NegaScout(-beta, -alpha, depth + 1, limit);

		board->unmake(move);

		if (bestScore >= beta) {
			tt->setBest(board, limit - depth, -bestScore, move);
			return bestScore;
		}
		alpha = max(bestScore, alpha);
	}
hashMiss:
	MoveList *ptr = (!depth && curr)? curr : board->getMovesList(board->currPlayer());
	if (!ptr->size) {
		delete ptr;
		return tactical[depth]? -(INT_MAX - 2) : -(INT_MAX / 2);
	}
	stable_sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpScore);

	int b = alpha + 1;
	for (int n = 0; n < ptr->size; n++) {
		board->make(ptr->list[n].move);
		tactical[depth + 1] = ptr->list[n].check;
		ptr->list[n].score = -NegaScout(-b, -alpha, depth + 1, limit);

		if (ptr->list[n].score > alpha && ptr->list[n].score < beta)
			ptr->list[n].score = -NegaScout(-beta, -alpha, depth + 1, limit);
		board->unmake(ptr->list[n].move);

		if (ptr->list[n].score > bestScore) {
			bestScore = ptr->list[n].score;
			alpha = max(bestScore, alpha);
			if (alpha >= beta) {
				tt->setBest(board, limit - depth, -alpha, ptr->list[n].move);
				delete ptr;
				return alpha;
			}
		}
		b = alpha + 1;
	}
	tt->setScore(board, limit - depth, -bestScore);
	if (depth)
		delete ptr;
	else
		curr = ptr;
	return bestScore;
}

Move ComputerPlayer::think()
{
	srand(time(NULL));
	curr = NULL;
	tactical[0] = board->incheck(board->currPlayer());
	for (int depth = 0; depth <= maxNg; depth++)
		NegaScout(-INT_MAX, INT_MAX, 0, depth);
	pickMove(curr);

#ifdef DEBUG_SCORES
	debugTree();
#endif
	Move move = curr->list[0].move;
	delete curr;
	return move;
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
			board->unmake(mstack.back());
			mstack.pop_back();
			cDepth--;
			mDepth++;
			delete ptr;
			ptr = NULL;
			NegaScout(-INT_MAX, INT_MAX, 0, mDepth);
		} else if (cmd == "down") {
			cin >> cmd;
			if (board->validMove(cmd, board->currPlayer(), move) != VALID_MOVE) {
				cout << "error\n";
			} else {
				mstack.push_back(move);
				board->make(move);
				cDepth++;
				mDepth--;
				delete ptr;
				ptr = NULL;
				NegaScout(-INT_MAX, INT_MAX, 0, mDepth);
			}
		} else {
			cout << "error\n";
		}
	}
	for (unsigned int i = 0; i < mstack.size(); i++) {
		board->unmake(mstack.back());
                mstack.pop_back();
	}
	delete ptr;
}
