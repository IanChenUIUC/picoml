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
    case kind::S_ADDOP:
    case kind::S_MULOP:
    case kind::S_RELOP:
        return "<op>";
    case kind::S_INTEGER:
    case kind::S_TRUE:
    case kind::S_FALSE:
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
        std::cout << token_to_string(token.kind());
        if (token.kind() == kind::S_INTEGER)
            std::cout << " " << token.value.as<int>();
        else if (token.kind() == kind::S_TRUE || token.kind() == kind::S_FALSE)
            std::cout << " " << std::boolalpha << token.value.as<bool>();
        else if (token.kind() == kind::S_VARIABLE)
            std::cout << " " << token.value.as<Variable>();
        else if (token.kind() == kind::S_ADDOP || token.kind() == kind::S_MULOP ||
                 token.kind() == kind::S_RELOP)
            std::cout << " " << token.value.as<BinOp>();
        std::cout << "\n";
    };
};
