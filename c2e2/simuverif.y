%require "2.4.1"
%skeleton "lalr1.cc"
%defines
%define parser_class_name "BisonParser"
%parse-param { class FlexScanner &holder }
%lex-param   { class FlexScanner &holder }

%code requires {
	// Forward-declare the Scanner class; the Parser needs to be assigned a 
	// Scanner, but the Scanner can't be declared without the Parser
	// class Parser;
	class FlexScanner;
}

%code {
	// Prototype for the yylex function
	#include "simuverifParser.h"
	static int yylex(yy::BisonParser::semantic_type * yylval, class FlexScanner &holder);
}

%{

 #include <iostream>
 #include <fstream>
 #include <stdio.h>
 #include <vector>
 using namespace std;

// extern FILE *yyin;
// extern int yyparse();
// extern int yylex();
// void yyerror(const char *);

// vector<char*> biddenstr;

%}

%union{
  int intVal;
  double doubVal;
  char* str;
};

%token DIMENSIONS
%token MODES
%token SIMULATOR
%token ABSERROR
%token RELERROR
%token INIT
%token INITE
%token INITM
%token INITB
%token INITMODE
%token TSTEP
%token THORIZON
%token DELTA
%token FORB
%token FORBE
%token FORBM
%token FORBB
%token ANNOTTYPE
%token ANNOTMODE
%token ANNOT
%token KCONST
%token ISLINEAR
%token TIME
%token GAMMA
%token BETA
%token CONTRACTION
%token LINEAR
%token EXPONENTIAL
%token VISU
%token TO
%token EQ
%token QU
%token IN
%token SO
%token SC
%token ALL
%token <intVal> XID
%token <doubVal> NUMBER
%token CO
%token <str> STRING
%token <str> WORD

%%

%start ss;

ss:
	sentences
	;

sentences:
	sentence
	| sentence sentences
	;

sentence:
	DIMENSIONS EQ QU NUMBER QU {holder.dimensions = (int)$4; 
					holder.initMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.initMax = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMax = (double*)malloc(holder.dimensions*sizeof(double));
					printf(" dim - %d - \n",holder.dimensions);}
	| DIMENSIONS EQ NUMBER {holder.dimensions = (int)$3; 
					holder.initMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.initMax = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMin = (double*)malloc(holder.dimensions*sizeof(double));
					holder.forbMax = (double*)malloc(holder.dimensions*sizeof(double));
					printf(" dim - %d - \n",holder.dimensions);}
	| MODES EQ NUMBER {holder.modes = (int)$3; holder.kConst = (double*)malloc(holder.modes*sizeof(double));
					holder.gamma = (double*)malloc(holder.dimensions*sizeof(double));
					holder.islinear = (int*)malloc(holder.dimensions*sizeof(int));}
	| MODES EQ QU NUMBER QU {holder.modes = (int)$4; holder.kConst = (double*)malloc(holder.modes*sizeof(double));
					holder.gamma = (double*)malloc(holder.modes*sizeof(double));
					holder.islinear = (int*)malloc(holder.modes*sizeof(int));}
	| SIMULATOR EQ QU WORD QU {holder.simulator = (char*)$4; /*printf(" sim - %s -\n",$4);*/}
	| SIMULATOR EQ WORD {holder.simulator = (char*)$3; /*printf(" sim - %s -\n",$3);*/}
	| ABSERROR EQ QU NUMBER QU {holder.absoluteError = (double)$4; /*printf(" abserror - %f -\n",$4);*/}
	| RELERROR EQ QU NUMBER QU {holder.relativeError = (double)$4; /*printf(" relerror - %f -\n",$4);*/}
	| ABSERROR EQ NUMBER {holder.absoluteError = (double)$3; /*printf(" abserror - %f -\n",$3);*/}
	| RELERROR EQ NUMBER {holder.relativeError = (double)$3; /*printf(" relerror - %f -\n",$3);*/}
	| INITMODE EQ QU NUMBER QU {holder.initMode = (int)$4;}
	| INITMODE EQ NUMBER {holder.initMode = (int)$3;}
	| INIT EQ QU XID IN SO NUMBER CO NUMBER SC QU {int curdim; curdim = (int)$4; 
							*(holder.initMin+(curdim-1)) = $7; *(holder.initMax+(curdim-1)) = $9; 
							/*printf(" dim - %d - lower %f - higher %f -\n",curdim,$7,$9);*/}
	| INIT EQ XID IN SO NUMBER CO NUMBER SC {int curdim; curdim = (int)$3; 
							*(holder.initMin+(curdim-1)) = $6; *(holder.initMax+(curdim-1)) = $8; 
							/*printf(" dim - %d - lower %f - higher %f -\n",curdim,$6,$8);*/}
	| INITE EQ QU NUMBER QU {int numEqns; numEqns = (int)$4;
							holder.initEqns = numEqns;
							/*printf(" init equations - %d -\n",numEqns);*/}
	| INITE EQ NUMBER {int numEqns; numEqns = (int)$3;
							holder.initEqns = numEqns;
							/*printf(" init equations - %d -\n",numEqns);*/}
	| INITM EQ { holder.ptr = (double*)malloc(holder.dimensions*holder.initEqns*sizeof(double)); holder.index=0; printf("Starting init matrix");} SO numbersequence SC { holder.initMatrix = holder.ptr; printf(" Done parsing initial matrix \n");}
	| INITB EQ { holder.ptr = (double*)malloc(holder.initEqns*sizeof(double)); holder.index=0; printf("Starting init b");} SO numbersequence SC { holder.initB = holder.ptr; printf(" Done parsing initial B \n");}
	| DELTA EQ QU NUMBER QU {holder.deltaVal = (double)$4; /*printf(" delta - %f -\n",$4);*/}
	| TSTEP EQ QU NUMBER QU {holder.tStep = (double)$4; /*printf(" tstep - %f -\n",$4);*/}
	| THORIZON EQ QU NUMBER QU {holder.tGlobal = (double)$4; /*printf(" thorizon - %f -\n",$4);*/}
	| DELTA EQ NUMBER {holder.deltaVal = (double)$3; /*printf(" delta - %f -\n",$3);*/}
	| TSTEP EQ NUMBER {holder.tStep = (double)$3; /*printf(" tstep - %f -\n",$3);*/}
	| THORIZON EQ NUMBER {holder.tGlobal = (double)$3; /*printf(" thorizon - %f -\n",$3);*/}
	| FORB EQ QU XID IN SO NUMBER CO NUMBER SC QU {int curdim; curdim = (int)$4; 
							*(holder.forbMin+(curdim-1)) = $7; *(holder.forbMax+(curdim-1)) = $9; 
							/*holder.biddenstr.push_back($3); printf(" forbidden - %s -\n",$3);*/}
	| FORB EQ XID IN SO NUMBER CO NUMBER SC {int curdim; curdim = (int)$3; 
							*(holder.forbMin+(curdim-1)) = $6; *(holder.forbMax+(curdim-1)) = $8; 
							/*holder.biddenstr.push_back($3); printf(" forbidden - %s -\n",$3);*/}
	| FORBE EQ QU NUMBER QU {int numEqns; numEqns = (int)$4;
							holder.forbEqns = numEqns;
							/*printf(" forbidden equations - %d -\n",numEqns);*/}
	| FORBE EQ NUMBER {int numEqns; numEqns = (int)$3;
							holder.forbEqns = numEqns;
							/*printf(" forbidden - %d -\n",numEqns);*/}
	| FORBM EQ { holder.ptr = (double*)malloc(holder.dimensions*holder.forbEqns*sizeof(double)); holder.index=0; printf("Starting forb matrix");} SO numbersequence SC { holder.forbMatrix = holder.ptr; printf(" Done parsing forbidden matrix \n");}
	| FORBB EQ { holder.ptr = (double*)malloc(holder.forbEqns*sizeof(double)); holder.index=0; printf("Starting forb B");} SO numbersequence SC { holder.forbB = holder.ptr; printf(" Done parsing forbidden B \n");}
	| ANNOTMODE EQ QU NUMBER QU { holder.curMode = (int) $4;}
	| ANNOTMODE EQ NUMBER {holder.curMode = (int) $3;}
	| ANNOTTYPE EQ QU LINEAR QU {holder.typeAnnot = 2; /*printf(" annot - linear \n");*/}
	| ANNOTTYPE EQ QU CONTRACTION QU {holder.typeAnnot = 3; /*printf(" annot - contraction \n");*/}
	| ANNOTTYPE EQ QU EXPONENTIAL QU {holder.typeAnnot = 4; /*printf(" annot - exponential \n");*/}
	| ANNOTTYPE EQ LINEAR {holder.typeAnnot = 2; /*printf(" annot - linear \n");*/}
	| ANNOTTYPE EQ CONTRACTION {holder.typeAnnot = 3; /*printf(" annot - contraction \n");*/}
	| ANNOTTYPE EQ EXPONENTIAL {holder.typeAnnot = 4; /*printf(" annot - exponential \n");*/}
	| BETA EQ STRING {holder.beta = $3; /*printf(" beta - %s -\n",$3);*/}
	| ANNOT EQ STRING {holder.V = $3; /*printf(" annot - %s -\n",$3);*/}
	| ISLINEAR EQ QU NUMBER QU {*(holder.islinear+(holder.curMode-1)) = (int)$4;}
	| KCONST EQ QU NUMBER QU {*(holder.kConst+(holder.curMode-1)) = (double)$4; /*printf(" Kconst - %f -\n",$4);*/}
	| GAMMA EQ QU NUMBER QU {*(holder.gamma+(holder.curMode-1)) = (double)$4; /*printf(" Gamma - %f -\n",$4);*/}
	| KCONST EQ NUMBER {*(holder.kConst+(holder.curMode-1)) = (double)$3; /*printf(" Kconst - %f -\n",$3);*/}
	| GAMMA EQ NUMBER {*(holder.gamma+(holder.curMode-1)) = (double)$3; /*printf(" Gamma - %f -\n",$3);*/}
	| VISU XID CO XID TO WORD {holder.val1 = (int)$2; holder.val2 = (int)$4; holder.visuFile = (char*) $6; printf(" visualize - %d and %d to %s \n",$2,$4,$6);}
	| VISU ALL TO WORD {holder.val1 = -1; holder.val2 = -1; holder.visuFile = (char*) $4; printf(" visualize - -1 and -1 to %s \n",$4);}
	| VISU XID CO TIME TO WORD {holder.val1 = $2; holder.val2 = 0; holder.visuFile = (char*) $6; /*printf(" visualize - %d and time to %s \n",$2,$6);*/}
	;

numbersequence:
	NUMBER {double val = (double)$1; *(holder.ptr+holder.index) = val; printf("The value is %f and index is %d \n",val,holder.index); holder.index++;}
	| NUMBER {double val = (double)$1; *(holder.ptr+holder.index) = val; printf("The value is %f and index is %d \n",val,holder.index); holder.index++;} CO numbersequence
	;

%%


// We have to implement the error function
void yy::BisonParser::error(const std::string &msg) {
	std::cerr << "Error: " << msg << std::endl;
}


/*
void yyerror(const char* err_string)
{
	cout << "Parse error! " << err_string << endl;
	exit(0);

}
*/

// Now that we have the Parser declared, we can declare the Scanner and implement
// the yylex function

#include "simuverifScanner.h"

static int yylex(yy::BisonParser::semantic_type * yylval,  class FlexScanner &holder) {
	return holder.yylex(yylval);
}

/*
int tricky(int argc, char* argv[])
{
	if(argc == 1){
		cout << "Please enter a file name \n";
		exit(0);
	}

	
	FILE* myfile = fopen((char*)argv[1],"r");
	if(!myfile){
		cout << "Error in opening the file , please give a correct file" << endl;
		exit(0);
	}
	yyin = myfile;
	while(!feof(myfile)){
		yyparse();
	}

	cout << "IS this coming \n";

       int l,m;
       for(l=0;l<biddenstr.size();l++){
	 cout << " strings - " << biddenstr.at(l) << "\n";
       }

}

*/


