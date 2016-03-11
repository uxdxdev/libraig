// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.
//https://github.com/damorton/libraig.git

#include "base/event.h"

namespace base{

Event::Event(Type type)
{
	m_Type = type;
}

Event::Type Event::GetType()
{
	return m_Type;
}


Event::~Event()
{

}

} // namespace base



