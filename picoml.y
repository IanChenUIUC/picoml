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

%precedence	ELSE
%right		OP

%parse-param { Evaluation &result }

%type	<Evaluation>	input
%type 	<Evaluation>	EvalConst
%type 	<Evaluation>	EvalVar
%type 	<Evaluation>	EvalPair
%type 	<Evaluation>	EvalPairFst
%type 	<Evaluation>	EvalPairSnd
%type 	<Evaluation>	EvalIfTrue
%type 	<Evaluation>	EvalIfFalse
%type 	<Evaluation>	EvalIf
%type 	<Evaluation>	EvalPrimOp
%type 	<Evaluation>	EvalPrimOpL
%type 	<Evaluation>	EvalPrimOpR

%type	<Value>			value
%type	<Expression>	expr
%type	<Binding>		binding
%type	<Bindings>		binding_list
%type	<Environment>	env

%token	<Variable> 		VARIABLE
%token	<BinOp>			OP

%token	<int> 			INTEGER
%token	<bool> 			TRUE
%token	<bool> 			FALSE

%token	EVAL VAL
%token 	IF THEN ELSE
%token	LET IN EQUAL
%token	FUN	MAPSTO
%token	ERROR

%%

input 	: EvalConst 	{ result = $1; }
		| EvalVar 		{ result = $1; }
		| EvalPair		{ result = $1; }
		| EvalPairFst 	{ result = $1; }
		| EvalPairSnd 	{ result = $1; }
		| EvalIfTrue 	{ result = $1; }
		| EvalIfFalse 	{ result = $1; }
		| EvalIf 		{ result = $1; }
		| EvalPrimOp 	{ result = $1; }
		| EvalPrimOpL 	{ result = $1; }
		| EvalPrimOpR 	{ result = $1; }
		;

EvalConst
		: EVAL '(' INTEGER ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($INTEGER)); }
		| EVAL '(' TRUE ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($TRUE)); }
		| EVAL '(' FALSE ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($FALSE)); }
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

EvalIfTrue
		: EVAL '(' IF VAL TRUE THEN expr[dotrue] ELSE expr[dofalse] ',' env ')'
			{ $$ = Evaluation::makeEvalIfTrue($dotrue, $dofalse); }
		;

EvalIfFalse
		: EVAL '(' IF VAL FALSE THEN expr[dotrue] ELSE expr[dofalse] ',' env ')'
			{ $$ = Evaluation::makeEvalIfFalse($dotrue, $dofalse); }
		;

EvalIf
		: EVAL '(' IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse] ',' env ')'
			{ $$ = Evaluation::makeEvalIf($pred, $dotrue, $dofalse); }
		;

EvalPrimOp
		: EVAL '(' VAL value[left] OP VAL value[right] ',' env ')'
			{ $$ = Evaluation::makeEvalPrimOp($left, $right, $OP); }
		;

EvalPrimOpL
		: EVAL '(' expr[left] OP VAL value[right] ',' env ')'
			{ $$ = Evaluation::makeEvalPrimOpL($left, $right, $OP); }
		;

EvalPrimOpR
		: EVAL '(' expr[left] OP expr[right] ',' env ')'
			{ $$ = Evaluation::makeEvalPrimOpR($left, $right, $OP); }
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

value	: INTEGER
			{ $$ = Value($INTEGER); }
		| TRUE
			{ $$ = Value($TRUE); }
		| FALSE
			{ $$ = Value($FALSE); }
		| '(' value[left] ',' value[right] ')'
			{ $$ = Value::makePair($left, $right); }
		| '(' value ')'
			{ $$ = $2; }
		;

expr	: INTEGER
			{ $$ = Expression(Value($INTEGER)); }
		| TRUE
			{ $$ = Expression(Value($TRUE)); }
		| FALSE
			{ $$ = Expression(Value($FALSE)); }
		| VARIABLE
			{ $$ = Expression($VARIABLE); }
		| '(' expr[left] ',' expr[right] ')'
			{ $$ = Expression::makePair($left, $right); }
		| IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Expression::makeIf($pred, $dotrue, $dofalse); }
		| expr[left] OP expr[right]
			{ $$ = Expression::makeBinary($left, $right, $OP); }
		| '(' expr ')'
			{ $$ = $2; }
		;

%%

void yy::parser::error(const std::string &msg) {
	std::cerr << msg << std::endl;
}
