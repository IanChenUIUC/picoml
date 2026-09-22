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
%type	<std::optional<Rule>>		CPS_Trans_App
%type	<std::optional<Rule>>		CPS_Trans_Binop
%type	<std::optional<Rule>>		CPS_Trans_Monop
%type	<std::optional<Rule>>		CPS_Trans_Fun
%type	<std::optional<Rule>>		CPS_Trans_LetIn

%type	<std::optional<Expression>>	expr
%type	<std::optional<Expression>>	keyword_expr
%type	<std::optional<Expression>>	unary_expr
%type	<std::optional<Expression>>	cmp_expr
%type	<std::optional<Expression>>	add_expr
%type	<std::optional<Expression>>	mul_expr
%type	<std::optional<Expression>>	atom
%type	<std::optional<Expression>>	app

%type	<BinOp>			addop
%type	<BinOp>			mulop
%type	<BinOp>			relop

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
		| CPS_Trans_App		{ result = $1; }
		| CPS_Trans_Binop	{ result = $1; }
		| CPS_Trans_Monop	{ result = $1; }
		| CPS_Trans_Fun		{ result = $1; }
		| CPS_Trans_LetIn	{ result = $1; }
		;

CPS_Trans_Var
		: LL VARIABLE RR atom
			{ $$ = Rule::makeTransVar($VARIABLE, $atom.value()); }
		;

CPS_Trans_Const
		: LL INTEGER RR atom
			{ $$ = Rule::makeTransConst(Value($INTEGER), $atom.value()); }
		| LL TRUE RR atom
			{ $$ = Rule::makeTransConst(Value($TRUE), $atom.value()); }
		| LL FALSE RR atom
			{ $$ = Rule::makeTransConst(Value($FALSE), $atom.value()); }
		;

CPS_Trans_If
		: LL IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse] RR atom
			{ $$ = Rule::makeTransIf($pred.value(), $dotrue.value(), $dofalse.value(), $atom.value()); }
		;

CPS_Trans_App
		: LL app atom[arg] RR atom[cont]
			{ $$ = Rule::makeTransApp($app.value(), $arg.value(), $cont.value()); }
		;

CPS_Trans_Binop
		: LL cmp_expr[left] relop[op] add_expr[right] RR atom[cont]
			{ $$ = Rule::makeTransBinop($left.value(), $right.value(), $op, $cont.value()); }
		| LL add_expr[left] addop[op] mul_expr[right] RR atom[cont]
			{ $$ = Rule::makeTransBinop($left.value(), $right.value(), $op, $cont.value()); }
		| LL mul_expr[left] mulop[op] unary_expr[right] RR atom[cont]
			{ $$ = Rule::makeTransBinop($left.value(), $right.value(), $op, $cont.value()); }
		;

CPS_Trans_Monop
		: LL MINUS[op] unary_expr[right] RR atom[cont]
			{ $$ = Rule::makeTransMonop($right.value(), $op, $cont.value()); }
		| LL MINUS[op] keyword_expr[right] RR atom[cont]
			{ $$ = Rule::makeTransMonop($right.value(), $op, $cont.value()); }
		;

CPS_Trans_Fun
		: LL FUN VARIABLE[param] MAPSTO expr[body] RR atom[cont]
			{ $$ = Rule::makeTransFun($param, $body.value(), $cont.value()); }
		;

CPS_Trans_LetIn
		: LL LET VARIABLE EQUAL expr[pre] IN expr[body] RR atom[cont]
			{ $$ = Rule::makeTransLetIn($VARIABLE, $pre.value(), $body.value(), $cont.value()); }
		;

addop	: ADDOP				{ $$ = $1; }
		| MINUS				{ $$ = $1; }
		;

mulop	: MULOP				{ $$ = $1; }
		;

relop	: RELOP				{ $$ = $1; }
		| EQUAL				{ $$ = BinOp(BinOp::EQ); }
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
		;

unary_expr
		: MINUS[op] unary_expr[right]
			{ $$ = Expression::makeUnary($right.value(), $op); }
		| app
			{ $$ = $1; }
		;

mul_expr
		: mul_expr[left] mulop[op] unary_expr[right]
			{ $$ = Expression::makeBinary($left.value(), $right.value(), $op); }
		| unary_expr
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

keyword_expr
		: FUN VARIABLE MAPSTO expr[body]
			{ $$ = Expression::makeFunction($VARIABLE, $body.value()); }
		| IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Expression::makeIf($pred.value(), $dotrue.value(), $dofalse.value()); }
		| LET VARIABLE EQUAL expr[pre] IN expr[body]
			{ $$ = Expression::makeLet($VARIABLE, $pre.value(), $body.value()); }
		;

expr	: cmp_expr
			{ $$ = $1; }
		| keyword_expr
			{ $$ = $1; }
		| MINUS[op] keyword_expr[right]
			{ $$ = Expression::makeUnary($right.value(), $op); }
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
