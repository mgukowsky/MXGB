#pragma once

#include "defs.h"

namespace MXGB {

//Simple base class with methods common to all system components
class Component {

public:
	//Re-initializes all member variables to their initial states, except those members which are pointers or references
	virtual void reset() = 0;

};

}