#include "picoml.h"

#include <type_traits>

namespace
{

bool isCompound(const Expression &expression)
{
    return std::visit(
        [](auto &&arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Value>)
                return std::holds_alternative<int>(arg.val) && std::get<int>(arg.val) < 0;
            else if constexpr (std::is_same_v<T, Variable> || std::is_same_v<T, Expression::PairExpr>)
                return false;
            else if constexpr (std::is_same_v<T, Expression::BinaryExpr> || std::is_same_v<T, Expression::AppExpr> ||
                               std::is_same_v<T, Expression::IfExpr> || std::is_same_v<T, Expression::FunExpr> ||
                               std::is_same_v<T, Expression::LetExpr>)
                return true;
            else
                static_assert(false, "non-exhaustive visitor");
        },
        expression.expr);
}

} // namespace

std::ostream &operator<<(std::ostream &os, Paren paren)
{
    if (!isCompound(paren.expression))
        return os << paren.expression;
    return os << "(" << paren.expression << ")";
}

std::ostream &operator<<(std::ostream &os, const BinOp &binop)
{
    switch (binop.op)
    {
    case BinOp::ADD:
        return os << "+";
    case BinOp::SUB:
        return os << "-";
    case BinOp::MUL:
        return os << "*";
    case BinOp::DIV:
        return os << "/";
    case BinOp::LT:
        return os << "<";
    case BinOp::GT:
        return os << ">";
    case BinOp::LEQ:
        return os << "<=";
    case BinOp::GEQ:
        return os << ">=";
    case BinOp::EQ:
        return os << "=";
    case BinOp::NEQ:
        return os << "<>";
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Value &value)
{
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>)
                os << arg;
            else if constexpr (std::is_same_v<T, bool>)
                os << std::boolalpha << arg;
            else
                static_assert(false, "non-exhaustive visitor");
        },
        value.val);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Variable &variable)
{
    return os << variable.identifier;
}

std::ostream &operator<<(std::ostream &os, const Expression &expression)
{
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Value> || std::is_same_v<T, Variable>)
                os << arg;
            else if constexpr (std::is_same_v<T, Expression::PairExpr>)
                os << "(" << *arg.left << ", " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Expression::BinaryExpr>)
                os << Paren{*arg.left} << " " << arg.op << " " << Paren{*arg.right};
            else if constexpr (std::is_same_v<T, Expression::AppExpr>)
                os << Paren{*arg.fun} << " " << Paren{*arg.arg};
            else if constexpr (std::is_same_v<T, Expression::FunExpr>)
                os << "fun " << *arg.param << " -> " << *arg.body;
            else if constexpr (std::is_same_v<T, Expression::LetExpr>)
                os << "let " << *arg.var << " = " << Paren{*arg.pre} << " in " << *arg.body;
            else if constexpr (std::is_same_v<T, Expression::IfExpr>)
                os << "if " << *arg.pred << " then " << *arg.dotrue << " else " << *arg.dofalse;
            else
                static_assert(false, "non-exhaustive visitor");
        },
        expression.expr);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Rule::TransVar>)
                os << "[[" << *arg.var << "]] " << Paren{*arg.continuation};
            else if constexpr (std::is_same_v<T, Rule::TransConst>)
                os << "[[" << *arg.val << "]] " << Paren{*arg.continuation};
            else if constexpr (std::is_same_v<T, Rule::TransIf>)
                os << "[[if " << *arg.pred << " then " << *arg.dotrue << " else " << *arg.dofalse << "]] "
                   << Paren{*arg.continuation};
            else
                static_assert(false, "non-exhaustive visitor");
        },
        rule.rule);
    return os;
}
