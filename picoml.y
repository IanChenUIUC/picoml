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

%parse-param { std::optional<Evaluation> &result }

%type	<std::optional<Rule>>	rule
%type 	<std::optional<Rule>>	EvalConst
%type 	<std::optional<Rule>>	EvalVar
%type 	<std::optional<Rule>>	EvalPair
%type 	<std::optional<Rule>>	EvalPairFst
%type 	<std::optional<Rule>>	EvalPairSnd
%type 	<std::optional<Rule>>	EvalIfTrue
%type 	<std::optional<Rule>>	EvalIfFalse
%type 	<std::optional<Rule>>	EvalIf
%type 	<std::optional<Rule>>	EvalPrimOp
%type 	<std::optional<Rule>>	EvalPrimOpL
%type 	<std::optional<Rule>>	EvalPrimOpR
%type	<std::optional<Rule>>	EvalApp
%type	<std::optional<Rule>>	EvalAppFun
%type	<std::optional<Rule>>	EvalAppArg
%type	<std::optional<Rule>>	EvalFun
%type	<std::optional<Rule>>	EvalLet
%type	<std::optional<Rule>>	EvalLetBinding

%type	<std::optional<Value>>		value
%type	<std::optional<Value>>		closure

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
%type	<Binding>		binding
%type	<Bindings>		binding_list
%type	<Environment>	env

%token	<Variable> 		VARIABLE
%token	<BinOp>			ADDOP MULOP MINUS RELOP

%token	<int> 			INTEGER
%token	<bool> 			TRUE
%token	<bool> 			FALSE

%token	EVAL VAL
%token 	IF THEN ELSE
%token	LET IN EQUAL
%token	FUN	MAPSTO
%token	ERROR

%%

input	: EVAL '(' rule ',' env ')'
			{ result = Evaluation($rule.value(), $env); }
		;

rule 	: EvalConst 		{ $$ = $1; }
		| EvalVar 			{ $$ = $1; }
		| EvalPair			{ $$ = $1; }
		| EvalPairFst 		{ $$ = $1; }
		| EvalPairSnd 		{ $$ = $1; }
		| EvalIfTrue 		{ $$ = $1; }
		| EvalIfFalse 		{ $$ = $1; }
		| EvalIf 			{ $$ = $1; }
		| EvalPrimOp 		{ $$ = $1; }
		| EvalPrimOpL 		{ $$ = $1; }
		| EvalPrimOpR 		{ $$ = $1; }
		| EvalApp 			{ $$ = $1; }
		| EvalAppFun 		{ $$ = $1; }
		| EvalAppArg 		{ $$ = $1; }
		| EvalFun 			{ $$ = $1; }
		| EvalLet 			{ $$ = $1; }
		| EvalLetBinding	{ $$ = $1; }
		;

EvalConst
		: INTEGER
			{ $$ = Rule::makeEvalConst(Value($INTEGER)); }
		| MINUS INTEGER
			{ $$ = Rule::makeEvalConst(Value(-$INTEGER)); }
		| TRUE
			{ $$ = Rule::makeEvalConst(Value($TRUE)); }
		| FALSE
			{ $$ = Rule::makeEvalConst(Value($FALSE)); }
		| closure
			{ $$ = Rule::makeEvalConst($closure.value()); }
		;

EvalVar	: VARIABLE
			{ $$ = Rule::makeEvalVar($VARIABLE); }
		;

EvalPair
		: '(' VAL value[left] ',' VAL value[right] ')'
			{ $$ = Rule::makeEvalPair($left.value(), $right.value()); }
		;

EvalPairFst
		: '(' expr[left] ',' VAL value[right] ')'
			{ $$ = Rule::makeEvalPairFst($left.value(), $right.value()); }
		;

EvalPairSnd
		: '(' expr[left] ',' expr[right] ')'
			{ $$ = Rule::makeEvalPairSnd($left.value(), $right.value()); }
		;

EvalIfTrue
		: IF VAL TRUE THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Rule::makeEvalIfTrue($dotrue.value(), $dofalse.value()); }
		;

EvalIfFalse
		: IF VAL FALSE THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Rule::makeEvalIfFalse($dotrue.value(), $dofalse.value()); }
		;

EvalIf
		: IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Rule::makeEvalIf($pred.value(), $dotrue.value(), $dofalse.value()); }
		;

EvalPrimOp
		: VAL value[left] binop[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOp($left.value(), $right.value(), $op); }
		;

EvalPrimOpL
		: cmp_expr[left] relop[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOpL($left.value(), $right.value(), $op); }
		| add_expr[left] addop[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOpL($left.value(), $right.value(), $op); }
		| mul_expr[left] mulop[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOpL($left.value(), $right.value(), $op); }
		;

EvalPrimOpR
		: cmp_expr[left] relop[op] add_expr[right]
			{ $$ = Rule::makeEvalPrimOpR($left.value(), $right.value(), $op); }
		| add_expr[left] addop[op] mul_expr[right]
			{ $$ = Rule::makeEvalPrimOpR($left.value(), $right.value(), $op); }
		| mul_expr[left] mulop[op] app[right]
			{ $$ = Rule::makeEvalPrimOpR($left.value(), $right.value(), $op); }
		;

EvalApp : VAL '<' VARIABLE MAPSTO expr[body] ',' env[captured] '>' VAL value
			{ $$ = Rule::makeEvalApp($VARIABLE, $body.value(), $captured, $value.value()); }
		;

EvalAppFun
		: app[fun] VAL value
			{ $$ = Rule::makeEvalAppFun($fun.value(), $value.value()); }
		;

EvalAppArg
		: app[fun] atom[arg]
			{ $$ = Rule::makeEvalAppArg($fun.value(), $arg.value()); }
		;

EvalFun	: FUN VARIABLE MAPSTO expr
			{ $$ = Rule::makeEvalFun($VARIABLE, $expr.value()); }
		;

EvalLet : LET VARIABLE EQUAL VAL value IN expr[body]
			{ $$ = Rule::makeEvalLet($VARIABLE, $value.value(), $body.value()); }
		;

EvalLetBinding
		 : LET VARIABLE EQUAL expr[pre] IN expr[body]
			{ $$ = Rule::makeEvalLetBinding($VARIABLE, $pre.value(), $body.value()); }
		;

env		: '{' binding_list[bindings] '}'
			{ $$ = Environment($bindings); }
		;

binding : VARIABLE MAPSTO value
			{ $$ = Binding($VARIABLE, $value.value()); }
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
		| MINUS INTEGER
			{ $$ = Value(-$INTEGER); }
		| TRUE
			{ $$ = Value($TRUE); }
		| FALSE
			{ $$ = Value($FALSE); }
		| '(' value[left] ',' value[right] ')'
			{ $$ = Value::makePair($left.value(), $right.value()); }
		| closure
			{ $$ = $closure; }
		| '(' value ')'
			{ $$ = $2; }
		;

closure	: '<' VARIABLE MAPSTO expr[body] ',' env '>'
			{ $$ = Value::makeFunction($VARIABLE, $body.value(), $env); }
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
		| closure
			{ $$ = Expression($closure.value()); }
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
