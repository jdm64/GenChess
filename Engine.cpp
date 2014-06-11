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

#include "Engine.h"

//#define DEBUG_SCORES
#define PRINT_HASH_STATS

template<>
void Engine<GenMove>::pickRandomMove()
{
	const int score = curr->list[0].score;
	int end = 1;

	for (int i = 1; i < curr->size; i++) {
		if (curr->list[i].score != score) {
			end = i + 1;
			break;
		}
	}
	pvMove[0] = curr->list[rand() % end].move;
}

template<>
int Engine<GenMove>::Quiescence(int alpha, int beta, int depth)
{
	GenMoveList* const ptr = board->getMoveList(board->getStm(), tactical[depth]? MoveClass::ALL : MoveClass::CAPTURE);

	if (!ptr->size) {
		delete ptr;
		return tactical[depth]? CHECKMATE_SCORE + board->getPly() : -board->eval();
	}

	int best = MIN_SCORE, score = -board->eval();

	if (score >= beta) {
		delete ptr;
		return score;
	}
	alpha = max(alpha, score);

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
		best = max(best, score);
		alpha = max(alpha, score);
	}
	delete ptr;
	return best;
}

// forward declaration
template<>
int Engine<GenMove>::NegaScout(int alpha, const int beta, const int depth, int limit);

template<>
bool Engine<GenMove>::NegaMoveType(int &alpha, const int beta, int &best,
		const int depth, const int limit, Array<GenMove> &killer, const MoveClass type)
{
	GenMove move;

	best = MIN_SCORE;

	// Try Killer Move
	if (board->validMove(killer[depth], move)) {
		ismate[depth] = false;

		board->make(move);

		// set check for opponent
		tactical[depth + 1] = board->incheck(board->getStm());

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
	GenMoveList* const ptr = board->getMoveList(board->getStm(), type);

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

template<>
int Engine<GenMove>::NegaScout(int alpha, const int beta, const int depth, int limit)
{
	if (depth >= limit) {
		if (!tactical[depth])
			return Quiescence(alpha, beta, depth);
		else
			limit++;
	}
	GenTransItem *tt_item;
	int score, best = MIN_SCORE;
	GenMove move;

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
			tactical[depth + 1] = board->incheck(board->getStm());

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
	if (NegaMoveType(alpha, beta, score, depth, limit, captureKiller, MoveClass::CAPTURE))
		return score;
	best = max(best, score);
	if (NegaMoveType(alpha, beta, score, depth, limit, moveKiller, MoveClass::MOVE))
		return score;
	best = max(best, score);
	if (NegaMoveType(alpha, beta, score, depth, limit, placeKiller, MoveClass::PLACE))
		return score;
	best = max(best, score);

	if (ismate[depth])
		best = tactical[depth]? CHECKMATE_SCORE + board->getPly() : STALEMATE_SCORE;
	tt->setItem(board->hash(), best, pvMove[depth], limit - depth, (pvMove[depth].isNull())? ALL_NODE : PV_NODE);

	return best;
}

template<>
void Engine<GenMove>::search(int alpha, const int beta, const int depth, const int limit)
{
	curr = curr? curr : board->getMoveList(board->getStm(), MoveClass::ALL);

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

template<>
void Engine<GenMove>::debugTree()
{
	GenMoveList *ptr = new GenMoveList(*curr);
	string cmd;
	int mDepth = maxNg, cDepth = 0;
	GenMove move;
	vector<GenMove> mstack;

	while (true) {
		ptr->print();

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
			if (board->validMove(cmd, board->getStm(), move) != VALID_MOVE) {
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

// -- Start Regular Chess ---

template<>
void Engine<RegMove>::pickRandomMove()
{
	int score = curr->list[0].score, end = 1;

	for (int i = 1; i < curr->size; i++) {
		if (curr->list[i].score != score) {
			end = i + 1;
			break;
		}
	}
	pvMove[0] = curr->list[rand() % end].move;
}

template<>
int Engine<RegMove>::Quiescence(int alpha, int beta, int depth)
{
	RegMoveList *ptr = board->getMoveList(board->getStm(), tactical[depth]? MoveClass::ALL : MoveClass::CAPTURE);
	const MoveFlags undoFlags = board->getMoveFlags();

	if (!ptr->size) {
		delete ptr;
		return tactical[depth]? CHECKMATE_SCORE + board->getPly() : -board->eval();
	}

	int best = MIN_SCORE, score = -board->eval();

	if (score >= beta) {
		delete ptr;
		return score;
	}
	alpha = max(alpha, score);

	sort(ptr->list.begin(), ptr->list.begin() + ptr->size, cmpScore);

	for (int n = 0; n < ptr->size; n++) {
		// set check for opponent
		tactical[depth + 1] = ptr->list[n].check;

		board->make(ptr->list[n].move);
		score = -Quiescence(-beta, -alpha, depth + 1);
		board->unmake(ptr->list[n].move, undoFlags);

		if (score >= beta) {
			delete ptr;
			return score;
		}
		best = max(best, score);
		alpha = max(alpha, score);
	}
	delete ptr;
	return best;
}

// forward declaration
template<>
int Engine<RegMove>::NegaScout(int alpha, int beta, int depth, int limit);

template<>
bool Engine<RegMove>::NegaMoveType(int &alpha, const int beta, int &best,
		const int depth, const int limit, Array<RegMove> &killer, const MoveClass type)
{
	const MoveFlags undoFlags = board->getMoveFlags();
	RegMove move;

	best = MIN_SCORE;

	// Try Killer Move
	if (board->validMove(killer[depth], move)) {
		ismate[depth] = false;

		board->make(move);

		// set check for opponent
		tactical[depth + 1] = board->incheck(board->getStm());

		best = -NegaScout(-beta, -alpha, depth + 1, limit);
		board->unmake(move, undoFlags);

		if (best >= beta) {
			tt->setItem(board->hash(), best, move, limit - depth, CUT_NODE);
			return true;
		} else if (best > alpha) {
			alpha = best;
			pvMove[depth] = move;
		}
	}
	// Try all of moveType Moves
	RegMoveList *ptr = board->getMoveList(board->getStm(), type);

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
		board->unmake(ptr->list[n].move, undoFlags);

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

template<>
int Engine<RegMove>::NegaScout(int alpha, int beta, int depth, int limit)
{
	const MoveFlags undoFlags = board->getMoveFlags();

	if (depth >= limit) {
		if (!tactical[depth])
			return Quiescence(alpha, beta, depth);
		else
			limit++;
	}
	RegTransItem *tt_item;
	int score, best = MIN_SCORE;
	RegMove move;

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
			tactical[depth + 1] = board->incheck(board->getStm());

			best = -NegaScout(-beta, -alpha, depth + 1, limit);
			board->unmake(move, undoFlags);

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
	if (NegaMoveType(alpha, beta, score, depth, limit, captureKiller, MoveClass::CAPTURE))
		return score;
	best = max(best, score);
	if (NegaMoveType(alpha, beta, score, depth, limit, moveKiller, MoveClass::MOVE))
		return score;
	best = max(best, score);

	if (ismate[depth])
		best = tactical[depth]? CHECKMATE_SCORE + board->getPly() : STALEMATE_SCORE;
	tt->setItem(board->hash(), best, pvMove[depth], limit - depth, (pvMove[depth].isNull())? ALL_NODE : PV_NODE);

	return best;
}

template<>
void Engine<RegMove>::search(int alpha, int beta, int depth, int limit)
{
	const MoveFlags undoFlags = board->getMoveFlags();
	curr = curr? curr : board->getMoveList(board->getStm(), MoveClass::ALL);

	int b = beta;
	for (int n = 0; n < curr->size; n++) {
		tactical[depth + 1] = curr->list[n].check;

		board->make(curr->list[n].move);
		curr->list[n].score = -NegaScout(-b, -alpha, depth + 1, limit);
		if (curr->list[n].score > alpha && curr->list[n].score < beta && n > 0)
			curr->list[n].score = -NegaScout(-beta, -alpha, depth + 1, limit);
		board->unmake(curr->list[n].move, undoFlags);

		if (curr->list[n].score > alpha) {
			alpha = curr->list[n].score;
			pvMove[depth] = curr->list[n].move;
			tt->setItem(board->hash(), alpha, pvMove[depth], limit - depth, PV_NODE);
		}
		b = alpha + 1;
	}
	stable_sort(curr->list.begin(), curr->list.begin() + curr->size, cmpScore);
}

template<>
void Engine<RegMove>::debugTree()
{
	RegMoveList *ptr = new RegMoveList(*curr);
	string cmd;
	int mDepth = maxNg, cDepth = 0;
	RegMove move;
	vector<RegMove> mstack;

	while (true) {
		ptr->print();

		cout << "<" << cDepth << "/" << mDepth << "> ";
		cin >> cmd;

		if (cmd == "quit") {
			break;
		} else if (cmd == "up") {
		//	board->unmake(mstack.back());
			mstack.pop_back();
			cDepth--;
			mDepth++;
			delete ptr;
			ptr = NULL;
			NegaScout(-INT_MAX, INT_MAX, 0, mDepth);
		} else if (cmd == "down") {
			cin >> cmd;
			if (board->validMove(cmd, board->getStm(), move) != VALID_MOVE) {
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
	//	board->unmake(mstack.back());
                mstack.pop_back();
	}
	delete ptr;
}
