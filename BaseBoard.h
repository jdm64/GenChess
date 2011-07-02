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

#ifndef __BASE_BOARD_H__
#define __BASE_BOARD_H__

#include <string>
#include "Defines.h"

using namespace std;

class BaseBoard
{
protected:
	int8 square[64];
	int ply;
	int8 stm;

	string printSquare(const int index) const;
public:
	string printBoard() const;
};

#endif
