%{

#include <iostream>

%}

%code requires {

#include <optional>

#include "picoml.h"

}

%code provides {

yy::parser::symbol_type yylex();
const std::string &last_parse_error();

}

%language "c++"

%define parse.assert
%define parse.error detailed
%define api.token.constructor
%define api.value.type variant
%define api.value.automove

%parse-param { std::optional<Rule> &result }

%type	<std::optional<Rule>>		CPS_Trans_Var
%type	<std::optional<Rule>>		CPS_Trans_Const
%type	<std::optional<Rule>>		CPS_Trans_If

%type	<std::optional<Expression>>	expr
%type	<std::optional<Expression>>	cmp_expr
%type	<std::optional<Expression>>	add_expr
%type	<std::optional<Expression>>	mul_expr
%type	<std::optional<Expression>>	atom
%type	<std::optional<Expression>>	app

%type	<BinOp>			addop
%type	<BinOp>			mulop
%type	<BinOp>			relop
%type	<BinOp>			binop

%token	<Variable> 		VARIABLE
%token	<BinOp>			ADDOP MULOP MINUS RELOP

%token	<int> 			INTEGER
%token	<bool> 			TRUE
%token	<bool> 			FALSE

%token	LL RR
%token 	IF THEN ELSE
%token	LET IN EQUAL
%token	FUN	MAPSTO
%token	ERROR

%%

input	: CPS_Trans_Var		{ result = $1; }
		| CPS_Trans_Const	{ result = $1; }
		| CPS_Trans_If		{ result = $1; }
		;

CPS_Trans_Var
		: LL VARIABLE RR atom
			{ $$ = Rule::makeTransVar($VARIABLE, $atom.value()); }
		;

CPS_Trans_Const
		: LL INTEGER RR atom
			{ $$ = Rule::makeTransConst(Value($INTEGER), $atom.value()); }
		| LL MINUS INTEGER RR atom
			{ $$ = Rule::makeTransConst(Value(-$INTEGER), $atom.value()); }
		| LL TRUE RR atom
			{ $$ = Rule::makeTransConst(Value($TRUE), $atom.value()); }
		| LL FALSE RR atom
			{ $$ = Rule::makeTransConst(Value($FALSE), $atom.value()); }
		;

CPS_Trans_If
		: LL IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse] RR atom
			{ $$ = Rule::makeTransIf($pred.value(), $dotrue.value(), $dofalse.value(), $atom.value()); }
		;

addop	: ADDOP				{ $$ = $1; }
		| MINUS				{ $$ = $1; }
		;

mulop	: MULOP				{ $$ = $1; }
		;

relop	: RELOP				{ $$ = $1; }
		| '>'				{ $$ = BinOp(BinOp::GT); }
		| EQUAL				{ $$ = BinOp(BinOp::EQ); }
		;

binop	: addop				{ $$ = $1; }
		| mulop				{ $$ = $1; }
		| relop				{ $$ = $1; }
		;

atom	: VARIABLE
			{ $$ = Expression($VARIABLE); }
		| INTEGER
			{ $$ = Expression(Value($INTEGER)); }
		| TRUE
			{ $$ = Expression(Value($TRUE)); }
		| FALSE
			{ $$ = Expression(Value($FALSE)); }
		| '(' expr[left] ',' expr[right] ')'
			{ $$ = Expression::makePair($left.value(), $right.value()); }
		| '(' expr ')'
			{ $$ = $2; }
		;

app		: app atom
			{ $$ = Expression::makeApp($1.value(), $2.value()); }
		| atom
			{ $$ = $1; }
		| MINUS INTEGER
			{ $$ = Expression(Value(-$INTEGER)); }
		;

mul_expr
		: mul_expr[left] mulop[op] app[right]
			{ $$ = Expression::makeBinary($left.value(), $right.value(), $op); }
		| app
			{ $$ = $1; }
		;

add_expr
		: add_expr[left] addop[op] mul_expr[right]
			{ $$ = Expression::makeBinary($left.value(), $right.value(), $op); }
		| mul_expr
			{ $$ = $1; }
		;

cmp_expr
		: cmp_expr[left] relop[op] add_expr[right]
			{ $$ = Expression::makeBinary($left.value(), $right.value(), $op); }
		| add_expr
			{ $$ = $1; }
		;

expr	: cmp_expr
			{ $$ = $1; }
		| FUN VARIABLE MAPSTO expr[body]
			{ $$ = Expression::makeFunction($VARIABLE, $body.value()); }
		| IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Expression::makeIf($pred.value(), $dotrue.value(), $dofalse.value()); }
		| LET VARIABLE EQUAL expr[pre] IN expr[body]
			{ $$ = Expression::makeLet($VARIABLE, $pre.value(), $body.value()); }
		;

%%

namespace
{

std::string parse_error;

}

const std::string &last_parse_error()
{
    return parse_error;
}

void yy::parser::error(const std::string &message)
{
    parse_error = message;
}
