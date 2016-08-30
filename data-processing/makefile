all: wiki-parser

clean:
	rm -f wiki-parser

wiki-parser: wiki_parser.cc file.cc file.h
	g++ -std=c++14 -Ofast wiki_parser.cc file.cc -o wiki-parser

