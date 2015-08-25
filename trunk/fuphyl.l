%{

/* Get Token IDs from Bison Output */
#include "fuphyl.tab.h"

/*
#include "stream_yy_input.h"

#define YY_DECL int yylex(void *stream)

#define YY_INPUT(buf,result,max_size) \
    \
    result = YY_NULL; \
    stream_yy_input(stream, buf, &result, max_size); \
   
}
*/

YYLTYPE yylloc = {1,0,1,0};
YYSTYPE yylval;

int nestlevel = 0;

#define UPDATE_LVAL \
	++yylloc.first_column; \
	yylloc.last_column = yylloc.first_column + yyleng

#define YY_USER_ACTION \
	UPDATE_LVAL;

#define LOC_NEXT_LINE \
	++yylloc.first_line; \
	++yylloc.last_line; \
	yylloc.first_column = 0; \
	yylloc.last_column = 0

// %option never-interactive
// %option prefix="fuphyl"
%}

%option stack

%option noyywrap

%option case-sensitive

%x nestedcomment
%x string
%x string_done
%x atom
%x atom_done
%x sep_eater

%%

":" {
	return SYN_TYPE;
}
"?" {
	yy_push_state(sep_eater);
	return SYN_COND;
}
";" {
	yy_push_state(sep_eater);
	return SYN_END;
}
"->" {
	return SYN_RESULT;
}
"=" {
	yy_push_state(sep_eater);
	return SYN_ASSIGN;
}
<INITIAL>"," {
ECHO;
	yy_push_state(sep_eater);
	return SYN_SEP;
}
<sep_eater>"," {
	/* do nothing */
}

"(" {
	yy_push_state(sep_eater);
	return TUPLE_BEGIN;
}
")" {
	return TUPLE_END;
}
"{" {
	yy_push_state(sep_eater);
	return ARRAY_BEGIN;
}
"}" {
	return ARRAY_END;
}
"[" {
	yy_push_state(sep_eater);
	return LIST_BEGIN;
}
"]" {
	return LIST_END;
}

"+" {
	return OP_ADD;
}
"-" {
	return OP_SUBTRACT;
}
"*" {
	return OP_MULTIPLY;
}
"/" {
	return OP_DIVIDE;
}
"**" {
	return OP_POWER;
}

"&&" {
	return LOG_AND;
}
"&&&" {
	return LOG_ANDALSO;
}
"||" {
	return LOG_OR;
}
"|||" {
	return LOG_ORELSE;
}
"^^" {
	return LOG_XOR;
}
"!" {
	return LOG_NOT;
}

"<" {
	return REL_LT;
}
"<=" {
	return REL_LTE;
}
">" {
	return REL_GT;
}
">=" {
	return REL_GTE;
}
"==" {
	return REL_EQ;
}
"!=" {
	return REL_NEQ;
}

if {
	yy_push_state(sep_eater);
	return KEY_IF;
}
else {
	yy_push_state(sep_eater);
	return KEY_ELSE;
}

[[:digit:]][[:alnum:]_.]* {
	return LIT_NUMBER;
}

"\"" {
    BEGIN(string);
}
<string,string_done>"\"" {
	BEGIN(INITIAL);
}
<string>([^\\"[:cntrl:]]|("\\".))* {
	BEGIN(string_done);
	return LIT_STRING;
}
<string>. {
	BEGIN(string_done);
	return ERROR_STRING;
}
<string,string_done>"\n" {
	LOC_NEXT_LINE;
	return ERROR_STRING;
}
<string_done>. {
	return ERROR_STRING;
}

"'" {
	BEGIN(atom);
}
<atom>"'" {
	BEGIN(INITIAL);
	return ERROR_ATOM;
}
<atom_done>"'" {
	BEGIN(INITIAL);
}
<atom,atom_done>"\n" {
	LOC_NEXT_LINE;
	return ERROR_ATOM;
}
<atom>[[:print:]]+ {
	BEGIN(atom_done);
	return LIT_ATOM;
}
<atom,atom_done>. {
	return ERROR_ATOM;
}


[[:alpha:]_][[:alnum:]_]* {
ECHO;
	return LIT_IDENTIFIER;
}

<*>"\n" {
	LOC_NEXT_LINE;
	ECHO;
}

<*>[[:space:]] {
	/* do nothing */
}

<INITIAL,sep_eater>"//".*\n {
	/* do nothing */
}
<INITIAL,nestedcomment,sep_eater>"/*" {
 	++nestlevel;
 	if ( nestlevel == 1 ) {
 		yy_push_state(nestedcomment);
 	}
}
<nestedcomment>[^*/\n]+ {}
<nestedcomment>"*"+ {}
<nestedcomment>"*/" {
 	--nestlevel;
 	if ( !nestlevel ) {
		yy_pop_state();
 	}
}

<sep_eater>. {
	yyless( 0 );
	yy_pop_state();
}

<INITIAL>. {
	printf("unmatched"); ECHO;
}


%%

