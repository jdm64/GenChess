/*	GenChess, a genesis chess engine
	Copyright (C) 2010, Justin Madru (jdm64@gawab.com)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __RAND64_H__
#define __RAND64_H__

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <ctime>

typedef uint64_t uint64;

#define BLOCK 8192

class Rand64 {
private:
	uint64 box[BLOCK];

	int size, last;

	inline uint64 block()
	{
		int rn;

		if (size < 6)
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
		for (int i = 0; i < BLOCK; i++)
			box[i] = i;
		srand(time(NULL));
	}

	uint64 next()
	{
		uint64 val = 0;

		for (int i = 0; i < 5; i++)
			val |= block() << (13 * i);
		return val;
	}
};

#endif
