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
