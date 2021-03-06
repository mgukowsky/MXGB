﻿var fs = require("fs");

var endl = "\n";

//Class to encapsulate file I/O
var Writer = function (path) {
	this.fd = fs.openSync(path, "w");

	this.writeCommentBorder();
	this.writeComment("This file was generated by OpGen.js");
	this.writeCommentBorder();
}

Writer.prototype.write = function (text) {
	fs.writeSync(this.fd, text);
}

Writer.prototype.writeln = function (text) {
	this.write(text + endl);
}

Writer.prototype.writeComment = function (comment) {
	this.writeln("//" + comment);
}

Writer.prototype.writeCommentBorder = function () {
	this.writeln("/" + ("*").repeat(80) + "/");
}

Writer.prototype.close = function () {
	fs.closeSync(this.fd);
}

module.exports.Writer = Writer;