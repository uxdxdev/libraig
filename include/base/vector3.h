// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.
// https://github.com/damorton/libraig.git

#ifndef BASE_VECTOR3_H_
#define BASE_VECTOR3_H_

#include "export/raig_Export.h"

namespace base{

class Vector3{
public:
	int m_iX;
	int m_iY;
	int m_iZ;
	int m_iId;

	raig_EXPORT Vector3();
	raig_EXPORT ~Vector3();
	raig_EXPORT Vector3(int x, int y, int z);
	raig_EXPORT Vector3(int id, int x, int y, int z);
	int raig_EXPORT Compare(const Vector3 *other);
};

} // namespace base

#endif
