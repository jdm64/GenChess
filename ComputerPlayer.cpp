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
			return beta;
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

		if (best >= beta) {
			tt->setItem(board->hash(), best, move, limit - depth, CUT_NODE);
			return true;
		} else if (best > alpha) {
			alpha = best;
			pvMove[depth] = move;
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

		best = max(best, ptr->list[n].score);
		if (best >= beta) {
			killer[depth] = ptr->list[n].move;
			tt->setItem(board->hash(), best, killer[depth], limit - depth, CUT_NODE);
			delete ptr;
			return true;
		} else if (best > alpha) {
			alpha = best;
			pvMove[depth] = ptr->list[n].move;
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
	TransItem *tt_item;
	int score, best = MIN_SCORE;
	Move move;

	ismate[depth] = true;
	pvMove[depth].setNull();

	// Try Transposition Table
	if (tt->getItem(board->hash(), tt_item)) {
		// Try score
		if (tt_item->getScore(alpha, beta, limit - depth, score))
			return score;

		// Try Move
		if (tt_item->getMove(move)) {
			if (!board->validMove(move, move))
				goto hashMiss;
			ismate[depth] = false;

			board->make(move);

			// set check for opponent
			tactical[depth + 1] = board->incheck(board->currPlayer());

			best = -NegaScout(-beta, -alpha, depth + 1, limit);
			board->unmake(move);

			if (best >= beta) {
				tt->setItem(board->hash(), best, move, limit - depth, CUT_NODE);
				return best;
			} else if (best > alpha) {
				alpha = best;
				pvMove[depth] = move;
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
	tt->setItem(board->hash(), best, pvMove[depth], limit - depth, (pvMove[depth].isNull())? ALL_NODE : PV_NODE);

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

		if (curr->list[n].score > alpha) {
			alpha = curr->list[n].score;
			pvMove[depth] = curr->list[n].move;
			tt->setItem(board->hash(), alpha, pvMove[depth], limit - depth, PV_NODE);
		}
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

#ifdef DEBUG_SCORES
	debugTree();
#endif
	delete curr;
	return pvMove[0];
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
