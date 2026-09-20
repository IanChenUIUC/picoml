%{

#include <iostream>

%}

%code requires {

#include <optional>

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

%precedence	IN
%precedence	ELSE MAPSTO
%left		ADDOP MINUS
%left		MULOP

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

%type	<std::optional<Value>>			value
%type	<std::optional<Expression>>	expr
%type	<std::optional<Expression>>	atom
%type	<std::optional<Expression>>	app
%type	<Binding>		binding
%type	<Bindings>		binding_list
%type	<Environment>	env

%token	<Variable> 		VARIABLE
%token	<BinOp>			ADDOP MULOP MINUS

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
		| '<' VARIABLE MAPSTO expr ',' env[captured] '>'
			{ $$ = Rule::makeEvalConst(Value::makeFunction($VARIABLE, $expr.value(), $captured)); }
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
		: VAL value[left] ADDOP[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOp($left.value(), $right.value(), $op); }
		| VAL value[left] MULOP[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOp($left.value(), $right.value(), $op); }
		| VAL value[left] MINUS[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOp($left.value(), $right.value(), $op); }
		;

EvalPrimOpL
		: expr[left] ADDOP[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOpL($left.value(), $right.value(), $op); }
		| expr[left] MULOP[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOpL($left.value(), $right.value(), $op); }
		| expr[left] MINUS[op] VAL value[right]
			{ $$ = Rule::makeEvalPrimOpL($left.value(), $right.value(), $op); }
		;

EvalPrimOpR
		: expr[left] ADDOP[op] expr[right]
			{ $$ = Rule::makeEvalPrimOpR($left.value(), $right.value(), $op); }
		| expr[left] MULOP[op] expr[right]
			{ $$ = Rule::makeEvalPrimOpR($left.value(), $right.value(), $op); }
		| expr[left] MINUS[op] expr[right]
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
		| '<' VARIABLE MAPSTO expr[body] ',' env '>'
			{ $$ = Value::makeFunction($VARIABLE, $body.value(), $env); }
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
			{ $$ = Expression(Value::makeFunction($VARIABLE, $body.value(), $env)); }
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

expr	: app
			{ $$ = $1; }
		| FUN VARIABLE MAPSTO expr[body]
			{ $$ = Expression::makeFunction($VARIABLE, $body.value()); }
		| IF expr[pred] THEN expr[dotrue] ELSE expr[dofalse]
			{ $$ = Expression::makeIf($pred.value(), $dotrue.value(), $dofalse.value()); }
		| expr[left] ADDOP[op] expr[right]
			{ $$ = Expression::makeBinary($left.value(), $right.value(), $op); }
		| expr[left] MULOP[op] expr[right]
			{ $$ = Expression::makeBinary($left.value(), $right.value(), $op); }
		| expr[left] MINUS[op] expr[right]
			{ $$ = Expression::makeBinary($left.value(), $right.value(), $op); }
		| LET VARIABLE EQUAL expr[pre] IN expr[body]
			{ $$ = Expression::makeLet($VARIABLE, $pre.value(), $body.value()); }
		;

%%

void yy::parser::error(const std::string &) {
}
