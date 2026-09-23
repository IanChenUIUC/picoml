#include "picoml.h"

#include <memory>
#include <type_traits>

BinOp::BinOp(BinOp::Op op) : op(op)
{
}

Value::Value(Alternative &&alternative) : val(std::move(alternative))
{
}

Variable::Variable(std::string &&identifier) : identifier(std::move(identifier))
{
}

bool operator<(const Variable &lhs, const Variable &rhs)
{
    return lhs.identifier < rhs.identifier;
}

Expression::IfExpr::IfExpr(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Expression::BinaryExpr::BinaryExpr(Expression &&left, Expression &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right))), op(op)
{
}

Expression::UnaryExpr::UnaryExpr(Expression &&right, BinOp op)
    : right(std::make_unique<Expression>(std::move(right))), op(op)
{
}

Expression::PairExpr::PairExpr(Expression &&left, Expression &&right)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right)))
{
}

Expression::FunExpr::FunExpr(Variable &&param, Expression &&body)
    : param(std::make_unique<Variable>(std::move(param))), body(std::make_unique<Expression>(std::move(body)))
{
}

Expression::AppExpr::AppExpr(Expression &&fun, Expression &&arg)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Expression>(std::move(arg)))
{
}

Expression::LetExpr::LetExpr(Variable &&var, Expression &&pre, Expression &&body)
    : var(std::make_unique<Variable>(std::move(var))), pre(std::make_unique<Expression>(std::move(pre))),
      body(std::make_unique<Expression>(std::move(body)))
{
}

Expression::Expression(Alternative &&alternative) : expr(std::move(alternative))
{
}

Expression Expression::makeIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
{
    return Expression(IfExpr(std::move(pred), std::move(dotrue), std::move(dofalse)));
}

Expression Expression::makeBinary(Expression &&left, Expression &&right, BinOp op)
{
    return Expression(BinaryExpr(std::move(left), std::move(right), op));
}

Expression Expression::makeUnary(Expression &&right, BinOp op)
{
    return Expression(UnaryExpr(std::move(right), op));
}

Expression Expression::makePair(Expression &&left, Expression &&right)
{
    return Expression(PairExpr(std::move(left), std::move(right)));
}

Expression Expression::makeFunction(Variable &&param, Expression &&body)
{
    return Expression(FunExpr(std::move(param), std::move(body)));
}

Expression Expression::makeApp(Expression &&fun, Expression &&arg)
{
    return Expression(AppExpr(std::move(fun), std::move(arg)));
}

Expression Expression::makeLet(Variable &&var, Expression &&pre, Expression &&body)
{
    return Expression(LetExpr(std::move(var), std::move(pre), std::move(body)));
}

Expression clone(const Expression &expression)
{
    return std::visit(
        [](auto &&arg) -> Expression {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Value> || std::is_same_v<T, Variable>)
                return Expression(T(arg));
            else if constexpr (std::is_same_v<T, Expression::PairExpr>)
                return Expression::makePair(clone(*arg.left), clone(*arg.right));
            else if constexpr (std::is_same_v<T, Expression::BinaryExpr>)
                return Expression::makeBinary(clone(*arg.left), clone(*arg.right), arg.op);
            else if constexpr (std::is_same_v<T, Expression::UnaryExpr>)
                return Expression::makeUnary(clone(*arg.right), arg.op);
            else if constexpr (std::is_same_v<T, Expression::AppExpr>)
                return Expression::makeApp(clone(*arg.fun), clone(*arg.arg));
            else if constexpr (std::is_same_v<T, Expression::FunExpr>)
                return Expression::makeFunction(Variable(*arg.param), clone(*arg.body));
            else if constexpr (std::is_same_v<T, Expression::LetExpr>)
                return Expression::makeLet(Variable(*arg.var), clone(*arg.pre), clone(*arg.body));
            else if constexpr (std::is_same_v<T, Expression::IfExpr>)
                return Expression::makeIf(clone(*arg.pred), clone(*arg.dotrue), clone(*arg.dofalse));
            else
                static_assert(false, "non-exhaustive visitor");
        },
        expression.expr);
}

Rule::TransVar::TransVar(Variable &&var) : var(std::make_unique<Variable>(std::move(var)))
{
}

Rule::TransConst::TransConst(Value &&val) : val(std::make_unique<Value>(std::move(val)))
{
}

Rule::TransIf::TransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Rule::TransApp::TransApp(Expression &&fun, Expression &&arg)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Expression>(std::move(arg)))
{
}

Rule::TransBinop::TransBinop(Expression &&left, Expression &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right))), op(op)
{
}

Rule::TransMonop::TransMonop(Expression &&right, BinOp op)
    : right(std::make_unique<Expression>(std::move(right))), op(op)
{
}

Rule::TransFun::TransFun(Variable &&param, Expression &&body)
    : param(std::make_unique<Variable>(std::move(param))), body(std::make_unique<Expression>(std::move(body)))
{
}

Rule::TransLetIn::TransLetIn(Variable &&var, Expression &&pre, Expression &&body)
    : var(std::make_unique<Variable>(std::move(var))), pre(std::make_unique<Expression>(std::move(pre))),
      body(std::make_unique<Expression>(std::move(body)))
{
}

Rule::Rule(Alternative &&alternative, Expression &&continuation)
    : rule(std::move(alternative)), continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

Rule Rule::makeTransVar(Variable &&var, Expression &&continuation)
{
    return Rule(TransVar(std::move(var)), std::move(continuation));
}

Rule Rule::makeTransConst(Value &&val, Expression &&continuation)
{
    return Rule(TransConst(std::move(val)), std::move(continuation));
}

Rule Rule::makeTransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Expression &&continuation)
{
    return Rule(TransIf(std::move(pred), std::move(dotrue), std::move(dofalse)), std::move(continuation));
}

Rule Rule::makeTransApp(Expression &&fun, Expression &&arg, Expression &&continuation)
{
    return Rule(TransApp(std::move(fun), std::move(arg)), std::move(continuation));
}

Rule Rule::makeTransBinop(Expression &&left, Expression &&right, BinOp op, Expression &&continuation)
{
    return Rule(TransBinop(std::move(left), std::move(right), op), std::move(continuation));
}

Rule Rule::makeTransMonop(Expression &&right, BinOp op, Expression &&continuation)
{
    return Rule(TransMonop(std::move(right), op), std::move(continuation));
}

Rule Rule::makeTransFun(Variable &&param, Expression &&body, Expression &&continuation)
{
    return Rule(TransFun(std::move(param), std::move(body)), std::move(continuation));
}

Rule Rule::makeTransLetIn(Variable &&var, Expression &&pre, Expression &&body, Expression &&continuation)
{
    return Rule(TransLetIn(std::move(var), std::move(pre), std::move(body)), std::move(continuation));
}
