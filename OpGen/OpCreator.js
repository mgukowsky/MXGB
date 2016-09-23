var _opInfo = require("./OpInfo.js");

var OpInfo = _opInfo.OpInfo;
var opgroups = _opInfo.opgroups;

//Maps enum values to strings representing operation groups
var OP_NAME_MAP = {};
OP_NAME_MAP[opgroups.LD_R_N] = "LD_R_N";
OP_NAME_MAP[opgroups.LD_R_R] = "LD_R_R";
OP_NAME_MAP[opgroups.LD_R_indirectHL] = "LD_R_indirectHL";
OP_NAME_MAP[opgroups.LD_indirectHL_R] = "LD_indirectHL_R";
OP_NAME_MAP[opgroups.LD_indirectHL_N] = "LD_indirectHL_N";
OP_NAME_MAP[opgroups.LD_A_indirect] = "LD_A_indirect";
OP_NAME_MAP[opgroups.LD_indirect_A] = "LD_indirect_A";
OP_NAME_MAP[opgroups.LD_A_high_ram] = "LD_A_high_ram";
OP_NAME_MAP[opgroups.LD_high_ram_A] = "LD_high_ram_A";
OP_NAME_MAP[opgroups.LDD] = "LDD";
OP_NAME_MAP[opgroups.LDI] = "LDI";
OP_NAME_MAP[opgroups.LD_high_ram_imm] = "LD_high_ram_imm";

//Class responsible for creating the actual C++ code
var OpCreator = function () { };

//Create the unique symbol for the callback
OpCreator.prototype.getOpName = function (opcode) {
	var opFields = OpInfo[opcode];

	if (!opFields) {
		return "ERR";
	} else {
		return `${OP_NAME_MAP[opFields.opgroup]}_${opFields.dst}_${opFields.src}_at_0x${opcode.toString(16).toUpperCase()}`;
	}
}

//Determine how to create the string representation of the callback's definition.
OpCreator.prototype.makeOp = function (opcode) {
	var opFields = OpInfo[opcode];
	var opBody, cyclesTaken;

	if (!opFields) {
		return this.getErrFuncString();
	}

	switch (opFields.opgroup) {
		case opgroups.LD_R_N:
			opBody = makeFunc_LD_R_N(opFields);
			cyclesTaken = 8;
			break;

		case opgroups.LD_R_R:
			opBody = makeFunc_LD_R_R(opFields);
			cyclesTaken = 4;
			break;

		case opgroups.LD_R_indirectHL:
			opBody = makeFunc_LD_R_indirectHL(opFields);
			cyclesTaken = 8;
			break;

		case opgroups.LD_indirectHL_R:
			opBody = makeFunc_LD_indirectHL_R(opFields);
			cyclesTaken = 8;
			break;

		case opgroups.LD_indirectHL_N:
			opBody = makeFunc_LD_indirectHL_N(opFields);
			cyclesTaken = 12;
			break;

		case opgroups.LD_A_indirect:
			opBody = makeFunc_LD_A_indirect(opFields);
			cyclesTaken = (opFields.src === "INDIMM") ? 16 : 8;
			break;

		case opgroups.LD_indirect_A:
			opBody = makeFunc_LD_indirect_A(opFields);
			cyclesTaken = (opFields.dst === "INDIMM") ? 16 : 8;
			break;

		case opgroups.LD_A_high_ram:
			opBody = makeFunc_LD_A_high_ram(opFields);
			cyclesTaken = 8;
			break;

		case opgroups.LD_high_ram_A:
			opBody = makeFunc_LD_high_ram_A(opFields);
			cyclesTaken = 8;
			break;

		case opgroups.LDD:
			opBody = makeFunc_LDD(opFields);
			cyclesTaken = 8;
			break;

		case opgroups.LDI:
			opBody = makeFunc_LDI(opFields);
			cyclesTaken = 8;
			break;

		case opgroups.LD_high_ram_imm:
			opBody = makeFunc_LD_high_ram_imm(opFields);
			cyclesTaken = 12;
			break;

		default:
			console.error("Invalid opgroup value: " + opFields.opgroup);
	}

	return this.makeOpString(this.getOpName(opcode), opBody + makeCycleReturn(cyclesTaken));
}

//ERR function is a special case
OpCreator.prototype.getErrFuncString = function () {
	return this.makeOpString("ERR", "\trefCore.alert_err(\"Bad Opcode\");\n\treturn 2;");
}

//Create the actual string representation of the callback definitio-n
OpCreator.prototype.makeOpString = function (opName, opBody) {
	return this.makeOpHeaderString(opName) + opBody + "\n" + this.makeOpFooterString();
}

//Simply returns the string which makes up the first line of the callback's definition
OpCreator.prototype.makeOpHeaderString = function(op){
	return `const u8 CPU::${op}(){\n`;
}

//Closes out the callback's definition
OpCreator.prototype.makeOpFooterString = function () {
	return '}\n';
}

//Load the byte following the PC into an 8 bit register
function makeFunc_LD_R_N(opFields) {
	var src, pcIncrement = 1;
	if (opFields.src === "IMM") {
		//Get the immediate byte after the PC
		src = "refBus.read8(reg16PC + 1)";
		++pcIncrement;
	} else {
		src = opFields.src;
	}
	return `\t${opFields.dst} = ${src};\n\treg16PC += ${pcIncrement};\n`;
}

//Copy the source register into the destination register
function makeFunc_LD_R_R(opFields) {
	return `\t${opFields.dst} = ${opFields.src};\n\t++reg16PC;\n`;
}

//Move the value at the address formed by (regH << 8 | regL) into the destination register
function makeFunc_LD_R_indirectHL(opFields) {
	return `\t${opFields.dst} = refBus.read8(reg16HL);\n\t++reg16PC;\n`;
}

//Move the value in the source register to the address formed by (regH << 8 | regL)
function makeFunc_LD_indirectHL_R(opFields) {
	return `\trefBus.write8(reg16HL, ${opFields.src});\n\t++reg16PC;\n`;
}

//Move an immediate 8-bit value to the address contained in regHL
function makeFunc_LD_indirectHL_N(opFields) {
	return `\trefBus.write8(reg16HL, refBus.read8(reg16PC+1));\n\treg16PC += 2;\n`;
}

//Load an indirect 8-bit value into regA
function makeFunc_LD_A_indirect(opFields) {
	var src, pcIncrement = 1;
	if (opFields.src === "INDIMM") {
		src = "refBus.read16(reg16PC + 1)";
		pcIncrement = 3;
	} else {
		src = opFields.src;
	}
	return `\tregA = refBus.read8(${src});\n\treg16PC += ${pcIncrement};\n`;
}

//Write the value in regA to an indirect address
function makeFunc_LD_indirect_A(opFields) {
	var dst, pcIncrement = 1;
	if (opFields.dst === "INDIMM") {
		dst = "refBus.read16(reg16PC + 1)";
		pcIncrement = 3;
	} else {
		dst = opFields.dst;
	}
	return `\trefBus.write8(${dst}, regA);\n\treg16PC += ${pcIncrement};\n`;
}

//Form an address with the base of the highest RAM page, 0xFF00, plus the value in regC, and load the value at that address into regA
function makeFunc_LD_A_high_ram(opFields) {
	return `\tregA = refBus.read8(0xFF00 | regC);\n\t++reg16PC;\n`;
}

//Write the value in regA to the address formed by the base of the highest RAM page, 0xFF00, plus the value in regC
function makeFunc_LD_high_ram_A(opFields) {
	return `\trefBus.write8(0xFF00 | regC, regA);\n\t++reg16PC;\n`;
}

function makeFunc_LDD(opFields) {
	return _makeFunc_LDD_or_LDI(opFields, false);
}

function makeFunc_LDI(opFields) {
	return _makeFunc_LDD_or_LDI(opFields, true);
}

//A transfer between the value pointed to by reg16HL and regA. Afterwards, HL is incremented or decremented.
function _makeFunc_LDD_or_LDI(opFields, shouldInc) {
	var moveClause, incOrDecString;

	if (opFields.dst === "IND") {
		moveClause = "refBus.write8(reg16HL, regA);";
	} else {
		moveClause = "regA = refBus.read8(reg16HL);";
	}

	incOrDecString = (shouldInc) ? "++" : "--";

	return `\t${moveClause}\n\t${incOrDecString}reg16HL;\n\t++reg16PC;\n`;
}

//Transfers between regA and the value at the address formed by $FF00 (the last page in RAM) plus an immediate 8-bit offset
function makeFunc_LD_high_ram_imm(opFields) {
	var moveClause;

	if (opFields.dst === "regA") {
		moveClause = "regA = refBus.read8(0xFF00 | refBus.read8(reg16PC+1));";
	} else {
		moveClause = "refBus.write8(0xFF00 | refBus.read8(reg16PC+1), regA);";
	}

	return `\t${moveClause}\n\treg16PC += 2;\n`;
}

//Create the callback's return statement
function makeCycleReturn(numCycles) {
	return `\n\treturn ${numCycles};`;
}


module.exports.OpCreator = OpCreator;