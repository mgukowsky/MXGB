var _opInfo = require("./OpInfo.js");
var _opCreator = require("./OpCreator.js");

var OpInfo = _opInfo.OpInfo;
var OpCreator = _opCreator.OpCreator;

var oc = new OpCreator();

//Class to encapsulate logic about operations

var OpManager = function () { }

const TAB_SIZE = 2, NUM_TABS = 24;

//Returns an array containing the declarations of every operation callback
OpManager.prototype.makeDeclarations = function () {
	var qualifier = "static const u8";
	var decls = [];

	decls.push(`${qualifier} ERR(CPU &thisCPU);`);

	Object.keys(OpInfo).forEach(key => decls.push(`${qualifier} ${oc.getOpName(Number(key))}(CPU &thisCPU);`));

	return decls;
}

OpManager.prototype.makeOpVector = function () {

	var header = "const CPU::OpCallback CPU::OpcodeVector[0x100] = {\n";
	var footer = "\n};\n";

	var outputString = header;

	var i, opName;

	for (i = 0; i < 256; ++i) {
		opName = (OpInfo[i]) ? oc.getOpName(i) : "ERR";

		if (i && i % 4 === 0) {
			outputString += "\n\t";
		}

		if (!i || i % 8 === 0) {
			outputString += `\n\t//0x${i.toString(16).toUpperCase()}\n\t`;
		}

		//Append the function pointer, and space them appropriately (N.B. assumes tab size is equal to 2 spaces)
		outputString += opName + `,${('\t').repeat(NUM_TABS - (opName.length / TAB_SIZE))}`;
	}

	outputString += footer;

	return outputString;
}

OpManager.prototype.makeDefinitions = function () {
	var outputString = oc.getErrFuncString();

	Object.keys(OpInfo).forEach(key => outputString += oc.makeOp(Number(key)));

	return outputString;
}


module.exports.OpManager = OpManager;