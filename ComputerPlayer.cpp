/*      Justin Madru (jdm64@gawab.com)
 *      CPSC 386 - Game Design
 *      Assignment #6 - (2010-5-7)
 *
 *      ComputerPlayer class functions
 */

#include "ComputerPlayer.h"

void ComputerPlayer::getKillerMoves(MovesPly *ptr, int depth)
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

void ComputerPlayer::pickMove(MovesPly *ptr)
{
	int x;
	vector<int> match;

	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpHiLow);
	for (int i = 0; i < ptr->size; i++) {
		cout << board->printMove(ptr->list[i].move) << "[" <<
			ptr->list[i].score << "] ";
		if (ptr->list[i].score == score)
			match.push_back(i);
	}
	x = rand() % match.size();
	swap(ptr->list[0], ptr->list[match[x]]);
}

int ComputerPlayer::getMaxScore(MovesPly *ptr)
{
	int max = ptr->list[0].score;

	for (int i = 1; i < ptr->size; i++)
		if (ptr->list[i].score > max)
			max = ptr->list[i].score;
	return max;
}

int ComputerPlayer::Quiescence(MovesPly *&ptr, int alpha, int beta, int depth)
{
	int score;

	if (!ptr)
		ptr = board->getMovesList(board->currentPlayer());

	score = getMaxScore(ptr);
	if (score >= beta)
		return score;
	alpha = max(alpha, score);
	if (depth >= maxDepth)
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

int ComputerPlayer::NegaScout(MovesPly *&ptr, int alpha, int beta, int depth, int limit)
{
	int b = beta;

	if (!ptr)
		ptr = board->getMovesList(board->currentPlayer());
	if (!ptr->size)
		return -(INT_MAX - 2);
	if (depth >= limit)
		return Quiescence(ptr, alpha, beta, 0);
	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpHiLow);
	if (depth)
		getKillerMoves(ptr, depth);

	for (int n = 0; n < ptr->size; n++) {
		board->doMove(ptr->list[n].move);
		ptr->list[n].score = -NegaScout(ptr->list[n].next, -b, -alpha, depth + 1, limit);

		if (ptr->list[n].score > alpha && ptr->list[n].score < beta && n > 0)
			ptr->list[n].score = -NegaScout(ptr->list[n].next, -beta, -alpha, depth + 1, limit);
		board->undo(ptr->list[n].move);

		delete ptr->list[n].next;
		ptr->list[n].next = NULL;

		alpha = max(ptr->list[n].score, alpha);
		if (alpha >= beta) {
 			setKillerMoves(ptr->list[n].move, depth);
			return alpha;
		}
		b = alpha + 1;
	}
	return getMaxScore(ptr);
}

void ComputerPlayer::think()
{
	int score;

	srand(time(NULL));
	for (int depth = 0; depth <= maxDepth; depth++)
		score = NegaScout(curr, -INT_MAX, INT_MAX, 0, depth);
	pickMove(curr);

	assert(board->doMove(curr->list[0].move, color) == VALID_MOVE);
	cout << board->printMove(curr->list[0].move) << endl;
	delete curr;
	curr = NULL;
}
