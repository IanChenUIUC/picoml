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
%type	<Value>			value
%type	<Expression>	expr
%type	<Binding>		binding
%type	<Bindings>		binding_list
%type	<Environment>	env

%token	<Variable> 	VARIABLE
%token	<Value> 	CONST
%token	<BinOp>		OP

%right	OP

%token	EVAL VAL
%token 	IF THEN ELSE TRUE FALSE
%token	LET IN EQUAL
%token	FUN	MAPSTO
%token	ERROR

%%

input 	: EvalConst
			{ result = $EvalConst; }
		;

EvalConst
		: EVAL '(' CONST ',' env ')'
			{ $$ = Evaluation(Evaluation::EvalConst($CONST)); }
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
		| FUN VARIABLE MAPSTO expr ',' env
			{ $$ = Value::makeFunction($VARIABLE, $expr, $env); }
		| '(' value[left] ',' value[right] ')'
			{ $$ = Value::makePair($left, $right); }
		| '<' value '>'
			{ $$ = $2; }
		;

expr	: value
			{ $$ = Expression($value); }
		| VARIABLE
			{ $$ = Expression($VARIABLE); }
		| expr[left] OP expr[right]
			{ $$ = Expression::makeBinary($left, $right, $OP); }
		| '(' expr[left] ',' expr[right] ')'
			{ $$ = Expression::makePair($left, $right); }
		;

%%

void yy::parser::error(const std::string &msg) {
	std::cerr << msg << std::endl;
}
