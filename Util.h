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

#ifndef __UTIL_H__
#define __UTIL_H__                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    #define __COMPUTER_PLAYER_H__ 

#include <chrono>
#include "Defines.h"

using namespace std::chrono;

class Timer
{
public:
	static time_point<high_resolution_clock> now()
	{
		return high_resolution_clock::now();
	}

	static int64 ms_diff(time_point<high_resolution_clock> start)
	{
		return duration_cast<milliseconds>(now() - start).count();
	}
};

#endif
