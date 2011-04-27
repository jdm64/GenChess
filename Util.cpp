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

#include "Util.h"

timeval operator-(const timeval l, const timeval r)
{
	timeval ret = l;

	if (l.tv_usec < r.tv_usec) {
		ret.tv_sec--;
		ret.tv_usec += USEC_PER_SEC;
	}
	ret.tv_sec = l.tv_sec - r.tv_sec;
	ret.tv_usec = l.tv_usec - r.tv_usec;
        return ret;
}

void sum(timeval &l, const timeval r)
{
	l.tv_usec += r.tv_usec;

	while (l.tv_usec >= USEC_PER_SEC) {
		l.tv_sec++;
		l.tv_usec -= USEC_PER_SEC;
	}
	l.tv_sec += r.tv_sec;
}

int64 time_in_msec(const timeval t)
{
	return int64(t.tv_sec) * 1000 + int64(t.tv_usec) / 1000;
}
