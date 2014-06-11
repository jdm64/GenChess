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

#ifndef __RAND64_H__
#define __RAND64_H__

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include "Defines.h"

#define BLOCK 256

using namespace std;

class Rand64 {
private:
	uint64 box[BLOCK];

	int size, last;

	inline uint64 block()
	{
		int rn;

		if (size < 16)
			size = BLOCK - 1;
		do {
			rn = rand() % size;
		} while (rn == last);
		last = rn;
		swap(box[rn], box[size]);
		size--;
		return box[size + 1];
	}
public:
	Rand64()
	{
		size = BLOCK - 1;
		last = BLOCK;

		srand(time(NULL));
		for (int i = 0; i < BLOCK; i++)
			box[i] = i;
		for (int i = 0; i < BLOCK; i++)
			block();
	}

	uint64 next()
	{
		uint64 val = 0;

		for (int i = 0; i < 8; i++)
			val |= block() << (8 * i);
		return val;
	}
};

#endif
