digit [0-9]
number ([0-9]*|-[0-9]*)|([0-9]*|-[0-9]*)e([0-9]*|-[0-9]*)|([0-9]*|-[0-9]*)\.([0-9]*)
%{
#include <stdio.h>
#include "simuverifScanner.h"
void yyerror(const char* );

%}

%option nodefault yyclass="FlexScanner" noyywrap c++

%%
[ \t\r\n]+ ;
dimensions			{return yy::BisonParser::token::DIMENSIONS;}
simulator			{return yy::BisonParser::token::SIMULATOR;}
abs-error			{return yy::BisonParser::token::ABSERROR;}
rel-error			{return yy::BisonParser::token::RELERROR;}
init				{return yy::BisonParser::token::INIT;}
init-eqns			{return yy::BisonParser::token::INITE;}
init-matrix			{return yy::BisonParser::token::INITM;}
init-b				{return yy::BisonParser::token::INITB;}
init-mode			{return yy::BisonParser::token::INITMODE;}
time-step			{return yy::BisonParser::token::TSTEP;}
time-horizon			{return yy::BisonParser::token::THORIZON;}
delta				{return yy::BisonParser::token::DELTA;}
forbidden			{return yy::BisonParser::token::FORB;}
unsafe-eqns			{return yy::BisonParser::token::FORBE;}
unsafe-matrix		{return yy::BisonParser::token::FORBM;}
unsafe-b			{return yy::BisonParser::token::FORBB;}
annotation-type			{return yy::BisonParser::token::ANNOTTYPE;}
annotation-mode			{return yy::BisonParser::token::ANNOTMODE;}
annotation			{return yy::BisonParser::token::ANNOT;}
is_linear		{return yy::BisonParser::token::ISLINEAR;}
k				{return yy::BisonParser::token::KCONST;}
time				{return yy::BisonParser::token::TIME;}
gamma				{return yy::BisonParser::token::GAMMA;}
beta				{return yy::BisonParser::token::BETA;}
contraction			{return yy::BisonParser::token::CONTRACTION;}
linear				{return yy::BisonParser::token::LINEAR;}
exponential			{return yy::BisonParser::token::EXPONENTIAL;}
visualize			{return yy::BisonParser::token::VISU;}
to				{return yy::BisonParser::token::TO;}
=				{return yy::BisonParser::token::EQ;}
modes			{return yy::BisonParser::token::MODES;}
\"|\'				{return yy::BisonParser::token::QU;}
in				{return yy::BisonParser::token::IN;}
\[				{return yy::BisonParser::token::SO;}
\]				{return yy::BisonParser::token::SC;}
x[1-9][0-9]*			{yylval->intVal = atoi(yytext+1); return yy::BisonParser::token::XID;}
all				{return yy::BisonParser::token::ALL;}
{number}			{yylval->doubVal = atof(yytext); return yy::BisonParser::token::NUMBER;}
,				{return yy::BisonParser::token::CO;}
\'.*\'				{char* newStr; newStr = (char*)malloc((strlen(yytext)-1)*sizeof(char)); strcpy(newStr,yytext+1); newStr[strlen(newStr)-2] = '\0'; yylval->str = newStr; return yy::BisonParser::token::STRING;}
[a-zA-Z][a-zA-Z0-9\)\(\+]*    	{char* newStr; newStr = (char*)malloc(strlen(yytext)*sizeof(char)); strcpy(newStr,yytext); yylval->str = newStr; return yy::BisonParser::token::WORD;}
.				{printf("- Error while parsing - %s - ",yytext); return -1;} 
