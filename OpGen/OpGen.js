//The 'main' file for the project

var _writer = require("./Writer.js");
var _opManager = require("./OpManager.js");

var Writer = _writer.Writer;
var OpManager = _opManager.OpManager;

(function main() {
	var opDefWriter = new Writer("../moc/ops.gen.h");
	var opVectorWriter = new Writer("../moc/OpcodeVector.gen.h");
	var opDeclWriter = new Writer("../moc/opdecl.gen.h");
	var om = new OpManager();

	om.makeDeclarations().forEach(el => opDeclWriter.writeln(el));
	opVectorWriter.writeln("");
	opVectorWriter.write(om.makeOpVector());

	opDefWriter.write(om.makeDefinitions());

	opDefWriter.close();
	opVectorWriter.close();
	opDeclWriter.close();

	process.exit(0);
})();