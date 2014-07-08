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

	void pickRandomMove()
	{
		const int score = curr->at(0).score;
		int end = 0;

		for (auto item : *curr) {
			if (item.score != score)
				break;
			end++;
		}
		pvMove[0] = curr->at(rand() % end).move;
	}

	int Quiescence(int alpha, int beta, int depth)
	{
		auto ptr = board->getMoveList(board->getStm(), tactical[depth]? MoveClass::ALL : MoveClass::CAPTURE);

		if (ptr->empty()) {
			board->recycle(ptr);
			return tactical[depth]? CHECKMATE_SCORE + board->getPly() : -board->eval();
		}

		int best = MIN_SCORE, score = -board->eval();

		if (score >= beta) {
			board->recycle(ptr);
			return score;
		}
		alpha = max(alpha, score);
		const MoveFlags undoFlags = board->getMoveFlags();
		ptr->sort();

		for (auto& item : *ptr) {
			// set check for opponent
			tactical[depth + 1] = item.check;

			board->make(item.move);
			score = -Quiescence(-beta, -alpha, depth + 1);
			board->unmake(item.move, undoFlags);

			if (score >= beta) {
				board->recycle(ptr);
				return score;
			}
			best = max(best, score);
			alpha = max(alpha, score);
		}
		board->recycle(ptr);
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

		if (ptr->empty()) {
			board->recycle(ptr);
			return false;
		}
		ptr->sort();

		ismate[depth] = false;
		int b = alpha + 1;
		for (auto& item : *ptr) {
			board->make(item.move);

			// set check for opponent
			tactical[depth + 1] = item.check;

			item.score = -NegaScout(-b, -alpha, depth + 1, limit);
			if (item.score > alpha && item.score < beta)
				item.score = -NegaScout(-beta, -alpha, depth + 1, limit);
			board->unmake(item.move, undoFlags);

			best = max(best, item.score);
			if (best >= beta) {
				killer[depth] = item.move;
				tt->setItem(board->hash(), best, killer[depth], limit - depth, CUT_NODE);
				board->recycle(ptr);
				return true;
			} else if (best > alpha) {
				alpha = best;
				pvMove[depth] = item.move;
			}
			b = alpha + 1;
		}
		board->recycle(ptr);
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
		for (auto& item : *curr) {
			tactical[depth + 1] = item.check;

			board->make(item.move);
			item.score = -NegaScout(-b, -alpha, depth + 1, limit);
			if (item.score > alpha && item.score < beta)
				item.score = -NegaScout(-beta, -alpha, depth + 1, limit);
			board->unmake(item.move, undoFlags);

			if (item.score > alpha) {
				alpha = item.score;
				pvMove[depth] = item.move;
				tt->setItem(board->hash(), alpha, pvMove[depth], limit - depth, PV_NODE);
			}
			b = alpha + 1;
		}
		curr->sort();
	}

public:
	Engine(Board<MoveType> *_Board, TransTable<MoveType> *TT) : curr(nullptr), board(_Board), tt(TT)
	{
		maxNg = 5;
	}

	~Engine()
	{
	}

	MoveType think()
	{
		timeval t1, t2;

		srand(time(nullptr));
		curr = nullptr;

		gettimeofday(&t1, nullptr);
		for (int depth = 1; depth <= maxNg; depth++) {
			search(MIN_SCORE, MAX_SCORE, 0, depth);
			gettimeofday(&t2, nullptr);
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

		board->recycle(curr);
		return pvMove[0];
	}
};

#endif
