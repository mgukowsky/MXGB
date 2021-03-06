#pragma once

#include "Bus.h"

namespace MXGB {

//Emulates the Game Boy's modified Z80 processor
class CPU : public Component {
DECLARE_TESTRUNNER_ACCESS;

public:
	explicit CPU(Bus& rb);
	~CPU() = default;

	DISABLE_ALTERNATE_CTORS(CPU);

	/**
	 * @brief Sets all registers to zero (does NOT affect refBus)
	 */
	void reset() override final;

	/**
	 * @brief Execute the CPU instruction at regPC
	 * 
	 * @return The number of cycles taken
	 */
	FORCEINLINE const u8 execute_next() {
		//Get the opcode at PC, use it to get the callback, and invoke it with the correct context.
		//Invoking a pointer to a member function has an annoying syntax, which needs an explicit 'this' and specific parentheses:
		//		(this->*memberFuncPtr)(memberFuncArgs...);
		//This is because operator() has a higher precedence than operator->*, and operator->* needs to evaluate first
		return (this->*(OpcodeVector[refBus.read8(regPC.whole)]))();
	}

private:
	/**
	 * @brief Function pointers to the functions for CPU operations (note pointer-to-member-function syntax)
	 * 
	 * @return The number of machine cycles taken
	 */
	typedef const u8(CPU::*OpCallback)();

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