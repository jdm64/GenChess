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

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <cstring>

template<class T>
class Array {
private:
	T *data;

	int size;
public:
	Array(int Size = 1)
	{
		size = Size;
		data = new T[size];
	}

	~Array() { delete[] data; }

	const T operator[](const int i) const
	{
		T *tmp;

		if (i >= size) {
			tmp = new T[i + 1];
			memcpy(tmp, data, size * sizeof(T));
			memset(&tmp[size], 0, (i - size + 1) * sizeof(T));
			delete[] data;
			data = tmp;
			size = i + 1;
		}
		return data[i];
	}

	T& operator[](const int i)
	{
		T *tmp;

		if (i >= size) {
			tmp = new T[i + 1];
			memcpy(tmp, data, size * sizeof(T));
			memset(&tmp[size], 0, (i - size + 1) * sizeof(T));
			delete[] data;
			data = tmp;
			size = i + 1;
		}
		return data[i];
	}

	int len() const
	{
		return size;
	}
};

#endif
