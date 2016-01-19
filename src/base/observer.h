#ifndef BASE_OBSERVER_H
#define BASE_OBSERVER_H

namespace base{

class Observer
{
public:
	virtual ~Observer(){}

	virtual void onNotify(Node* node, Event event) = 0;
};

}

#endif
