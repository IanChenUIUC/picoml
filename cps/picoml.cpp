#include "picoml.h"

#include <memory>

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

Rule::TransVar::TransVar(Variable &&var, Expression &&continuation)
    : var(std::make_unique<Variable>(std::move(var))), continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

Rule::TransConst::TransConst(Value &&val, Expression &&continuation)
    : val(std::make_unique<Value>(std::move(val))), continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

Rule::TransIf::TransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Expression &&continuation)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse))),
      continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

Rule::TransApp::TransApp(Expression &&fun, Expression &&arg, Expression &&continuation)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Expression>(std::move(arg))),
      continuation(std::make_unique<Expression>(std::move(continuation)))
{
}

Rule::Rule(Alternative &&alternative) : rule(std::move(alternative))
{
}

Rule Rule::makeTransVar(Variable &&var, Expression &&continuation)
{
    return Rule(TransVar(std::move(var), std::move(continuation)));
}

Rule Rule::makeTransConst(Value &&val, Expression &&continuation)
{
    return Rule(TransConst(std::move(val), std::move(continuation)));
}

Rule Rule::makeTransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Expression &&continuation)
{
    return Rule(TransIf(std::move(pred), std::move(dotrue), std::move(dofalse), std::move(continuation)));
}

Rule Rule::makeTransApp(Expression &&fun, Expression &&arg, Expression &&continuation)
{
    return Rule(TransApp(std::move(fun), std::move(arg), std::move(continuation)));
}
