//This file should be #included as the body of the definition of Testrunner::run_op_cpu_tests.
//Done entirely in macros for ease of insertion inside the function definition.

//These macros must match EXACTLY as they appear in CPU.cpp
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

//Magic numbers for testing
#define MAGIC_NUMBER		0xAB
#define MAGIC_ADDRESS		0xACED

#define TEST_LD_R_N(OPCODE, REG_TO_TEST) \
	b.reset(); \
	c.reset(); \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	b.write8(GB_ENTRY_POINT + 1, MAGIC_NUMBER); \
	EXPECT_EQ(8, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 8 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, c.REG_TO_TEST) << "Opcode " << OPCODE << " does not load an 8-bit value into " << #REG_TO_TEST;

#define TEST_LD_R_R(OPCODE, REG_DST, REG_SRC) \
	b.reset(); \
	c.reset(); \
	c.REG_SRC = MAGIC_NUMBER; \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	EXPECT_EQ(4, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 4 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, c.REG_DST) << "Opcode " << OPCODE << " does transfer to the appropriate register; " \
																			<< #REG_SRC << ": " << c.REG_SRC << ", " << #REG_DST << ": " << c.REG_DST; \
	EXPECT_EQ(MAGIC_NUMBER, c.REG_SRC) << "Opcode " << OPCODE << " does not allow src(" << #REG_SRC << ") to retain its value";

#define TEST_LD_R_indirectHL(OPCODE, REG_TO_TEST) \
	b.reset(); \
	c.reset(); \
	c.reg16HL = MAGIC_ADDRESS; \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	b.write8(MAGIC_ADDRESS, MAGIC_NUMBER); \
	EXPECT_EQ(8, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 8 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, c.REG_TO_TEST)  << "Opcode " << OPCODE << " does not load an 8-bit value at (HL) into " << #REG_TO_TEST;

#define TEST_LD_indirectHL_R(OPCODE, REG_TO_TEST) \
	b.reset(); \
	c.reset(); \
	c.reg16HL = MAGIC_ADDRESS; \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	c.REG_TO_TEST = MAGIC_NUMBER; \
	EXPECT_EQ(8, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 8 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, b.read8(MAGIC_ADDRESS))  << "Opcode " << OPCODE << " does not write an 8-bit value in " << #REG_TO_TEST << " to memory";

#define TEST_LD_indirectHL_N(OPCODE) \
	b.reset(); \
	c.reset(); \
	c.reg16HL = MAGIC_ADDRESS; \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	b.write8(GB_ENTRY_POINT+1, MAGIC_NUMBER); \
	EXPECT_EQ(12, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 12 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, b.read8(MAGIC_ADDRESS))  << "Opcode " << OPCODE << " does not write an immediate 8-bit value to the address in regHL";

#define TEST_LD_A_indirect(OPCODE, REG_DST) \
	b.reset(); \
	c.reset(); \
	c.REG_DST = MAGIC_ADDRESS; \
	b.write8(MAGIC_ADDRESS, MAGIC_NUMBER); \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	EXPECT_EQ(8, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 8 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, c.regA)  << "Opcode " << OPCODE << " does not load the value found at the address in " << #REG_DST << " into regA";

#define TEST_LD_A_indirectImm(OPCODE) \
	b.reset(); \
	c.reset(); \
	b.write16(c.reg16PC+1, MAGIC_ADDRESS); \
	b.write8(MAGIC_ADDRESS, MAGIC_NUMBER); \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	EXPECT_EQ(16, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 16 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, c.regA)  << "Opcode " << OPCODE << " does not load the value found at the immediate 16-bit address into regA";

#define TEST_LD_indirect_A(OPCODE, REG_SRC) \
	b.reset(); \
	c.reset(); \
	c.REG_SRC = MAGIC_ADDRESS; \
	c.regA = MAGIC_NUMBER; \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	EXPECT_EQ(8, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 8 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, b.read8(MAGIC_ADDRESS))  << "Opcode " << OPCODE << " does not load the value in regA to the memory address in " << #REG_SRC;

#define TEST_LD_indirectImm_A(OPCODE) \
	b.reset(); \
	c.reset(); \
	b.write16(c.reg16PC+1, MAGIC_ADDRESS); \
	c.regA = MAGIC_NUMBER; \
	b.write8(GB_ENTRY_POINT, OPCODE); \
	EXPECT_EQ(16, c.execute_next()) << "Opcode " << OPCODE << " does not execute in 16 cycles"; \
	EXPECT_EQ(MAGIC_NUMBER, b.read8(MAGIC_ADDRESS))  << "Opcode " << OPCODE << " does not load the value in regA to the immediate 16-bit address";

Bus b;
CPU c(b);

//Load a register with an immediate value
TEST_LD_R_N(0x3E, regA);
TEST_LD_R_N(0x06, regB);
TEST_LD_R_N(0x0E, regC);
TEST_LD_R_N(0x16, regD);
TEST_LD_R_N(0x1E, regE);
TEST_LD_R_N(0x26, regH);
TEST_LD_R_N(0x2E, regL);

//Load regA with another register or a value in memory
TEST_LD_R_R(0x78, regA, regB);
TEST_LD_R_R(0x79, regA, regC);
TEST_LD_R_R(0x7A, regA, regD);
TEST_LD_R_R(0x7B, regA, regE);
TEST_LD_R_R(0x7C, regA, regH);
TEST_LD_R_R(0x7D, regA, regL);
TEST_LD_R_indirectHL(0x7E, regA);
TEST_LD_R_R(0x7F, regA, regA);

//Load regB with another register or a value in memory
TEST_LD_R_R(0x40, regB, regB);
TEST_LD_R_R(0x41, regB, regC);
TEST_LD_R_R(0x42, regB, regD);
TEST_LD_R_R(0x43, regB, regE);
TEST_LD_R_R(0x44, regB, regH);
TEST_LD_R_R(0x45, regB, regL);
TEST_LD_R_indirectHL(0x46, regB);
TEST_LD_R_R(0x47, regB, regA);

//Load regC with another register or a value in memory
TEST_LD_R_R(0x48, regC, regB);
TEST_LD_R_R(0x49, regC, regC);
TEST_LD_R_R(0x4A, regC, regD);
TEST_LD_R_R(0x4B, regC, regE);
TEST_LD_R_R(0x4C, regC, regH);
TEST_LD_R_R(0x4D, regC, regL);
TEST_LD_R_indirectHL(0x4E, regC);
TEST_LD_R_R(0x4F, regC, regA);

//Load regD with another register or a value in memory
TEST_LD_R_R(0x50, regD, regB);
TEST_LD_R_R(0x51, regD, regC);
TEST_LD_R_R(0x52, regD, regD);
TEST_LD_R_R(0x53, regD, regE);
TEST_LD_R_R(0x54, regD, regH);
TEST_LD_R_R(0x55, regD, regL);
TEST_LD_R_indirectHL(0x56, regD);
TEST_LD_R_R(0x57, regD, regA);

//Load regE with another register or a value in memory
TEST_LD_R_R(0x58, regE, regB);
TEST_LD_R_R(0x59, regE, regC);
TEST_LD_R_R(0x5A, regE, regD);
TEST_LD_R_R(0x5B, regE, regE);
TEST_LD_R_R(0x5C, regE, regH);
TEST_LD_R_R(0x5D, regE, regL);
TEST_LD_R_indirectHL(0x5E, regE);
TEST_LD_R_R(0x5F, regE, regA);

//Load regH with another register or a value in memory
TEST_LD_R_R(0x60, regH, regB);
TEST_LD_R_R(0x61, regH, regC);
TEST_LD_R_R(0x62, regH, regD);
TEST_LD_R_R(0x63, regH, regE);
TEST_LD_R_R(0x64, regH, regH);
TEST_LD_R_R(0x65, regH, regL);
TEST_LD_R_indirectHL(0x66, regH);
TEST_LD_R_R(0x67, regH, regA);

//Load regL with another register or a value in memory
TEST_LD_R_R(0x68, regL, regB);
TEST_LD_R_R(0x69, regL, regC);
TEST_LD_R_R(0x6A, regL, regD);
TEST_LD_R_R(0x6B, regL, regE);
TEST_LD_R_R(0x6C, regL, regH);
TEST_LD_R_R(0x6D, regL, regL);
TEST_LD_R_indirectHL(0x6E, regL);
TEST_LD_R_R(0x6F, regL, regA);

//Write a value in a register to the memory address in regHL
TEST_LD_indirectHL_R(0x70, regB);
TEST_LD_indirectHL_R(0x71, regC);
TEST_LD_indirectHL_R(0x72, regD);
TEST_LD_indirectHL_R(0x73, regE);

//These two opcodes MUST be tested separately, as they are writing a value to the same address they hold.
//For example if regHL = 0xACED, and we call <LD (HL), regH> then the value at 0xACED will be 0xAC. This breaks our system of checking
//for a magic value at a magic memory address, as writing the magic value to the register will also change the address the CPU will write to.
//TEST_LD_indirectHL_R(0x74, regH);
//TEST_LD_indirectHL_R(0x75, regL);
TEST_LD_indirectHL_R(0x77, regA);

TEST_LD_indirectHL_N(0x36);

TEST_LD_A_indirect(0x0A, reg16BC);
TEST_LD_A_indirect(0x1A, reg16DE);
TEST_LD_A_indirectImm(0xFA);

TEST_LD_indirect_A(0x02, reg16BC);
TEST_LD_indirect_A(0x12, reg16DE);
TEST_LD_indirectImm_A(0xEA);

//One-offs can just be defined inline

//LD A, (C)
b.reset();
c.reset();
b.write8(GB_ENTRY_POINT, 0xF2);
b.write8(0xFFCD, MAGIC_NUMBER);
c.regC = 0xCD;
EXPECT_EQ(8, c.execute_next()) << "Opcode " << 0xF2 << " does not execute in 8 cycles";
EXPECT_EQ(MAGIC_NUMBER, c.regA) << "Opcode " << 0xF2 << " does not load an address in high RAM into regA";

//LD (C), A
b.reset();
c.reset();
b.write8(GB_ENTRY_POINT, 0xE2);
c.regA = MAGIC_NUMBER;
c.regC = 0xCD;
EXPECT_EQ(8, c.execute_next()) << "Opcode " << 0xE2 << " does not execute in 8 cycles";
EXPECT_EQ(MAGIC_NUMBER, b.read8(0xFFCD)) << "Opcode " << 0xE2 << " does not load the value in regA into an address in high RAM";

//LDD A, (HL)
b.reset();
c.reset();
c.reg16HL = MAGIC_ADDRESS;
b.write8(GB_ENTRY_POINT, 0x3A);
b.write8(MAGIC_ADDRESS, MAGIC_NUMBER);
EXPECT_EQ(8, c.execute_next()) << "Opcode " << 0x3A << " does not execute in 8 cycles";
EXPECT_EQ(MAGIC_NUMBER, c.regA) << "Opcode " << 0x3A << " does not load an 8-bit value at (HL) into regA";
EXPECT_EQ(MAGIC_ADDRESS - 1, c.reg16HL) << "Opcode " << 0x3A << " does not decrement reg16HL after performing the indirect load";

//LDD (HL), A
b.reset();
c.reset();
c.reg16HL = MAGIC_ADDRESS;
b.write8(GB_ENTRY_POINT, 0x32);
c.regA = MAGIC_NUMBER;
EXPECT_EQ(8, c.execute_next()) << "Opcode " << 0x32 << " does not execute in 8 cycles";
EXPECT_EQ(MAGIC_NUMBER, b.read8(MAGIC_ADDRESS)) << "Opcode " << 0x32 << " does not load the value in regA into (HL)";
EXPECT_EQ(MAGIC_ADDRESS - 1, c.reg16HL) << "Opcode " << 0x32 << " does not decrement reg16HL after performing the indirect load";

//LDI A, (HL)
b.reset();
c.reset();
c.reg16HL = MAGIC_ADDRESS;
b.write8(GB_ENTRY_POINT, 0x2A);
b.write8(MAGIC_ADDRESS, MAGIC_NUMBER);
EXPECT_EQ(8, c.execute_next()) << "Opcode " << 0x2A << " does not execute in 8 cycles";
EXPECT_EQ(MAGIC_NUMBER, c.regA) << "Opcode " << 0x2A << " does not load an 8-bit value at (HL) into regA";
EXPECT_EQ(MAGIC_ADDRESS + 1, c.reg16HL) << "Opcode " << 0x2A << " does not increment reg16HL after performing the indirect load";

//LDI (HL), A
b.reset();
c.reset();
c.reg16HL = MAGIC_ADDRESS;
b.write8(GB_ENTRY_POINT, 0x22);
c.regA = MAGIC_NUMBER;
EXPECT_EQ(8, c.execute_next()) << "Opcode " << 0x22 << " does not execute in 8 cycles";
EXPECT_EQ(MAGIC_NUMBER, b.read8(MAGIC_ADDRESS)) << "Opcode " << 0x22 << " does not load the value in regA into (HL)";
EXPECT_EQ(MAGIC_ADDRESS + 1, c.reg16HL) << "Opcode " << 0x22 << " does not increment reg16HL after performing the indirect load";

//LD A, ($FF00+n)
b.reset();
c.reset();
b.write8(GB_ENTRY_POINT, 0xF0);
b.write8(GB_ENTRY_POINT+1, 0xCD);
b.write8(0xFFCD, MAGIC_NUMBER);
EXPECT_EQ(12, c.execute_next()) << "Opcode " << 0xF0 << " does not execute in 12 cycles";
EXPECT_EQ(MAGIC_NUMBER, c.regA) << "Opcode " << 0xF0 << " does not load an address in high RAM into regA";

//LD ($FF00+n), A
b.reset();
c.reset();
b.write8(GB_ENTRY_POINT, 0xE0);
b.write8(GB_ENTRY_POINT + 1, 0xCD);
c.regA = MAGIC_NUMBER;
EXPECT_EQ(12, c.execute_next()) << "Opcode " << 0xE2 << " does not execute in 12 cycles";
EXPECT_EQ(MAGIC_NUMBER, b.read8(0xFFCD)) << "Opcode " << 0xE2 << " does not load the value in regA into an address in high RAM";


//Remove our macros
#undef regA		
#undef regF
#undef regB
#undef regC
#undef regD
#undef regE
#undef regH
#undef regL

#undef reg16AF
#undef reg16BC
#undef reg16DE
#undef reg16HL
#undef reg16PC
#undef reg16SP

#undef GB_ENTRY_POINT
#undef INITIAL_STACK_POSITION