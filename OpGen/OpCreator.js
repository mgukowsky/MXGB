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

//Class responsible for creating the actual C++ code
var OpCreator = function () { };

var OP_SIGNATURE = "static const u8 CPU::";

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
	}

	return this.makeOpString(this.getOpName(opcode), opBody + makeCycleReturn(cyclesTaken));
}

//ERR function is a special case
OpCreator.prototype.getErrFuncString = function () {
	return this.makeOpString("ERR", "\tthisCPU.refCore.alert_err(\"Bad Opcode\");\n\treturn 2;");
}

//Create the actual string representation of the callback definitio-n
OpCreator.prototype.makeOpString = function (opName, opBody) {
	return this.makeOpHeaderString(opName) + opBody + "\n" + this.makeOpFooterString();
}

//Simply returns the string which makes up the first line of the callback's definition
OpCreator.prototype.makeOpHeaderString = function(op){
	return `const u8 CPU::${op}(CPU &thisCPU){\n`;
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
		src = "thisCPU.refBus.read8(thisCPU.reg16PC + 1)";
		++pcIncrement;
	} else {
		src = opFields.src;
	}
	return `\tthisCPU.${opFields.dst} = ${src};\n\tthisCPU.reg16PC += ${pcIncrement};\n`;
}

//Copy the source register into the destination register
function makeFunc_LD_R_R(opFields) {
	return `\tthisCPU.${opFields.dst} = thisCPU.${opFields.src};\n\t++thisCPU.reg16PC;\n`;
}

//Move the value at the address formed by (regH << 8 | regL) into the destination register
function makeFunc_LD_R_indirectHL(opFields) {
	return `\tthisCPU.${opFields.dst} = thisCPU.refBus.read8(thisCPU.reg16HL);\n\t++thisCPU.reg16PC;\n`;
}

//Move the value in the source register to the address formed by (regH << 8 | regL)
function makeFunc_LD_indirectHL_R(opFields) {
	return `\tthisCPU.refBus.write8(thisCPU.reg16HL, thisCPU.${opFields.src});\n\t++thisCPU.reg16PC;\n`;
}

//Move an immediate 8-bit value to the address contained in regHL
function makeFunc_LD_indirectHL_N(opFields) {
	return `\tthisCPU.refBus.write8(thisCPU.reg16HL, thisCPU.refBus.read8(thisCPU.reg16PC+1));\n\tthisCPU.reg16PC += 2;\n`;
}

//Load an indirect 8-bit value into regA
function makeFunc_LD_A_indirect(opFields) {
	var src, pcIncrement = 1;
	if (opFields.src === "INDIMM") {
		src = "refBus.read16(thisCPU.reg16PC + 1)";
		pcIncrement = 3;
	} else {
		src = opFields.src;
	}
	return `\tthisCPU.regA = thisCPU.refBus.read8(thisCPU.${src});\n\tthisCPU.reg16PC += ${pcIncrement};\n`;
}

//Write the value in regA to an indirect address
function makeFunc_LD_indirect_A(opFields) {
	var dst, pcIncrement = 1;
	if (opFields.dst === "INDIMM") {
		dst = "refBus.read16(thisCPU.reg16PC + 1)";
		pcIncrement = 3;
	} else {
		dst = opFields.dst;
	}
	return `\tthisCPU.refBus.write8(thisCPU.${dst}, thisCPU.regA);\n\tthisCPU.reg16PC += ${pcIncrement};\n`;
}


//Create the callback's return statement
function makeCycleReturn(numCycles) {
	return `\n\treturn ${numCycles};`;
}


module.exports.OpCreator = OpCreator;