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

#include <sstream>
#include "BaseBoard.h"
#include "Move.h"

string BaseBoard::printSquare(const int index) const
{
	string tmp;

	if (!square[index])
		return "  ";
	tmp = { pieceSymbol[ABS(square[index])],
		(square[index] > 0)? ' ':'*' };
	return tmp;
}

string BaseBoard::printBoard() const
{
	ostringstream buf;

	buf << "  / - + - + - + - + - + - + - + - \\\n";
	for (int i = 0, rank = 8; ;) {
		buf << rank-- << " |";
		for (int j = 0; j < 8; j++)
			buf << " " << printSquare(i++) << "|";
		if (i == 64)
			break;
		buf << "\n  + - + - + - + - + - + - + - + - +\n";
	}
	buf << "\n  \\ - + - + - + - + - + - + - + - /\n"
		<< "    a   b   c   d   e   f   g   h\n";
	return buf.str();
}