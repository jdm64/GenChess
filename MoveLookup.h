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

#define ATCK_CMP(A, B)		((A^B) >> 3) - ((A^(-B)) >> 3)
#define CAPTURE_MOVE(A, B)	(A * B <  0)
#define ANY_MOVE(A, B)		(A * B <= 0)
#define NOT_EMPTY(A, B)		(A * B != 0)
#define EMPTY_MOVE(A, B)	(A * B == 0)
#define NOT_CAPTURE(A, B)	(A * B >= 0)
#define OWN_PIECE(A, B)		(A * B >  0)

class MoveLookup {
private:
	static char mailbox[120];

	static char mailbox64[64];

	static char offsets[7][8];

	char *square;

public:
	MoveLookup(char *Square) : square(Square) {}

	char* genAll(const char &from) const;

	char* genCapture(const char &from) const;

	char* genMove(const char &from) const;

	bool fromto(const char &From, const char &To) const;

	bool isAttacked(const char &from) const;
};

#endif
