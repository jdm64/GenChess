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

#include <cassert>
#include "Move.h"
#include "MoveLookup.h"

int8 offsets[7][10] = {
	{ 0,  0,   0,   0,   0,   0,   0,   0, 0, 0},
	{17, 16,  15,   1, -17, -16, -15,  -1, 0, 0}, // Pawn: even=capture
	{33, 31,  18,  14, -33, -31, -18, -14, 0, 0}, // Knight
	{17, 15, -17, -15,   0,   0,   0,   0, 0, 0}, // Bishop
	{16,  1, -16,  -1,   0,   0,   0,   0, 0, 0}, // Rook
	{17, 16,  15,   1, -17, -16, -15,  -1, 0, 0}, // Queen
	{17, 16,  15,   1, -17, -16, -15,  -1, 0, 0} }; // King

DistDB ddb[120] = {
	{ 0,  EMPTY}, {1,    ROOK}, { 1,   ROOK}, { 1,   ROOK}, { 1,   ROOK}, { 1,   ROOK},
	{ 1,   ROOK}, {1,    ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, {0,   EMPTY}, {14, KNIGHT}, {15, BISHOP}, {16,   ROOK}, {17, BISHOP},
	{18, KNIGHT}, {0,   EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, {0,   EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{15, BISHOP}, {31, KNIGHT}, {16,   ROOK}, {33, KNIGHT}, {17, BISHOP}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{16,   ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, {17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {16,   ROOK}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, {17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, {16,   ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, {17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{16,   ROOK}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY},
	{17, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY}, {15, BISHOP}, { 0,  EMPTY}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {16,   ROOK}, { 0,  EMPTY},
	{ 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, { 0,  EMPTY}, {17, BISHOP}
};
