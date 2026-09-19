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

%precedence	ELSE MAPSTO
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
%type	<Evaluation>	EvalApp
%type	<Evaluation>	EvalAppFun
%type	<Evaluation>	EvalAppArg
%type	<Evaluation>	EvalFun

%type	<Value>			value
%type	<Expression>	expr
%type	<Expression>	atom
%type	<Expression>	app
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
		| EvalApp 		{ result = $1; }
		| EvalAppFun 	{ result = $1; }
		| EvalAppArg 	{ result = $1; }
		| EvalFun 		{ result = $1; }
		;

EvalConst
		: EVAL '(' INTEGER ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($INTEGER)); }
		| EVAL '(' TRUE ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($TRUE)); }
		| EVAL '(' FALSE ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($FALSE)); }
		| EVAL '(' '<' VARIABLE MAPSTO expr ',' env[captured] '>' ',' env[outside] ')'
			{ $$ = Evaluation(Evaluation::EvalConst(Value::makeFunction($VARIABLE, $expr, $captured))); }
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

EvalApp : EVAL '(' VAL '<' VARIABLE MAPSTO expr[body] ',' env[captured] '>' VAL value ',' env[outside] ')'
			{ $$ = Evaluation::makeEvalApp($VARIABLE, $body, $captured, $value); }
		;

EvalAppFun
		: EVAL '(' app[fun] VAL value ',' env[outside] ')'
			{ $$ = Evaluation::makeEvalAppFun($fun, $outside, $value); }
		;

EvalAppArg
		: EVAL '(' app[fun] atom[arg] ',' env[outside] ')'
			{ $$ = Evaluation::makeEvalAppArg($fun, $arg, $outside); }
		;

EvalFun	: EVAL '(' FUN VARIABLE MAPSTO expr ',' env ')'
			{ $$ = Evaluation::makeEvalFun($VARIABLE, $expr, $env); }
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
		| '<' VARIABLE MAPSTO expr[body] ',' env '>'
			{ $$ = Value::makeFunction($VARIABLE, $body, $env); }
		| '(' value ')'
			{ $$ = $2; }
		;

atom	: VARIABLE
			{ $$ = Expression($VARIABLE); }
		| INTEGER
			{ $$ = Expression(Value($INTEGER)); }
		| TRUE
			{ $$ = Expression(Value($TRUE)); }
		| FALSE
			{ $$ = Expression(Value($FALSE)); }
		| '<' VARIABLE MAPSTO expr[body] ',' env '>'
			{ $$ = Expression(Value::makeFunction($VARIABLE, $body, $env)); }
		| '(' expr[left] ',' expr[right] ')'
			{ $$ = Expression::makePair($left, $right); }
		| '(' expr ')'
			{ $$ = $2; }
		;

app		: app atom
			{ $$ = Expression::makeApp($1, $2); }
		| atom
			{ $$ = $1; }
		;

expr	: app
			{ $$ = $1; }
		| FUN VARIABLE MAPSTO expr[body]
			{ $$ = Expression::makeFunction($VARIABLE, $body); }
		| IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Expression::makeIf($pred, $dotrue, $dofalse); }
		| expr[left] OP expr[right]
			{ $$ = Expression::makeBinary($left, $right, $OP); }
		;

%%

void yy::parser::error(const std::string &msg) {
	std::cerr << msg << std::endl;
}
