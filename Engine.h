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

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <iostream>
#include <climits>
#include <vector>
#include "Array.h"
#include "Util.h"
#include "TransTable.h"

#define MAX_DEPTH		10
#define MIN_SCORE		-(INT_MAX - 4)
#define MAX_SCORE		(INT_MAX - 4)
#define CHECKMATE_SCORE		-(INT_MAX - 4)
#define STALEMATE_SCORE		0

template<class MoveType>
class Engine
{
private:
	MoveList<MoveType> *curr;

	Board<MoveType> *board;
	TransTable<MoveType> *tt;

	Array<MoveType> captureKiller;
	Array<MoveType> moveKiller;
	Array<MoveType> placeKiller;
	Array<MoveType> pvMove;

	int maxNg;

	Array<bool> tactical;
	Array<bool> ismate;

	ScoreSort<MoveType> cmpScore;

	void pickRandomMove()
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

	int Quiescence(int alpha, int beta, int depth)
	{
		auto ptr = board->getMoveList(board->getStm(), tactical[depth]? MoveClass::ALL : MoveClass::CAPTURE);

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
		const MoveFlags undoFlags = board->getMoveFlags();
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

	bool NegaMoveType(int &alpha, const int beta, int &best, const int depth, const int limit, Array<MoveType> &killer, const MoveClass type)
	{
		const MoveFlags undoFlags = board->getMoveFlags();
		MoveType move;

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
		auto ptr = board->getMoveList(board->getStm(), type);

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

	int NegaScout_hashMiss(int alpha, const int beta, const int depth, int limit);

	int NegaScout(int alpha, const int beta, const int depth, int limit)
	{
		if (depth >= limit) {
			if (!tactical[depth])
				return Quiescence(alpha, beta, depth);
			else
				limit++;
		}
		TransItem<MoveType> *tt_item;

		ismate[depth] = true;
		pvMove[depth].setNull();

		// Try Transposition Table
		if (tt->getItem(board->hash(), tt_item)) {
			// Try score
			int ttScore;
			if (tt_item->getScore(alpha, beta, limit - depth, ttScore))
				return ttScore;

			// Try Move
			MoveType move;
			if (tt_item->getMove(move)) {
				if (!board->validMove(move, move))
					goto hashMiss;
				const MoveFlags undoFlags = board->getMoveFlags();
				ismate[depth] = false;

				board->make(move);

				// set check for opponent
				tactical[depth + 1] = board->incheck(board->getStm());

				int best = -NegaScout(-beta, -alpha, depth + 1, limit);
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
		return NegaScout_hashMiss(alpha, beta, depth, limit);
	}

	void search(int alpha, const int beta, const int depth, const int limit)
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

public:
	Engine(Board<MoveType> *_Board, TransTable<MoveType> *TT) : curr(NULL), board(_Board), tt(TT)
	{
		maxNg = 5;
	}

	~Engine()
	{
	}

	MoveType think()
	{
		timeval t1, t2;

		srand(time(NULL));
		curr = NULL;

		gettimeofday(&t1, NULL);
		for (int depth = 1; depth <= maxNg; depth++) {
			search(MIN_SCORE, MAX_SCORE, 0, depth);
			gettimeofday(&t2, NULL);
			cout << "stats depth " << depth << " time " << time_in_msec(t2 - t1) << endl;
		}

#ifdef PRINT_HASH_STATS
		sixInt st = tt->stats();
		cout << "stats total=" << st.one + st.two << " hits=" << 100 * st.one / double(st.one + st.two)
			<< "% scorehits=" << 100 * st.three / double(st.three + st.four)
			<< "% movehits=" << 100 * st.five / double(st.five + st.six) << "%" << endl;
		tt->clearStats();
#endif

		// Randomize opening
		if (board->getPly() < 7)
			pickRandomMove();

		delete curr;
		return pvMove[0];
	}
};

#endif
