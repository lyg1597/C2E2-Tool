#pragma once

// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

// Override the interface for yylex since we namespaced it
#undef YY_DECL
#define YY_DECL int FlexScanner::yylex()

// Include Bison for types / tokens
#include "simuverif.tab.h"
#include <vector>
using namespace std;


class FlexScanner : public yyFlexLexer {
	public:

//		vector <char*> biddenstr;

		char* simulator;
		int dimensions;
		int initEqns;
		int forbEqns;
		int index;
		double* initMatrix, *initB, *forbMatrix, *forbB;
		double* ptr;
		int modes;
		int initMode;
		int curMode;
		double* initMin, *initMax, *forbMin, *forbMax;
		double absoluteError, relativeError;
		double deltaVal, tStep, tGlobal;
		double* kConst, *gamma;
		int* islinear;
		int typeAnnot;
		char* V, * beta;
		int val1, val2;
		char* visuFile;

		// save the pointer to yylval so we can change it, and invoke scanner
		int yylex(yy::BisonParser::semantic_type * lval) { yylval = lval; return yylex(); }
		FlexScanner(){
			initMode = -1;
		}
	
	private:
		// Scanning function created by Flex; make this private to force usage
		// of the overloaded method so we can get a pointer to Bison's yylval
		int yylex();
		
		// point to yylval (provided by Bison in overloaded yylex)
		yy::BisonParser::semantic_type * yylval;
};

