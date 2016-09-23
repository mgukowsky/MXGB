#include "CPU.h"

//8-bit register convenience macros
#define regA		regAF.hibyte
#define regF		regAF.lobyte
#define regB		regBC.hibyte
#define regC		regBC.lobyte
#define regD		regDE.hibyte
#define regE		regDE.lobyte
#define regH		regHL.hibyte
#define regL		regHL.lobyte

//16-bit regs
#define reg16AF		regAF.whole
#define reg16BC		regBC.whole
#define reg16DE		regDE.whole
#define reg16HL		regHL.whole
#define reg16PC		regPC.whole
#define reg16SP		regSP.whole

//ROM execution will start at this address
#define GB_ENTRY_POINT		0x0100

#define INITIAL_STACK_POSITION		0xFFFE

using namespace MXGB;

CPU::CPU(Bus &rb)
	: refBus(rb), refCore(Core::get_app_core()) {

	reset();
}

void CPU::reset() {
	reg16AF = 0;
	reg16BC = 0;
	reg16DE = 0;
	reg16HL = 0;
	reg16PC = GB_ENTRY_POINT;
	reg16SP = INITIAL_STACK_POSITION;
}

const u8 CPU::execute_next() {
	//Get the opcode at PC, use it to get the callback, and invoke it with the correct context.
	//Invoking a pointer to a member function has an annoying syntax, which needs an explicit 'this' and specific parentheses:
	//		(this->*memberFuncPtr)(memberFuncArgs...);
	//This is because operator() has a higher precedence than operator->*, and operator->* needs to evaluate first
	return (this->*(OpcodeVector[refBus.read8(reg16PC)]))();
}

//Generated opcode vector and operation definitions go here
#include "../moc/OpcodeVector.gen.h"
#include "../moc/ops.gen.h"

