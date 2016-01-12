/*

The MIT License (MIT)

Copyright (c) 2016 David Morton

https://github.com/damorton/libraig.git

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef _INCLUDE_VECTOR3_H_
#define _INCLUDE_VECTOR3_H_

#include <string>

class Vector3{
public:
	int m_iX;
	int m_iY;
	int m_iZ;
	int m_iId;

	Vector3()
	{
		m_iX = 0;
		m_iY = 0;
		m_iZ = 0;
		m_iId = 0;
	}

	Vector3(int x, int y, int z)
	{
		m_iX = x;
		m_iY = y;
		m_iZ = z;
		m_iId = 0;
	}

	Vector3(int id, int x, int y, int z)
	{
		m_iX = x;
		m_iY = y;
		m_iZ = z;
		m_iId = id;
	}

	int Compare(const Vector3 *other)
	{
		if(this->m_iX == other->m_iX && this->m_iY == other->m_iY && this->m_iZ == other->m_iZ)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	virtual ~Vector3()
	{

	}
};

#endif
