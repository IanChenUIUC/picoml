#include "picoml.h"
#include "picoml.tab.h"

#include <iostream>

extern char *yytext;

using kind = yy::parser::symbol_kind;

std::string token_to_string(kind::symbol_kind_type token)
{
    switch (token)
    {
    case kind::S_EVAL:
        return "<eval>";
    case kind::S_VAL:
        return "<val>";
    case kind::S_VARIABLE:
        return "<variable>";
    case kind::S_IF:
        return "<if>";
    case kind::S_THEN:
        return "<then>";
    case kind::S_ELSE:
        return "<else>";
    case kind::S_TRUE:
        return "<true>";
    case kind::S_FALSE:
        return "<false>";
    case kind::S_LET:
        return "<let>";
    case kind::S_IN:
        return "<in>";
    case kind::S_EQUAL:
        return "<equal>";
    case kind::S_FUN:
        return "<fun>";
    case kind::S_MAPSTO:
        return "<mapsto>";
    case kind::S_OP:
        return "<op>";
    case kind::S_CONST:
        return "<const>";
    case kind::S_ERROR:
        return "<error>";
    default:
        return yytext;
    };
};

int main(void)
{
    while (true)
    {
        auto token = yylex();
        if (token.kind() == kind::S_YYEOF)
            break;
        std::cout << token_to_string(token.kind()) << "\n";
    };
};
