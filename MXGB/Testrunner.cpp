#ifdef TESTBUILD

#include <gtest/gtest.h>

#include "Testrunner.h"
#include "Bus.h"
#include "CPU.h"

using namespace MXGB;

//Hack which lets Google Test access private/protected members of tested classes
Testrunner *tr;

void Testrunner::run_all_tests() {
	//Expose this instance to the file;
	tr = this;
	//Runs all TEST() functions
	RUN_ALL_TESTS();
}

void Testrunner::run_basic_bus_tests() {
	Bus b;

	EXPECT_TRUE(b.is_mem_allocated()) << "Fails to allocate memory";

	EXPECT_EQ(0xC234, b.coerce_address(0xE234)) << "Does not properly coerce an address";
	EXPECT_EQ(0xDDFF, b.coerce_address(0xFDFF)) << "Does not properly coerce an address";
	EXPECT_EQ(0x1234, b.coerce_address(0x1234)) << "Incorrectly coerces an address";
	EXPECT_EQ(0xFE65, b.coerce_address(0xFE65)) << "Incorrectly coerces an address";

	b.write8(0x1234, 0xAB);
	EXPECT_EQ(0xAB, *(b._mem + 0x1234)) << "Does not write one byte";
	EXPECT_EQ(0xAB, b.read8(0x1234)) << "Does not read one byte";

	b.write16(0xABCD, 0x4321);
	EXPECT_EQ(0x4321, *(reinterpret_cast<u16*>(b._mem + 0xABCD))) << "Does not write two bytes";
	EXPECT_EQ(0x4321, b.read16(0xABCD)) << "Does not read two bytes";

	b.write16(0xFABC, 0x9876);
	EXPECT_EQ(0x9876, b.read16(0xFABC)) << "Does not read and write bytes to mirrored sections";
}

void Testrunner::run_basic_cpu_tests() {
	Bus b;
	CPU c(b);

	EXPECT_EQ(2, sizeof(CPU::Z80REG)) << "Does not correctly compile an emulated Z80 register to the correct size";

	c.regAF.whole = 0xABCD;
	EXPECT_EQ(0xAB, c.regAF.hibyte) << "Does not read the low byte of a register";
	EXPECT_EQ(0xCD, c.regAF.lobyte) << "Does not read the high byte of a register";

	c.regAF.hibyte = 0x98;
	c.regAF.lobyte = 0x76;
	EXPECT_EQ(0x9876, c.regAF.whole) << "Does not write the low and high bytes of a register";

	c.regAF.whole = 0;
	c.regAF.lobyte = 0x40;
	EXPECT_EQ(1, c.regAF.flagN) << "Does not correctly read a flag";

	c.regAF.flagZ = 1;
	c.regAF.flagN = 0;
	EXPECT_EQ(0x80, c.regAF.lobyte) << "Does not correctly write a flag";
	EXPECT_EQ(0x0080, c.regAF.whole) << "Does not correctly evaluate an entire 16 bit register";
}

//These macros create the rests which are run by RUN_ALL_TESTS();
TEST(Bustest, basic) {
	tr->run_basic_bus_tests();
}

TEST(CPUtest, basic) {
	tr->run_basic_cpu_tests();
}

#endif /*ifdef TESTBUILD*/