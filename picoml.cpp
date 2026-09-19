#include "picoml.h"

#include <memory>

BinOp::BinOp(BinOp::Op op) : op(op)
{
}

Value::Function::Function(Variable &&param, Expression &&body, Environment &&env)
    : param(std::make_unique<Variable>(std::move(param))), body(std::make_unique<Expression>(std::move(body))),
      env(std::make_unique<Environment>(std::move(env)))
{
}

Value::Pair::Pair(Value &&left, Value &&right)
    : left(std::make_unique<Value>(std::move(left))), right(std::make_unique<Value>(std::move(right)))
{
}

Value::Value(Alternative &&alternative) : val(std::move(alternative))
{
}

Value Value::makeFunction(Variable &&param, Expression &&body, Environment &&env)
{
    return Value(Function(std::move(param), std::move(body), std::move(env)));
}

Value Value::makePair(Value &&left, Value &&right)
{
    return Value(Pair(std::move(left), std::move(right)));
}

Variable::Variable(std::string &&identifier) : identifier(std::move(identifier))
{
}

Expression::IfExpr::IfExpr(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Expression::BinaryExpr::BinaryExpr(Expression &&left, Expression &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right))),
      op(op)
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

Binding::Binding(Variable &&var, Value &&val)
    : var(std::make_unique<Variable>(std::move(var))), val(std::make_unique<Value>(std::move(val)))
{
}

Bindings::Bindings(Binding &&binding)
{
    this->bindings.emplace_back(std::move(binding));
}

Bindings::Bindings(Bindings &&bindings, Binding &&binding) : bindings(std::move(bindings.bindings))
{
    this->bindings.emplace_back(std::move(binding));
}

Environment::Environment(Bindings &&bindings) : bindings(std::move(bindings))
{
}

Rule::EvalConst::EvalConst(Value &&val) : val(std::make_unique<Value>(std::move(val)))
{
}

Rule::EvalVar::EvalVar(Variable &&var) : var(std::make_unique<Variable>(std::move(var)))
{
}

Rule::EvalPair::EvalPair(Value &&left, Value &&right)
    : left(std::make_unique<Value>(std::move(left))), right(std::make_unique<Value>(std::move(right)))
{
}

Rule::EvalPairFst::EvalPairFst(Expression &&left, Value &&right)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Value>(std::move(right)))
{
}

Rule::EvalPairSnd::EvalPairSnd(Expression &&left, Expression &&right)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right)))
{
}

Rule::EvalIfTrue::EvalIfTrue(Expression &&dotrue, Expression &&dofalse)
    : dotrue(std::make_unique<Expression>(std::move(dotrue))), dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Rule::EvalIfFalse::EvalIfFalse(Expression &&dotrue, Expression &&dofalse)
    : dotrue(std::make_unique<Expression>(std::move(dotrue))), dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Rule::EvalIf::EvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Rule::EvalPrimOp::EvalPrimOp(Value &&left, Value &&right, BinOp op)
    : left(std::make_unique<Value>(std::move(left))), right(std::make_unique<Value>(std::move(right))), op(op)
{
}

Rule::EvalPrimOpL::EvalPrimOpL(Expression &&left, Value &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Value>(std::move(right))), op(op)
{
}

Rule::EvalPrimOpR::EvalPrimOpR(Expression &&left, Expression &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right))),
      op(op)
{
}

Rule::EvalApp::EvalApp(Variable &&param, Expression &&body, Environment &&captured, Value &&arg)
    : param(std::make_unique<Variable>(std::move(param))), body(std::make_unique<Expression>(std::move(body))),
      captured(std::make_unique<Environment>(std::move(captured))), arg(std::make_unique<Value>(std::move(arg)))
{
}

Rule::EvalAppFun::EvalAppFun(Expression &&fun, Value &&arg)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Value>(std::move(arg)))
{
}

Rule::EvalAppArg::EvalAppArg(Expression &&fun, Expression &&arg)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Expression>(std::move(arg)))
{
}

Rule::EvalFun::EvalFun(Variable &&param, Expression &&body)
    : param(std::make_unique<Variable>(std::move(param))), body(std::make_unique<Expression>(std::move(body)))
{
}

Rule::EvalLet::EvalLet(Variable &&var, Value &&val, Expression &&body)
    : var(std::make_unique<Variable>(std::move(var))), val(std::make_unique<Value>(std::move(val))),
      body(std::make_unique<Expression>(std::move(body)))
{
}

Rule::EvalLetBinding::EvalLetBinding(Variable &&var, Expression &&pre, Expression &&body)
    : var(std::make_unique<Variable>(std::move(var))), pre(std::make_unique<Expression>(std::move(pre))),
      body(std::make_unique<Expression>(std::move(body)))
{
}

Rule Rule::makeEvalConst(Value &&val)
{
    return Rule(EvalConst(std::move(val)));
}

Rule Rule::makeEvalVar(Variable &&var)
{
    return Rule(EvalVar(std::move(var)));
}

Rule Rule::makeEvalPair(Value &&left, Value &&right)
{
    return Rule(EvalPair(std::move(left), std::move(right)));
}

Rule Rule::makeEvalPairFst(Expression &&left, Value &&right)
{
    return Rule(EvalPairFst(std::move(left), std::move(right)));
}

Rule Rule::makeEvalPairSnd(Expression &&left, Expression &&right)
{
    return Rule(EvalPairSnd(std::move(left), std::move(right)));
}

Rule Rule::makeEvalIfTrue(Expression &&dotrue, Expression &&dofalse)
{
    return Rule(EvalIfTrue(std::move(dotrue), std::move(dofalse)));
}

Rule Rule::makeEvalIfFalse(Expression &&dotrue, Expression &&dofalse)
{
    return Rule(EvalIfFalse(std::move(dotrue), std::move(dofalse)));
}

Rule Rule::makeEvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
{
    return Rule(EvalIf(std::move(pred), std::move(dotrue), std::move(dofalse)));
}

Rule Rule::makeEvalPrimOp(Value &&left, Value &&right, BinOp op)
{
    return Rule(EvalPrimOp(std::move(left), std::move(right), op));
}

Rule Rule::makeEvalPrimOpL(Expression &&left, Value &&right, BinOp op)
{
    return Rule(EvalPrimOpL(std::move(left), std::move(right), op));
}

Rule Rule::makeEvalPrimOpR(Expression &&left, Expression &&right, BinOp op)
{
    return Rule(EvalPrimOpR(std::move(left), std::move(right), op));
}

Rule Rule::makeEvalApp(Variable &&param, Expression &&body, Environment &&captured, Value &&arg)
{
    return Rule(EvalApp(std::move(param), std::move(body), std::move(captured), std::move(arg)));
}

Rule Rule::makeEvalAppFun(Expression &&fun, Value &&arg)
{
    return Rule(EvalAppFun(std::move(fun), std::move(arg)));
}

Rule Rule::makeEvalAppArg(Expression &&fun, Expression &&arg)
{
    return Rule(EvalAppArg(std::move(fun), std::move(arg)));
}

Rule Rule::makeEvalFun(Variable &&param, Expression &&body)
{
    return Rule(EvalFun(std::move(param), std::move(body)));
}

Rule Rule::makeEvalLet(Variable &&var, Value &&val, Expression &&body)
{
    return Rule(EvalLet(std::move(var), std::move(val), std::move(body)));
}

Rule Rule::makeEvalLetBinding(Variable &&var, Expression &&pre, Expression &&body)
{
    return Rule(EvalLetBinding(std::move(var), std::move(pre), std::move(body)));
}

Rule::Rule(Alternative &&alternative) : rule(std::move(alternative))
{
}

Evaluation::Evaluation(Rule &&rule, Environment &&env)
    : rule(std::move(rule)), env(std::make_unique<Environment>(std::move(env)))
{
}
