/*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 2 example flex file
// created: 9-22-2015
//  author: J. Edward Swan II
*****************************************************************************/
%option noyywrap
%s instring
%{
#include "lexer.h"
int yyline = 1;
%}

%%
if {
	return TOK_IF;
}

else {
	return TOK_ELSE;
}

for {
	return TOK_FOR;
}

while {
	return TOK_WHILE;
}

print {
	return TOK_PRINT;
}

return {
	return TOK_RETURN;
}

continue {
	return TOK_CONTINUE;
}

break {
	return TOK_BREAK;
}

debug {
	return TOK_DEBUG;
}

read {
	return TOK_READ;
}

let {
	return TOK_LET;
}

int {
	return TOK_INT;
}

float {
	return TOK_FLOAT;
}

string {
	return TOK_STRING;
}

and {
	return TOK_AND;
}

or {
	return TOK_OR;
}

not {
	return TOK_NOT;
}

length {
	return TOK_LENGTH;
}

[a-zA-Z][a-zA-Z0-9_]* {
	return TOK_IDENTIFIER;
}

[0-9]* {
	return TOK_FLOATLIT;
}

[0-9]+"."[0-9]* {
	return TOK_FLOATLIT;
}

\"[^\"]*\" {
	return TOK_STRINGLIT;
}

";" {
	return TOK_SEMICOLON;
}

"(" {
	return TOK_OPENPAREN;
}

")" {
	return TOK_CLOSEPAREN;
}

"[" {
	return TOK_OPENBRACKET;
}

"]" {
	return TOK_CLOSEBRACKET;
}

"{" {
	return TOK_OPENBRACE;
}

"}" {
	return TOK_CLOSEBRACE;
}

"," {
	return TOK_COMMA;
}

"+" {
	return TOK_PLUS;
}

"-" {
	return TOK_MINUS;
}

"*" {
	return TOK_MULTIPLY;
}

"/" {
	return TOK_DIVIDE;
}

":=" {
	return TOK_ASSIGN;
}

"<>" {
	return TOK_NOTEQUALTO;
}

"==" {
	return TOK_EQUALTO;
}

"<" {
	return TOK_LESSTHAN;
}

">" {
	return TOK_GREATERTHAN;
}

[ \t\r]+ ;

. {return TOK_UNKNOWN;}

\"[^\"]* {
          return TOK_EOF_SL;
}

[\n] {
	yyline++;
}

<<EOF>> { return TOK_EOF; }


