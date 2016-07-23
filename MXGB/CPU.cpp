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
#define regAF		regAF.whole
#define regBC		regBC.whole
#define regDE		regDE.whole
#define regHL		regHL.whole
#define regPC		regPC.whole
#define regSP		regSP.whole

//ROM execution will start at this address
#define GB_ENTRY_POINT		0x0100

#define INITIAL_STACK_POSITION		0xFFFE;

using namespace MXGB;

CPU::CPU(Bus &rb)
	: refBus(rb) {

	regAF = 0;
	regBC = 0;
	regDE = 0;
	regHL = 0;
	regPC = GB_ENTRY_POINT;
	regSP = INITIAL_STACK_POSITION;
}