#include "Bus.h"
#include <new>
#include <cstdlib>
#include <cstring>

#define MEMSIZE		0x10000

using namespace MXGB;

Bus::Bus() 
	: _mem(nullptr), readBus(0), writeBus(0) {
	 
	_mem = new (std::nothrow) u8[MEMSIZE];

	reset();
}

Bus::~Bus() {
	if (_mem)
		delete[] _mem;
}

void Bus::reset() {
	if (_mem)
		std::memset(_mem, 0, MEMSIZE);
	readBus = writeBus = 0;
}