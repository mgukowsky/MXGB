#include "Bus.h"
#include <new>
#include <cstring>

#define MEMSIZE		0x10000

using namespace MXGB;

Bus::Bus() 
	: _mem(nullptr), readBus(0), writeBus(0) {
	 
	_mem = new (std::nothrow) u8[MEMSIZE];

	if (_mem)
		std::memset(_mem, 0, MEMSIZE);
}

Bus::~Bus() {
	if (_mem)
		delete[] _mem;
}