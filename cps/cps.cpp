#include "cps.h"
#include "picoml.h"
#include <stdexcept>
#include <type_traits>

Rewrite::Rewrite(Expression &&expr, Expression &&continuation)
    : expr(std::make_unique<Expression>(std::move(expr))),
      continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

Hole::IfR::IfR(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Hole::If::If(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
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

Hole::Hole(Alternative &&alternative, Expression &&continuation)
    : hole(std::move(alternative)), continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

AppliedRule Hole::plug(Expression &&expr)
{
    return std::visit(
        [&](auto &&arg) -> AppliedRule {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, IfR>)
            {
                return AppliedRule(Hole(Hole::If(std::move(*arg.pred), std::move(expr), std::move(*arg.dofalse)),
                                        clone(*continuation)));
            }
            else if constexpr (std::is_same_v<T, If>)
            {
                return AppliedRule(
                    Rewrite(std::move(*arg.pred),
                            Expression::makeFunction(Variable("b"),
                                                     Expression::makeIf(Expression(Variable("b")),
                                                                        std::move(*arg.dotrue), std::move(expr)))));
            }
            else if constexpr (std::is_same_v<T, App>)
            {
                throw std::runtime_error("Hole::App: not implemented");
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                throw std::runtime_error("Hole::BinOp: not implemented");
            }
            else if constexpr (std::is_same_v<T, Fun>)
            {
                throw std::runtime_error("Hole::Fun: not implemented");
            }
            else if constexpr (std::is_same_v<T, LetIn>)
            {
                throw std::runtime_error("Hole::LetIn: not implemented");
            }
            else
                static_assert(false, "non-exhaustive visitor");
        },
        hole);
}

std::string Hole::getNextCursor()
{
    return std::visit(
        [&](auto &&arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, IfR>)
            {
                return to_string(*arg.dotrue);
            }
            else if constexpr (std::is_same_v<T, If>)
            {
                return to_string(*arg.dofalse);
            }
            else if constexpr (std::is_same_v<T, App>)
            {
                throw std::runtime_error("Hole::App::getNextCursor: not implemented");
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                throw std::runtime_error("Hole::BinOp::getNextCursor: not implemented");
            }
            else if constexpr (std::is_same_v<T, Fun>)
            {
                throw std::runtime_error("Hole::Fun::getNextCursor: not implemented");
            }
            else if constexpr (std::is_same_v<T, LetIn>)
            {
                throw std::runtime_error("Hole::LetIn::getNextCursor: not implemented");
            }
            else
                static_assert(false, "non-exhaustive visitor");
        },
        hole);
}

std::unique_ptr<Expression> Hole::getNextContinuation()
{
    return std::visit(
        [&](auto &&arg) -> std::unique_ptr<Expression> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, IfR>)
            {
                return std::make_unique<Expression>(clone(*continuation));
            }
            else if constexpr (std::is_same_v<T, If>)
            {
                return std::make_unique<Expression>(clone(*continuation));
            }
            else if constexpr (std::is_same_v<T, App>)
            {
                throw std::runtime_error("Hole::App::getNextContinuation: not implemented");
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                throw std::runtime_error("Hole::BinOp::getNextContinuation: not implemented");
            }
            else if constexpr (std::is_same_v<T, Fun>)
            {
                throw std::runtime_error("Hole::Fun::getNextContinuation: not implemented");
            }
            else if constexpr (std::is_same_v<T, LetIn>)
            {
                throw std::runtime_error("Hole::LetIn::getNextContinuation: not implemented");
            }
            else
                static_assert(false, "non-exhaustive visitor");
        },
        hole);
}

std::string Hole::render(const std::string &inner) const
{
    return std::visit(
        [&](const auto &arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, IfR>)
            {
                return "[[" + to_string(*arg.pred) + "]] (fun b -> if b then " + inner + " else [[" +
                       to_string(*arg.dofalse) + "]] (" + to_string(*continuation) + "))";
            }
            else if constexpr (std::is_same_v<T, If>)
            {
                return "[[" + to_string(*arg.pred) + "]] (fun b -> if b then " + to_string(*arg.dotrue) + " else " +
                       inner + ")";
            }
            else if constexpr (std::is_same_v<T, App>)
            {
                throw std::runtime_error("Hole::App::render: not implemented");
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                throw std::runtime_error("Hole::BinOp::render: not implemented");
            }
            else if constexpr (std::is_same_v<T, Fun>)
            {
                throw std::runtime_error("Hole::Fun::render: not implemented");
            }
            else if constexpr (std::is_same_v<T, LetIn>)
            {
                throw std::runtime_error("Hole::LetIn::render: not implemented");
            }
            else
                static_assert(false, "non-exhaustive visitor");
        },
        hole);
}

AppliedRule Applier::operator()(Rule::TransVar &rule)
{
    return Expression::makeApp(clone(*continuation), Expression(std::move(*rule.var)));
}

AppliedRule Applier::operator()(Rule::TransConst &rule)
{
    return Expression::makeApp(clone(*continuation), Expression(std::move(*rule.val)));
}

AppliedRule Applier::operator()(Rule::TransIf &rule)
{
    return AppliedRule(Hole(Hole::IfR(std::move(*rule.pred), std::move(*rule.dotrue), std::move(*rule.dofalse)),
                            clone(*continuation)));
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
