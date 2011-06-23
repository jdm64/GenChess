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

#ifndef __MOVE_LOOKUP_H__
#define __MOVE_LOOKUP_H__

#include "Defines.h"

#define ATCK_CMP(A, B)		((A^B) >> 3) - ((A^(-B)) >> 3)
#define CAPTURE_MOVE(A, B)	(A * B <  0)
#define ANY_MOVE(A, B)		(A * B <= 0)
// #define NOT_EMPTY(A, B)	(A * B != 0)
// #define EMPTY_MOVE(A, B)	(A * B == 0)
#define NOT_CAPTURE(A, B)	(A * B >= 0)
#define OWN_PIECE(A, B)		(A * B >  0)

class GenMoveLookup {
private:
	static const int8 mailbox[120];

	static const int8 mailbox64[64];

	static const int8 offsets[7][8];

	const int8 *square;

public:
	GenMoveLookup(int8 *Square) : square(Square) {}

	int8* genAll(const int8 &from) const;

	int8* genCapture(const int8 &from) const;

	int8* genMove(const int8 &from) const;

	bool fromto(const int8 &From, const int8 &To) const;

	bool isAttacked(const int8 &from) const;
};

#endif
