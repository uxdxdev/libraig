// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.

#include "base/vector3.h"

#include <iostream>

namespace base{

raig_EXPORT Vector3::Vector3()
{
	m_iX = 0;
	m_iY = 0;
	m_iZ = 0;
	m_iId = 0;
}

raig_EXPORT Vector3::~Vector3()
{
}

raig_EXPORT Vector3::Vector3(int x, int y, int z)
{
	m_iX = x;
	m_iY = y;
	m_iZ = z;
	m_iId = 0;
}

raig_EXPORT Vector3::Vector3(int id, int x, int y, int z)
{
	m_iX = x;
	m_iY = y;
	m_iZ = z;
	m_iId = id;
}

int raig_EXPORT Vector3::Compare(const Vector3 *other)
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

} // namespace base
