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

#ifndef __PIECE_H__
#define __PIECE_H__

struct Piece {
	int8 loc;
	int8 type;
};

struct MoveFlags {
	uint8 bits;

	MoveFlags()
	{
		reset();
	}

	void reset()
	{
		bits = 0xf0;
	}

	int8 canEnPassant() const
	{
		return bits & 0x8;
	}

	int8 enPassantFile() const
	{
		return bits & 0x7;
	}

	void setEnPassant(int8 file)
	{
		bits = (bits & ~0xf) | (file | 0x8);
	}

	void clearEnPassant()
	{
		bits &= ~0xf;
	}

	int8 canCastle(const int8 color) const
	{
		return bits & ((color == WHITE)? 0x30 : 0xc0);
	}

	int8 canKingCastle(const int8 color) const
	{
		return bits & ((color == WHITE)? 0x10 : 0x40);
	}

	int8 canQueenCastle(const int8 color) const
	{
		return bits & ((color == WHITE)? 0x20 : 0x80);
	}

	void clearCastle(int8 color)
	{
		bits &= ((color == WHITE)? ~0x30 : ~0xc0);
	}

	void clearKingCastle(int8 color)
	{
		bits &= ((color == WHITE)? ~0x10 : ~0x40);
	}

	void clearQueenCastle(int8 color)
	{
		bits &= ((color == WHITE)? ~0x20 : ~0x80);
	}

	void setCastle(int8 value)
	{
		bits &= (0xff & value);
	}
};

#endif