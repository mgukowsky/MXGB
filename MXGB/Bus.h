#pragma once

#include "Core.h"

namespace MXGB {

//Handles the emulated memory of the app; essentially a pseudo-MMU
class Bus {
DECLARE_TESTRUNNER_ACCESS;
public:
	Bus();
	~Bus();

	DISABLE_ALTERNATE_CTORS(Bus);

	//Check if an error occurred during the ctor
	const bool is_mem_allocated() { return !!_mem; }

	//Readers and writers for main memory
	FORCEINLINE u8 read8(u16 addr) { return *(_mem + coerce_address(addr)); }
	FORCEINLINE void write8(u16 addr, const u8 val) { *(_mem + coerce_address(addr)) = val; }

	//EDGE CASE: reading/writing 16 bytes at mirror boundaries (0xDFFF and 0xFDFF) will behave inappropriately
	FORCEINLINE u16 read16(u16 addr) { return *(reinterpret_cast<u16*>(_mem + coerce_address(addr))); }
	FORCEINLINE void write16(u16 addr, const u16 val) { *(reinterpret_cast<u16*>(_mem + coerce_address(addr))) = val; }

	u16 readBus, writeBus;

	

private:
	//Pointer to the underlying memory array
	u8 *_mem;
	FORCEINLINE const u16 coerce_address(u16 addr) {
		//These addresses are mirrored 0x2000 bytes lower
		if (addr >= 0xE000 && addr < 0xFE00) {
			//Turn off bit 13 to convert it to range 0xC000 to 0xDFFF
			addr &= 0xDFFF;
		}

		return addr;
	}
};

} /*namespace MXGB*/