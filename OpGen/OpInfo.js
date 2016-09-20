var LD = "LD";

var OpInfo = {};
//The 'reg' definitions must match the macro names in CPU.cpp
var regA = "regA";
var regB = "regB";
var regC = "regC";
var regD = "regD";
var regE = "regE";
var regH = "regH";
var regL = "regL";

var IMM = "IMM"; //Immediate
var IND = "IND"; //Indirect (HL)
var INDBC = "reg16BC"; //(BC)
var INDDE = "reg16DE"; //(DE)
var INDIMM = "INDIMM"; //(nn)

//Operations are categorized in many sources; we follow that convention here
var opgroups = {};
opgroups.LD_R_N = 0;
opgroups.LD_R_R = 1;
opgroups.LD_R_indirectHL = 2;
opgroups.LD_indirectHL_R = 3;
opgroups.LD_indirectHL_N = 4;
opgroups.LD_A_indirect = 5;
opgroups.LD_indirect_A = 6;
		
OpInfo[0x06] = { opgroup: opgroups.LD_R_N, dst: regB, src: IMM };
OpInfo[0x0E] = { opgroup: opgroups.LD_R_N, dst: regC, src: IMM };
OpInfo[0x16] = { opgroup: opgroups.LD_R_N, dst: regD, src: IMM };
OpInfo[0x1E] = { opgroup: opgroups.LD_R_N, dst: regE, src: IMM };
OpInfo[0x26] = { opgroup: opgroups.LD_R_N, dst: regH, src: IMM };
OpInfo[0x2E] = { opgroup: opgroups.LD_R_N, dst: regL, src: IMM };
OpInfo[0x3E] = { opgroup: opgroups.LD_R_N, dst: regA, src: IMM };

(function synthesizeLD_R_R_ops() {
	var REG_DICT = {};
	REG_DICT[regA] = 0b111;
	REG_DICT[regB] = 0b000;
	REG_DICT[regC] = 0b001;
	REG_DICT[regD] = 0b010;
	REG_DICT[regE] = 0b011;
	REG_DICT[regH] = 0b100;
	REG_DICT[regL] = 0b101;

	INDIRECT_HL_OP = 0b110;

	var REGS = [regA, regB, regC, regD, regE, regH, regL];
	REGS.forEach(regDst => {
		REGS.forEach(regSrc => {
			//LD r, r' has the binary signature: bits 0-2: src, bits 3-5: dst, bit6: 1, bit7: 0
			var opcode = 0x40 | REG_DICT[regDst] << 3 | REG_DICT[regSrc];
			OpInfo[opcode] = { opgroup: opgroups.LD_R_R, dst: regDst, src: regSrc };
		});
		//LD r, (HL) can be included as part of this grouping
		var indirectOpcode = 0x40 | REG_DICT[regDst] << 3 | INDIRECT_HL_OP;
		OpInfo[indirectOpcode] = { opgroup: opgroups.LD_R_indirectHL, dst: regDst, src: IND };

		//LD (HL), r can also be included here, if we condiser regDst to be the source
		var indirectToMemoryOpcode = 0x40 | INDIRECT_HL_OP << 3 | REG_DICT[regDst];
		OpInfo[indirectToMemoryOpcode] = { opgroup: opgroups.LD_indirectHL_R, dst: IND, src: regDst };
	});
})();

//LD (HL), n
OpInfo[0x36] = { opgroup: opgroups.LD_indirectHL_N, dst: IND, src: IMM };

//LD A, (XX)
OpInfo[0x0A] = { opgroup: opgroups.LD_A_indirect, dst: regA, src: INDBC };
OpInfo[0x1A] = { opgroup: opgroups.LD_A_indirect, dst: regA, src: INDDE };
OpInfo[0xFA] = { opgroup: opgroups.LD_A_indirect, dst: regA, src: INDIMM }; //GB exclusive

//LD (xx), A
OpInfo[0x02] = { opgroup: opgroups.LD_indirect_A, dst: INDBC, src: regA };
OpInfo[0x12] = { opgroup: opgroups.LD_indirect_A, dst: INDDE, src: regA };
OpInfo[0xEA] = { opgroup: opgroups.LD_indirect_A, dst: INDIMM, src: regA }; //GB exclusive

module.exports.OpInfo = OpInfo;
module.exports.opgroups = opgroups;