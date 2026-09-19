%option noyywrap
%option	nounput

%{

#include <stdlib.h>
#include "picoml.tab.h"

#define YY_DECL yy::parser::symbol_type yylex()

%}

%%

"Eval"			{	return yy::parser::token::EVAL; 	}
"Val"			{	return yy::parser::token::VAL;		}
"if"			{	return yy::parser::token::IF;		}
"then"			{	return yy::parser::token::THEN;		}
"true"			{	return yy::parser::token::TRUE;		}
"false"			{	return yy::parser::token::FALSE;	}
"let"			{	return yy::parser::token::LET;		}
"in"			{	return yy::parser::token::IN;		}
"fun"			{	return yy::parser::token::FUN;		}
"->"			{	return yy::parser::token::MAPSTO;	}
"="				{	return yy::parser::token::EQUAL;	}

"+"				{	return yy::parser::make_OP(BinOp::ADD);	}
"-"				{	return yy::parser::make_OP(BinOp::SUB);	}
"*"				{	return yy::parser::make_OP(BinOp::MUL);	}
"/"				{	return yy::parser::make_OP(BinOp::DIV);	}

[0-9]+			{
					return yy::parser::make_CONST(strtod(yytext, NULL));
				}

[a-zA-Z_][a-zA-Z_0-9]* {
					return yy::parser::make_VARIABLE(std::string(yytext));
				}

[(){},\[\]<>]	{	return *yytext;	}
[ \t\n.]		{	}

.				{	return yy::parser::token::ERROR;	}
			

%%
