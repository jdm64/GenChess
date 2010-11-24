#include <climits>
#include "ComputerPlayer.h"

//#define DEBUG_SCORES
//#define RANDOM_MOVE_ORDER

void ComputerPlayer::printList(MoveList *ptr)
{
	for (int i = 0; i < ptr->size; i++)
		cout << ptr->list[i].move.toString() << "[" <<
			ptr->list[i].score << "] ";
	cout << "\n\n";
}

/****
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
****/

void ComputerPlayer::pickMove(MoveList *ptr)
{
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

/****
int ComputerPlayer::getMaxScore(MoveList *ptr)
{
	int max = ptr->list[0].score;

	for (int i = 1; i < ptr->size; i++) {
		if (ptr->list[i].score > max)
			max = ptr->list[i].score;
	}
	return max;
}
****/

int ComputerPlayer::Quiescence(int alpha, int beta, int depth)
{
	MoveList *ptr;
	int score;

	if (depth >= MAX_DEPTH) {
		return -board->eval();
	} else if (tactical[depth]) {
		ptr = board->getMovesList(board->currPlayer());
		if (!ptr->size) {
			delete ptr;
			return CHECKMATE_SCORE;
		}
	} else {
		score = -board->eval();
		if (score >= beta)
			return beta;
		alpha = max(alpha, score);

		ptr = board->getMovesList(board->currPlayer(), MOVE_CAPTURE);
	}

	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpScore);

	for (int n = 0; n < ptr->size; n++) {
		// set check for opponent
		tactical[depth + 1] = ptr->list[n].check;

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

bool ComputerPlayer::NegaMoveType(int &alpha, const int beta, int &best,
		const int depth, const int limit, Array<Move> &killer, const int type)
{
	Move move;

	best = MIN_SCORE;

	// Try Killer Move
	if (board->validMove(killer[depth], move)) {
		ismate[depth] = false;

		board->make(move);

		// set check for opponent
		tactical[depth + 1] = board->incheck(board->currPlayer());

		best = -NegaScout(-beta, -alpha, depth + 1, limit);
		board->unmake(move);

		if (best > alpha) {
			alpha = best;

			if (alpha >= beta) {
				tt->setBest(board, limit - depth, -alpha, move);
				return true;
			}
		}
	}
	// Try all of moveType Moves
	MoveList *ptr = board->getMovesList(board->currPlayer(), type);

	if (!ptr->size) {
		delete ptr;
		return false;
	}
	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpScore);

	ismate[depth] = false;
	int b = alpha + 1;
	for (int n = 0; n < ptr->size; n++) {
		board->make(ptr->list[n].move);

		// set check for opponent
		tactical[depth + 1] = ptr->list[n].check;

		ptr->list[n].score = -NegaScout(-b, -alpha, depth + 1, limit);
		if (ptr->list[n].score > alpha && ptr->list[n].score < beta)
			ptr->list[n].score = -NegaScout(-beta, -alpha, depth + 1, limit);
		board->unmake(ptr->list[n].move);

		if (ptr->list[n].score > best) {
			best = ptr->list[n].score;

			if (best > alpha) {
				alpha = best;

				if (alpha >= beta) {
					killer[depth] = ptr->list[n].move;
					tt->setBest(board, limit - depth, -alpha, killer[depth]);
					delete ptr;
					return true;
				}
			}
		}
		b = alpha + 1;
	}
	delete ptr;
	return false;
}

int ComputerPlayer::NegaScout(int alpha, int beta, int depth, int limit)
{
	if (depth >= limit) {
		if (!tactical[depth])
			return Quiescence(alpha, beta, depth);
		else
			limit++;
	}
	int score, best = MIN_SCORE;
	Move move;

	ismate[depth] = true;

	// Transposition Move
	if (tt->getMove(board, move)) {
		if (!board->validMove(move, move))
			goto hashMiss;
		ismate[depth] = false;
		board->make(move);

		// set check for opponent
		tactical[depth + 1] = board->incheck(board->currPlayer());

		best = -NegaScout(-beta, -alpha, depth + 1, limit);
		board->unmake(move);

		if (best > alpha) {
			alpha = best;

			if (alpha >= beta) {
				tt->setBest(board, limit - depth, -alpha, move);
				return alpha;
			}
		}
	}
hashMiss:
	if (NegaMoveType(alpha, beta, score, depth, limit, captureKiller, MOVE_CAPTURE))
		return score;
	best = max(best, score);
	if (NegaMoveType(alpha, beta, score, depth, limit, moveKiller, MOVE_MOVE))
		return score;
	best = max(best, score);
	if (NegaMoveType(alpha, beta, score, depth, limit, placeKiller, MOVE_PLACE))
		return score;
	best = max(best, score);

	if (ismate[depth])
		best = tactical[depth]? CHECKMATE_SCORE : STALEMATE_SCORE;
	tt->setScore(board, limit - depth, -best);

	return best;
}

void ComputerPlayer::search(int alpha, int beta, int depth, int limit)
{
	curr = curr? curr : board->getMovesList(board->currPlayer());

	int b = beta;
	for (int n = 0; n < curr->size; n++) {
		tactical[depth + 1] = curr->list[n].check;

		board->make(curr->list[n].move);
		curr->list[n].score = -NegaScout(-b, -alpha, depth + 1, limit);
		if (curr->list[n].score > alpha && curr->list[n].score < beta && n > 0)
			curr->list[n].score = -NegaScout(-beta, -alpha, depth + 1, limit);
		board->unmake(curr->list[n].move);

		alpha = max(alpha, curr->list[n].score);
		b = alpha + 1;
	}
	stable_sort(curr->list.begin(), curr->list.begin() + curr->size, cmpScore);
}

Move ComputerPlayer::think()
{
	srand(time(NULL));
	curr = NULL;
	for (int depth = 0; depth <= maxNg; depth++)
		search(MIN_SCORE, MAX_SCORE, 0, depth);
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
