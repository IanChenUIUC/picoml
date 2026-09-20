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
"else"			{	return yy::parser::token::ELSE;		}
"let"			{	return yy::parser::token::LET;		}
"in"			{	return yy::parser::token::IN;		}
"fun"			{	return yy::parser::token::FUN;		}
"->"			{	return yy::parser::token::MAPSTO;	}
"="				{	return yy::parser::token::EQUAL;	}

"+"				{	return yy::parser::make_ADDOP(BinOp::ADD);	}
"-"				{	return yy::parser::make_MINUS(BinOp::SUB);	}
"*"				{	return yy::parser::make_MULOP(BinOp::MUL);	}
"/"				{	return yy::parser::make_MULOP(BinOp::DIV);	}

"<>"			{	return yy::parser::make_RELOP(BinOp::NEQ);	}
"<="			{	return yy::parser::make_RELOP(BinOp::LEQ);	}
">="			{	return yy::parser::make_RELOP(BinOp::GEQ);	}

[0-9]+			{ 	return yy::parser::make_INTEGER(strtod(yytext, NULL)); }
"true"			{	return yy::parser::make_TRUE(true);		}
"false"			{	return yy::parser::make_FALSE(false);	}

[a-zA-Z_][a-zA-Z_0-9]* {
					return yy::parser::make_VARIABLE(std::string(yytext));
				}

[(){},\[\]<>]	{	return *yytext;	}
[ \t\n.]		{	}

.				{	return yy::parser::token::ERROR;	}
			

%%
