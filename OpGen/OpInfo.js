var LD = "LD";

var OpInfo = {};
//The 'reg' definitions must match the macro names in CPU.cpp
var regA = "regA";
var regB = "regB";
var regC = "regC";
var regD = "regD";

var IMM = "IMM";

//Operations are categorized in many sources; we follow that convention here
var opgroups = {};
opgroups.LD_R_N = 0;
		
OpInfo[0x06] = { opgroup: opgroups.LD_R_N, dst: regB, src: IMM };
OpInfo[0x0E] = { opgroup: opgroups.LD_R_N, dst: regC, src: IMM };
OpInfo[0x16] = { opgroup: opgroups.LD_R_N, dst: regD, src: IMM };
OpInfo[0x3E] = { opgroup: opgroups.LD_R_N, dst: regA, src: IMM };

module.exports.OpInfo = OpInfo;
module.exports.opgroups = opgroups;