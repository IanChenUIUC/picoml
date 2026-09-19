#include "picoml.h"

#include <memory>
#include <type_traits>

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

template <class> inline constexpr bool always_false_v = false;

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
                os << "INTEGER(" << arg << ")";
            else if constexpr (std::is_same_v<T, bool>)
                os << "BOOLEAN(" << std::boolalpha << arg << ")";
            else if constexpr (std::is_same_v<T, Value::Function>)
                os << "FUNCTION(" << *arg.param << " -> " << *arg.body << ", " << *arg.env << ")";
            else if constexpr (std::is_same_v<T, Value::Pair>)
                os << "PAIR(" << *arg.left << ", " << *arg.right << ")";
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
            else if constexpr (std::is_same_v<T, Expression::IfExpr>)
                os << "if " << *arg.pred << " then " << *arg.dotrue << " else " << *arg.dofalse;
            else if constexpr (std::is_same_v<T, Expression::BinaryExpr>)
                os << "(" << *arg.left << " " << arg.op << " " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Expression::PairExpr>)
                os << "(" << *arg.left << ", " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Expression::FunExpr>)
                os << "fun " << *arg.param << " -> " << *arg.body;
            else if constexpr (std::is_same_v<T, Expression::AppExpr>)
                os << "(" << *arg.fun << " " << *arg.arg << ")";
            else if constexpr (std::is_same_v<T, Expression::LetExpr>)
                os << "let " << *arg.var << " = " << *arg.pre << " in " << *arg.body;
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

std::ostream &operator<<(std::ostream &os, const Evaluation &eval)
{
    const Environment &env = *eval.env;
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Rule::EvalConst>)
                os << "EvalConst(" << *arg.val << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalVar>)
                os << "EvalVar(" << *arg.var << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPair>)
                os << "EvalPair(Val " << *arg.left << ", Val " << *arg.right << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPairFst>)
                os << "EvalPairFst(" << *arg.left << ", Val " << *arg.right << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPairSnd>)
                os << "EvalPairSnd(" << *arg.left << ", " << *arg.right << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalIfTrue>)
                os << "EvalIfTrue(if Val true then " << *arg.dotrue << " else " << *arg.dofalse << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalIfFalse>)
                os << "EvalIfFalse(if Val false then " << *arg.dotrue << " else " << *arg.dofalse << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalIf>)
                os << "EvalIf(if " << *arg.pred << " then " << *arg.dotrue << " else " << *arg.dofalse << ", " << env
                   << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPrimOp>)
                os << "EvalPrimOp(Val " << *arg.left << " " << arg.op << " Val " << *arg.right << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPrimOpL>)
                os << "EvalPrimOpL(" << *arg.left << " " << arg.op << " Val " << *arg.right << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalPrimOpR>)
                os << "EvalPrimOpR(" << *arg.left << " " << arg.op << " " << *arg.right << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalApp>)
                os << "EvalApp(Val <" << *arg.param << " -> " << *arg.body << ", " << *arg.captured << "> Val "
                   << *arg.arg << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalAppFun>)
                os << "EvalAppFun(" << *arg.fun << " Val " << *arg.arg << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalAppArg>)
                os << "EvalAppArg(" << *arg.fun << " " << *arg.arg << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalFun>)
                os << "EvalFun(fun " << *arg.param << " -> " << *arg.body << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalLet>)
                os << "EvalLet(let " << *arg.var << " = Val " << *arg.val << " in " << *arg.body << ", " << env << ")";
            else if constexpr (std::is_same_v<T, Rule::EvalLetBinding>)
                os << "EvalLetBinding(let " << *arg.var << " = " << *arg.pre << " in " << *arg.body << ", " << env
                   << ")";
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        eval.rule.rule);
    return os;
}
