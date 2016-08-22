#pragma once

#include "Bus.h"

namespace MXGB {

//Emulates the Game Boy's modified Z80 processor
class CPU : public Component {
DECLARE_TESTRUNNER_ACCESS;

public:
	CPU(Bus& rb);
	~CPU() = default;

	DISABLE_ALTERNATE_CTORS(CPU);

	void reset() override final;

private:
	/**
	 * @brief Function pointers to the functions for CPU operations
	 * 
	 * @return The number of machine cycles taken
	 */
	typedef const u8(*OpCallback)(CPU &thisCPU);

	//Opcodes serve as an index into this array to retrieve the correct callback
	static const OpCallback OpcodeVector[];

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
	Core &refCore;

	//The opcode vector and headers for the opcode callbacks are generated and placed here
#include"../moc/opdecl.gen.h"

};

} /*namespace MXGB*/