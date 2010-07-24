/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      ComputerPlayer class functions
 */

#include "ComputerPlayer.h"

#define PRINT_ROOT_SCORES
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
	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpHiLow);
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
	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpCapture);

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
	if (!ptr)
		ptr = board->getMovesList(board->currentPlayer());
	if (!ptr->size)
		return -(INT_MAX - 2);
	if (depth >= limit) {
		if (!tactical.back())
			return getMaxScore(ptr);
		else
			limit++;
	}
	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpHiLow);

	int b = beta;
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

		alpha = max(ptr->list[n].score, alpha);
		if (alpha >= beta)
			return alpha;
		b = alpha + 1;
	}
	return alpha;
}

void ComputerPlayer::think()
{
	srand(time(NULL));
	tactical.clear();
	tactical.push_back(board->inCheck(board->currentPlayer()));
	for (int depth = 0; depth <= maxNg; depth++)
		NegaScout(curr, -INT_MAX, INT_MAX, 0, depth);
	pickMove(curr);

#ifdef PRINT_ROOT_SCORES
	printList(curr);
#endif
	assert(board->doMove(curr->list[0].move, color) == VALID_MOVE);
	cout << board->printMove(curr->list[0].move) << endl;
	delete curr;
	curr = NULL;
}
