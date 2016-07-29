var _writer = require("./Writer.js");
var Writer = _writer.Writer;

(function main() {
	var writer = new Writer("../moc/ops.h");
	writer.writeln("baz");
	writer.close();

	process.exit(0);
})();
