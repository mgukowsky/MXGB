var _opInfo = require("./OpInfo.js");

var OpInfo = _opInfo.OpInfo;
var opgroups = _opInfo.opgroups;

//Maps enum values to strings representing operation groups
var OP_NAME_MAP = {};
OP_NAME_MAP[opgroups.LD_R_N] = "LD_R_N";

//Class responsible for creating the actual C++ code
var OpCreator = function () { };

var OP_SIGNATURE = "static const u8 CPU::";

//Create the unique symbol for the callback
OpCreator.prototype.getOpName = function (opcode) {
	var opFields = OpInfo[opcode];

	if (!opFields) {
		return "ERR";
	} else {
		return `${OP_NAME_MAP[opFields.opgroup]}_at_0x${opcode.toString(16).toUpperCase()}`;
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


//Create the callback's return statement
function makeCycleReturn(numCycles) {
	return `\n\treturn ${numCycles};`;
}


module.exports.OpCreator = OpCreator;