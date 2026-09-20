#include "interpreter.h"
#include <stdexcept>
#include <type_traits>

Atom::Atom(Value &&val) : val(std::make_unique<Value>(std::move(val)))
{
}

Rewrite::Rewrite(Expression &&expr) : expr(std::make_unique<Expression>(std::move(expr)))
{
}

Hole::PairLeft::PairLeft(Expression &&left, Value &&right)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Value>(std::move(right)))
{
}

Hole::PairRight::PairRight(Expression &&left, Expression &&right)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right)))
{
}

Hole::PrimOpL::PrimOpL(Expression &&left, Value &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Value>(std::move(right))), op(op)
{
}

Hole::PrimOpR::PrimOpR(Expression &&left, Expression &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right))), op(op)
{
}

Hole::If::If(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Hole::AppFun::AppFun(Expression &&fun, Value &&arg)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Value>(std::move(arg)))
{
}

Hole::AppArg::AppArg(Expression &&fun, Expression &&arg)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Expression>(std::move(arg)))
{
}

Hole::LetBindings::LetBindings(Variable &&var, Expression &&pre, Expression &&body)
    : var(std::make_unique<Variable>(std::move(var))), pre(std::make_unique<Expression>(std::move(pre))),
      body(std::make_unique<Expression>(std::move(body)))
{
}

Hole::Hole(Alternative &&alternative) : hole(std::move(alternative))
{
}

AppliedRule::AppliedRule(Alternative &&alternative) : result(std::move(alternative))
{
}

AppliedRule::AppliedRule(Alternative &&alternative, Environment &&env)
    : result(std::move(alternative)), env(std::make_unique<Environment>(std::move(env)))
{
}

std::string Hole::before() const
{
    return std::visit(
        [](const auto &h) -> std::string {
            using T = std::decay_t<decltype(h)>;
            if constexpr (std::is_same_v<T, PairLeft>)
                return "(";
            else if constexpr (std::is_same_v<T, PairRight>)
                return "(" + to_string(*h.left) + ", ";
            else if constexpr (std::is_same_v<T, PrimOpL>)
                return "";
            else if constexpr (std::is_same_v<T, PrimOpR>)
                return to_string(Paren{*h.left}) + " " + to_string(h.op) + " ";
            else if constexpr (std::is_same_v<T, If>)
                return "";
            else if constexpr (std::is_same_v<T, AppFun>)
                return "";
            else if constexpr (std::is_same_v<T, AppArg>)
                return "";
            else if constexpr (std::is_same_v<T, LetBindings>)
                return "";
            else
                static_assert(false, "non-exhaustive visitor!");
        },
        hole);
}

std::string Hole::after() const
{
    return std::visit(
        [](const auto &h) -> std::string {
            using T = std::decay_t<decltype(h)>;
            if constexpr (std::is_same_v<T, PairLeft>)
                return ", Val " + to_string(*h.right) + ")";
            else if constexpr (std::is_same_v<T, PairRight>)
                return ")";
            else if constexpr (std::is_same_v<T, PrimOpL>)
                return " " + to_string(h.op) + " Val " + to_string(*h.right);
            else if constexpr (std::is_same_v<T, PrimOpR>)
                return "";
            else if constexpr (std::is_same_v<T, If>)
                return "";
            else if constexpr (std::is_same_v<T, AppFun>)
                return "";
            else if constexpr (std::is_same_v<T, AppArg>)
                return "";
            else if constexpr (std::is_same_v<T, LetBindings>)
                return "";
            else
                static_assert(false, "non-exhaustive visitor!");
        },
        hole);
}

std::string Hole::cursor() const
{
    return std::visit(
        [](const auto &h) -> std::string {
            using T = std::decay_t<decltype(h)>;
            if constexpr (std::is_same_v<T, PairLeft>)
                return to_string(*h.left);
            else if constexpr (std::is_same_v<T, PairRight>)
                return to_string(*h.right);
            else if constexpr (std::is_same_v<T, PrimOpL>)
                return to_string(*h.left);
            else if constexpr (std::is_same_v<T, PrimOpR>)
                return to_string(*h.right);
            else if constexpr (std::is_same_v<T, If>)
                return "";
            else if constexpr (std::is_same_v<T, AppFun>)
                return "";
            else if constexpr (std::is_same_v<T, AppArg>)
                return "";
            else if constexpr (std::is_same_v<T, LetBindings>)
                return "";
            else
                static_assert(false, "non-exhaustive visitor!");
        },
        hole);
}

AppliedRule Applier::operator()(Rule::EvalConst &rule)
{
    return AppliedRule(Atom(std::move(*rule.val)));
};

AppliedRule Applier::operator()(Rule::EvalVar &rule)
{
    throw std::runtime_error("Not implemented");
}

AppliedRule Applier::operator()(Rule::EvalPair &rule)
{
    return AppliedRule(Atom(Value::makePair(std::move(*rule.left), std::move(*rule.right))));
}

AppliedRule Applier::operator()(Rule::EvalPairFst &rule)
{
    return AppliedRule(Hole(Hole::PairLeft(std::move(*rule.left), std::move(*rule.right))), std::move(env));
}

AppliedRule Applier::operator()(Rule::EvalPairSnd &rule)
{
    return AppliedRule(Hole(Hole::PairRight(std::move(*rule.left), std::move(*rule.right))), std::move(env));
}

AppliedRule Applier::operator()(Rule::EvalIfTrue &rule)
{
    throw std::runtime_error("Not implemented");
}
AppliedRule Applier::operator()(Rule::EvalIfFalse &rule)
{
    throw std::runtime_error("Not implemented");
}
AppliedRule Applier::operator()(Rule::EvalIf &rule)
{
    throw std::runtime_error("Not implemented");
}

AppliedRule Applier::operator()(Rule::EvalPrimOp &rule)
{
    const int *left = std::get_if<int>(&rule.left->val);
    const int *right = std::get_if<int>(&rule.right->val);
    if (!left || !right)
        throw std::runtime_error("expected integers, got " + to_string(*rule.left) + " and " + to_string(*rule.right));

    switch (rule.op.op)
    {
    case BinOp::ADD:
        return AppliedRule(Atom(Value(*left + *right)));
    case BinOp::SUB:
        return AppliedRule(Atom(Value(*left - *right)));
    case BinOp::MUL:
        return AppliedRule(Atom(Value(*left * *right)));
    case BinOp::DIV:
        if (*right == 0)
            throw std::runtime_error("division by zero: " + to_string(*rule.left) + " / " + to_string(*rule.right));
        return AppliedRule(Atom(Value(*left / *right)));
        break;
    }
}

AppliedRule Applier::operator()(Rule::EvalPrimOpL &rule)
{
    return AppliedRule(Hole(Hole::PrimOpL(std::move(*rule.left), std::move(*rule.right), rule.op)), std::move(env));
}

AppliedRule Applier::operator()(Rule::EvalPrimOpR &rule)
{
    return AppliedRule(Hole(Hole::PrimOpR(std::move(*rule.left), std::move(*rule.right), rule.op)), std::move(env));
}
AppliedRule Applier::operator()(Rule::EvalApp &rule)
{
    throw std::runtime_error("Not implemented");
}
AppliedRule Applier::operator()(Rule::EvalAppFun &rule)
{
    throw std::runtime_error("Not implemented");
}
AppliedRule Applier::operator()(Rule::EvalAppArg &rule)
{
    throw std::runtime_error("Not implemented");
}
AppliedRule Applier::operator()(Rule::EvalFun &rule)
{
    throw std::runtime_error("Not implemented");
}
AppliedRule Applier::operator()(Rule::EvalLet &rule)
{
    throw std::runtime_error("Not implemented");
}
AppliedRule Applier::operator()(Rule::EvalLetBinding &rule)
{
    throw std::runtime_error("Not implemented");
}
