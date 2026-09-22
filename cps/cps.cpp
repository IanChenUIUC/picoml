#include "cps.h"
#include "picoml.h"
#include <stdexcept>
#include <type_traits>

Rewrite::Rewrite(Expression &&expr) : expr(std::make_unique<Expression>(std::move(expr)))
{
}

Hole::IfR::IfR(Expression &&pred, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Hole::If::If(Expression &&pred) : pred(std::make_unique<Expression>(std::move(pred)))
{
}

Hole::App::App(Expression &&arg) : arg(std::make_unique<Expression>(std::move(arg)))
{
}

Hole::BinOp::BinOp(Expression &&arg) : rhs(std::make_unique<Expression>(std::move(arg)))
{
}

Hole::LetIn::LetIn(Expression &&pre) : pre(std::make_unique<Expression>(std::move(pre)))
{
}

Hole::Hole(Alternative &&alternative) : hole(std::move(alternative))
{
}

AppliedRule::AppliedRule(Alternative &&alernative) : result(std::move(alernative))
{
}

AppliedRule::AppliedRule(Alternative &&alernative, Expression &&continuation)
    : result(std::move(alernative)), continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

AppliedRule Applier::operator()(Rule::TransVar &rule)
{
    return AppliedRule(std::move(*rule.var), std::move(*continuation));
}

AppliedRule Applier::operator()(Rule::TransConst &rule)
{
    return AppliedRule(std::move(*rule.val), std::move(*continuation));
}

AppliedRule Applier::operator()(Rule::TransIf &rule)
{
    throw std::runtime_error("TransIf: not implemented");
}

AppliedRule Applier::operator()(Rule::TransApp &rule)
{
    throw std::runtime_error("TransApp: not implemented");
}

AppliedRule Applier::operator()(Rule::TransBinop &rule)
{
    throw std::runtime_error("TransBinop: not implemented");
}

AppliedRule Applier::operator()(Rule::TransMonop &rule)
{
    throw std::runtime_error("TransMonop: not implemented");
}

AppliedRule Applier::operator()(Rule::TransFun &rule)
{
    throw std::runtime_error("TransFun: not implemented");
}

AppliedRule Applier::operator()(Rule::TransLetIn &rule)
{
    throw std::runtime_error("TransLetIn: not implemented");
}
