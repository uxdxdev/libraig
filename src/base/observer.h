// Copyright (c) 2016 David Morton
// Use of this source code is governed by a license that can be
// found in the LICENSE file.

#ifndef BASE_OBSERVER_H
#define BASE_OBSERVER_H

#include "base/node.h"
#include "base/event.h"

namespace base{

class Observer
{
public:
	virtual ~Observer(){}

	virtual void onNotify(Node* node, Event event) = 0;
};

}

#endif
