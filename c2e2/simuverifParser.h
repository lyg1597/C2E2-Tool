#pragma once
#include <vector>
using namespace std;
#include "simuverifScanner.h"

class Parser {
	public:
			Parser():parser(scanner){ } 
		
			int parse() {
				return parser.parse();
			}

		FlexScanner scanner;
		yy::BisonParser parser;
	
	private:
};
