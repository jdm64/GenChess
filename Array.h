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
			delete data;
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
