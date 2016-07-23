#pragma once

#include "Bus.h"

namespace MXGB {

//Emulates the Game Boy's modified Z80 processor
class CPU {
DECLARE_TESTRUNNER_ACCESS;

public:
	CPU(Bus& rb);
	~CPU() = default;

	DISABLE_ALTERNATE_CTORS(CPU);

private:
	union Z80REG {
		u16 whole;
		struct {
			u8 lobyte;
			u8 hibyte;
		};
		struct {
			u8 __unused : 4; //These bits should always be 0
			u8 flagC : 1;
			u8 flagH : 1;
			u8 flagN : 1;
			u8 flagZ : 1;
		};
	} regAF, regBC, regDE, regHL, regPC, regSP;

	Bus &refBus;

};

} /*namespace MXGB*/