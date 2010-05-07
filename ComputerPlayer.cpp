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

void ComputerPlayer::pickMove(MovesPly *ptr, int score)
{
	int x;
	vector<int> list;

	for (int i = 0; i < ptr->size; i++)
		if (ptr->list[i].score == score)
			list.push_back(i);
	x = rand() % list.size();
	swap(ptr->list[0], ptr->list[x]);
}

int ComputerPlayer::getMaxScore(MovesPly *ptr)
{
	int max = ptr->list[0].score;

	for (int i = 1; i < ptr->size; i++)
		if (ptr->list[i].score > max)
			max = ptr->list[i].score;
	return max;
}

int ComputerPlayer::NegaScout(int alpha, int beta, int depth, MovesPly *&ptr)
{
	int b = beta;

	if (!ptr) {
		if (depth <= 0)
			ptr = board->getMovesList(board->currentPlayer(), true);
		else
			ptr = board->getMovesList(board->currentPlayer());
	}
	if (!ptr->size)
		return -INT_MAX;
	if (depth <= 0)
		return getMaxScore(ptr);
	partial_sort(ptr->list.begin(), ptr->list.begin() + ptr->size / 2, ptr->list.begin() + ptr->size, cmpCapture);

	getKillerMoves(ptr, maxDepth - depth);

	for (int n = 0; n < ptr->size; n++) {
		board->doMove(ptr->list[n].move);
		ptr->list[n].score = -NegaScout(-b, -alpha, depth - 1, ptr->list[n].next);
		board->undo(ptr->list[n].move);

		if (ptr->list[n].score > alpha && ptr->list[n].score < beta && n > 0) {
			board->doMove(ptr->list[n].move);
			ptr->list[n].score = -NegaScout(-beta, -alpha, depth - 1, ptr->list[n].next);
			board->undo(ptr->list[n].move);
		}
		alpha = max(ptr->list[n].score, alpha);

		if (alpha >= beta) {
			setKillerMoves(ptr->list[n].move, maxDepth - depth);
			delete ptr->list[n].next;
			ptr->list[n].next = NULL;
			return alpha;
		}
		delete ptr->list[n].next;
		ptr->list[n].next = NULL;
		b = alpha + 1;
	}
	return alpha;
}

void ComputerPlayer::think()
{
	int score;

	srand(time(NULL));
	score = NegaScout(-INT_MAX, INT_MAX, maxDepth, curr);
	pickMove(curr, score);

	assert(board->doMove(curr->list[0].move, color) == VALID_MOVE);
	cout << board->printMove(curr->list[0].move) << endl;
	delete curr;
	curr = NULL;
}
