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

template<>
int Engine<GenMove>::NegaScout_hashMiss(int alpha, const int beta, const int depth, int limit)
{
	int score, best = MIN_SCORE;
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
