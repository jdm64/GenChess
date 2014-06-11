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

#ifndef __PIECE_H__
#define __PIECE_H__

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

	int canEnPassant() const
	{
		return bits & CAN_EP;
	}

	int enPassantFile() const
	{
		return bits & EP_FILE;
	}

	void setEnPassant(int file)
	{
		bits = (bits & ~0xf) | (file | CAN_EP);
	}

	void clearEnPassant()
	{
		bits &= ~0xf;
	}

	int canCastle(const int color) const
	{
		return bits & ((color == WHITE)? W_CASTLE : B_CASTLE);
	}

	int canKingCastle(const int color) const
	{
		return bits & ((color == WHITE)? WK_CASTLE : BK_CASTLE);
	}

	int canQueenCastle(const int color) const
	{
		return bits & ((color == WHITE)? WQ_CASTLE : BQ_CASTLE);
	}

	void clearCastle(int color)
	{
		bits &= ((color == WHITE)? ~W_CASTLE : ~B_CASTLE);
	}

	void clearKingCastle(int color)
	{
		bits &= ((color == WHITE)? ~WK_CASTLE : ~BK_CASTLE);
	}

	void clearQueenCastle(int color)
	{
		bits &= ((color == WHITE)? ~WQ_CASTLE : ~BQ_CASTLE);
	}

	void setCastle(int value)
	{
		bits &= (0xff & value);
	}
};

#endif
