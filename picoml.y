%{

#include <iostream>

%}

%code requires {

#include "picoml.h"

}

%code provides {

yy::parser::symbol_type yylex();

}

%language "c++"

%define parse.assert
%define api.token.constructor
%define api.value.type variant
%define api.value.automove

%parse-param { Evaluation &result }

%type	<Evaluation>	input
%type 	<Evaluation>	EvalConst
%type 	<Evaluation>	EvalVar
%type 	<Evaluation>	EvalPair
%type 	<Evaluation>	EvalPairFst
%type 	<Evaluation>	EvalPairSnd

%type	<Value>			value
%type	<Expression>	expr
%type	<Binding>		binding
%type	<Bindings>		binding_list
%type	<Environment>	env

%token	<Variable> 	VARIABLE
%token	<Value> 	CONST
%token	<BinOp>		OP

%token	EVAL VAL
%token 	IF THEN ELSE TRUE FALSE
%token	LET IN EQUAL
%token	FUN	MAPSTO
%token	ERROR

%%

input 	: EvalConst 	{ result = $1; }
		| EvalVar 		{ result = $1; }
		| EvalPair		{ result = $1; }
		| EvalPairFst 	{ result = $1; }
		| EvalPairSnd 	{ result = $1; }
		;

EvalConst
		: EVAL '(' CONST ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($CONST)); }
		;

EvalVar	: EVAL '(' VARIABLE ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalVar($VARIABLE, $env)); }
		;

EvalPair
		: EVAL '(' '(' VAL value[left] ',' VAL value[right] ')' ',' env ')'
			{ $$ = Evaluation::makeEvalPair($left, $right); }
		;

EvalPairFst
		: EVAL '(' '(' expr[left] ',' VAL value[right] ')' ',' env ')'
			{ $$ = Evaluation::makeEvalPairFst($left, $right); }
		;

EvalPairSnd
		: EVAL '(' '(' expr[left] ',' expr[right] ')' ',' env ')'
			{ $$ = Evaluation::makeEvalPairSnd($left, $right); }
		;

env		: '{' binding_list[bindings] '}'
			{ $$ = Environment($bindings); }
		;

binding : VARIABLE MAPSTO value
			{ $$ = Binding($VARIABLE, $value); }
		;

binding_list
		: %empty { $$ = Bindings(); }
		| binding
			{ $$ = Bindings($binding); }
		| binding_list[lst] ',' binding
			{ $$ = Bindings($lst, $binding); }
		;

value	: CONST
			{ $$ = Value($CONST); }
		| '(' value[left] ',' value[right] ')'
			{ $$ = Value::makePair($left, $right); }
		| '(' value ')'
			{ $$ = $2; }
		;

expr	: CONST
			{ $$ = Expression($CONST); }
		| VARIABLE
			{ $$ = Expression($VARIABLE); }
		| '(' expr[left] ',' expr[right] ')'
			{ $$ = Expression::makePair($left, $right); }
		| '(' expr ')'
			{ $$ = $2; }
		;

%%

void yy::parser::error(const std::string &msg) {
	std::cerr << msg << std::endl;
}
