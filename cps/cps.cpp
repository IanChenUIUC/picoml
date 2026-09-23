#include "cps.h"
#include "picoml.h"
#include <stdexcept>
#include <type_traits>

Rewrite::Rewrite(Expression &&expr, Expression &&continuation)
    : expr(std::make_unique<Expression>(std::move(expr))),
      continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

Hole::IfR::IfR(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Variable &&binder)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse))), binder(std::move(binder))
{
}

Hole::If::If(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Variable &&binder)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse))), binder(std::move(binder))
{
}

Hole::App::App(Expression &&fun, Expression &&arg, Variable &&binder1, Variable &&binder2)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Expression>(std::move(arg))),
      binder1(std::move(binder1)), binder2(std::move(binder2))
{
}

Hole::BinOp::BinOp(Expression &&lhs, Expression &&rhs, Variable &&binder1, Variable &&binder2, ::BinOp binop)
    : lhs(std::make_unique<Expression>(std::move(lhs))), rhs(std::make_unique<Expression>(std::move(rhs))),
      binder1(std::move(binder1)), binder2(std::move(binder2)), binop(binop)
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
                return AppliedRule(Hole(
                    Hole::If(std::move(*arg.pred), std::move(expr), std::move(*arg.dofalse), std::move(arg.binder)),
                    clone(*continuation)));
            }
            else if constexpr (std::is_same_v<T, If>)
            {
                return AppliedRule(
                    Rewrite(std::move(*arg.pred),
                            Expression::makeFunction(Variable(arg.binder),
                                                     Expression::makeIf(Expression(Variable(arg.binder)),
                                                                        std::move(*arg.dotrue), std::move(expr)))));
            }
            else if constexpr (std::is_same_v<T, App>)
            {
                return AppliedRule(
                    Rewrite(std::move(*arg.arg), Expression::makeFunction(Variable(arg.binder1), std::move(expr))));
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                return AppliedRule(
                    Rewrite(std::move(*arg.rhs), Expression::makeFunction(Variable(arg.binder1), std::move(expr))));
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
                return to_string(*arg.fun);
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                return to_string(*arg.lhs);
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
                return std::make_unique<Expression>(Expression::makeFunction(
                    Variable(arg.binder2), Expression::makeApp(Expression::makeApp(Expression(Variable(arg.binder2)),
                                                                                   Expression(Variable(arg.binder1))),
                                                               clone(*continuation))));
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                return std::make_unique<Expression>(Expression::makeFunction(
                    Variable(arg.binder2),
                    Expression::makeApp(clone(*continuation),
                                        Expression::makeBinary(Expression(Variable(arg.binder2)),
                                                               Expression(Variable(arg.binder1)), arg.binop))));
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
                return "[[" + to_string(*arg.pred) + "]] (fun " + to_string(arg.binder) + " -> if " +
                       to_string(arg.binder) + " then " + inner + " else [[" + to_string(*arg.dofalse) + "]] (" +
                       to_string(*continuation) + "))";
            }
            else if constexpr (std::is_same_v<T, If>)
            {
                return "[[" + to_string(*arg.pred) + "]] (fun " + to_string(arg.binder) + " -> if " +
                       to_string(arg.binder) + " then " + to_string(*arg.dotrue) + " else " + inner + ")";
            }
            else if constexpr (std::is_same_v<T, App>)
            {
                return "[[" + to_string(*arg.arg) + "]] (fun " + to_string(arg.binder1) + " -> " + inner + ")";
            }
            else if constexpr (std::is_same_v<T, BinOp>)
            {
                return "[[" + to_string(*arg.rhs) + "]] (fun " + to_string(arg.binder1) + " -> " + inner + ")";
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
    return AppliedRule(Hole(Hole::IfR(std::move(*rule.pred), std::move(*rule.dotrue), std::move(*rule.dofalse),
                                      Variable("__a" + std::to_string(epoch++))),
                            clone(*continuation)));
}

AppliedRule Applier::operator()(Rule::TransApp &rule)
{
    int id1 = epoch++;
    int id2 = epoch++;
    return AppliedRule(Hole(Hole::App(std::move(*rule.fun), std::move(*rule.arg), Variable("__a" + std::to_string(id1)),
                                      Variable("__a" + std::to_string(id2))),
                            clone(*continuation)));
}

AppliedRule Applier::operator()(Rule::TransBinop &rule)
{
    int id1 = epoch++;
    int id2 = epoch++;
    return AppliedRule(
        Hole(Hole::BinOp(std::move(*rule.left), std::move(*rule.right), Variable("__a" + std::to_string(id1)),
                         Variable("__a" + std::to_string(id2)), rule.op),
             clone(*continuation)));
}

AppliedRule Applier::operator()(Rule::TransMonop &rule)
{
    int id = epoch++;
    Expression body = Expression::makeFunction(
        Variable("__a" + std::to_string(id)),
        Expression::makeApp(clone(*continuation),
                            Expression::makeUnary(Expression(Variable("__a" + std::to_string(id))), rule.op)));
    return AppliedRule(Rewrite(std::move(*rule.right), std::move(body)));
}

AppliedRule Applier::operator()(Rule::TransFun &rule)
{
    throw std::runtime_error("TransFun: not implemented");
}

AppliedRule Applier::operator()(Rule::TransLetIn &rule)
{
    throw std::runtime_error("TransLetIn: not implemented");
}
