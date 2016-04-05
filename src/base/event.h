// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.

#ifndef BASE_EVENT_H
#define BASE_EVENT_H

namespace base{

class Event{
public:
	enum Type{
		PACKET,
		NODE,
		FINISH,
	};

	Event(Type type);
	virtual ~Event();
	virtual Type GetType();

private:
	Type m_Type;
};

} // namespace base

#endif
