#include "picoml.h"

#include <type_traits>

namespace
{

template <class> inline constexpr bool always_false_v = false;

bool isCompound(const Expression &expression)
{
    return std::visit(
        [](auto &&arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Value> || std::is_same_v<T, Variable> ||
                          std::is_same_v<T, Expression::PairExpr>)
                return false;
            else if constexpr (std::is_same_v<T, Expression::BinaryExpr> || std::is_same_v<T, Expression::AppExpr> ||
                               std::is_same_v<T, Expression::IfExpr> || std::is_same_v<T, Expression::FunExpr> ||
                               std::is_same_v<T, Expression::LetExpr>)
                return true;
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        expression.expr);
}

struct Paren
{
    const Expression &expression;
};

std::ostream &operator<<(std::ostream &os, Paren paren)
{
    if (!isCompound(paren.expression))
        return os << paren.expression;
    return os << "(" << paren.expression << ")";
}

} // namespace

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
            else if constexpr (std::is_same_v<T, Value::Function>)
                os << "<" << *arg.param << " -> " << *arg.body << ", " << *arg.env << ">";
            else if constexpr (std::is_same_v<T, Value::Pair>)
                os << "(" << *arg.left << ", " << *arg.right << ")";
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
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
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        expression.expr);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Binding &binding)
{
    return os << *binding.var << " -> " << *binding.val;
}

std::ostream &operator<<(std::ostream &os, const Bindings &bindings)
{
    os << "{";
    for (size_t i = 0; i < bindings.bindings.size(); i++)
    {
        if (i > 0)
            os << ", ";
        os << bindings.bindings[i];
    }
    return os << "}";
}

std::ostream &operator<<(std::ostream &os, const Environment &env)
{
    return os << env.bindings;
}

std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Rule::EvalConst>)
                os << *arg.val;
            else if constexpr (std::is_same_v<T, Rule::EvalVar>)
                os << *arg.var;
            else if constexpr (std::is_same_v<T, Rule::EvalPair>)
                os << "(Val " << *arg.left << ", Val " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPairFst>)
                os << "(" << *arg.left << ", Val " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPairSnd>)
                os << "(" << *arg.left << ", " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalIfTrue>)
                os << "if Val true then " << *arg.dotrue << " else " << *arg.dofalse;
            else if constexpr (std::is_same_v<T, Rule::EvalIfFalse>)
                os << "if Val false then " << *arg.dotrue << " else " << *arg.dofalse;
            else if constexpr (std::is_same_v<T, Rule::EvalIf>)
                os << "if " << *arg.pred << " then " << *arg.dotrue << " else " << *arg.dofalse;
            else if constexpr (std::is_same_v<T, Rule::EvalPrimOp>)
                os << "Val " << *arg.left << " " << arg.op << " Val " << *arg.right;
            else if constexpr (std::is_same_v<T, Rule::EvalPrimOpL>)
                os << Paren{*arg.left} << " " << arg.op << " Val " << *arg.right;
            else if constexpr (std::is_same_v<T, Rule::EvalPrimOpR>)
                os << Paren{*arg.left} << " " << arg.op << " " << Paren{*arg.right};
            else if constexpr (std::is_same_v<T, Rule::EvalApp>)
                os << "Val <" << *arg.param << " -> " << *arg.body << ", " << *arg.captured << "> Val " << *arg.arg;
            else if constexpr (std::is_same_v<T, Rule::EvalAppFun>)
                os << Paren{*arg.fun} << " Val " << *arg.arg;
            else if constexpr (std::is_same_v<T, Rule::EvalAppArg>)
                os << Paren{*arg.fun} << " " << Paren{*arg.arg};
            else if constexpr (std::is_same_v<T, Rule::EvalFun>)
                os << "fun " << *arg.param << " -> " << *arg.body;
            else if constexpr (std::is_same_v<T, Rule::EvalLet>)
                os << "let " << *arg.var << " = Val " << *arg.val << " in " << *arg.body;
            else if constexpr (std::is_same_v<T, Rule::EvalLetBinding>)
                os << "let " << *arg.var << " = " << Paren{*arg.pre} << " in " << *arg.body;
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        rule.rule);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Evaluation &eval)
{
    return os << "Eval(" << eval.rule << ", " << *eval.env << ")";
}
