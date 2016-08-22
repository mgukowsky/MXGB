#pragma once

#include "defs.h"

namespace MXGB {

//Simple mixin base class defines interface elements common to all system components
class Component {

public:
	Component() = default;
	virtual ~Component() = default;
	DISABLE_ALTERNATE_CTORS(Component);

	//Re-initializes all member variables to their initial states, except those members which are pointers or references
	virtual void reset() = 0;

};

} /* namespace MXGB */