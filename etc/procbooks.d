module main;

import std.conv;
import std.format;
import std.algorithm;
import std.string;
import std.file;
import std.array;

void main() {
	string f = "books.txt".readText;
	
	string[] recs;
	
	foreach(string line; f.splitter("\n")) {
		string[] data = line.strip.splitter(",").array;
		
		recs ~= "{%s, BibleBook{%s, \"%s\", \"%s\", QStringList{\"%s\"}}}".format(data[0],data[0],data[1],data[2],data[3..$].joiner("\",\""));
	}
	
	write("booksout.txt","%s".format(recs.joiner(",")));
}