#include "picoml.h"

#include <memory>
#include <type_traits>

BinOp::BinOp(BinOp::Op op) : op(op)
{
}

Value::Function::Function(std::unique_ptr<Variable> param, std::unique_ptr<Expression> body,
                          std::unique_ptr<Environment> env)
    : param(std::move(param)), body(std::move(body)), env(std::move(env))
{
}

Value::Pair::Pair(std::unique_ptr<Value> left, std::unique_ptr<Value> right)
    : left(std::move(left)), right(std::move(right))
{
}

Expression::IfExpr::IfExpr(std::unique_ptr<Expression> pred, std::unique_ptr<Expression> dotrue,
                           std::unique_ptr<Expression> dofalse)
    : pred(std::move(pred)), dotrue(std::move(dotrue)), dofalse(std::move(dofalse))
{
}

Expression::BinaryExpr::BinaryExpr(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, BinOp op)
    : left(std::move(left)), right(std::move(right)), op(std::move(op))
{
}

Expression::FunExpr::FunExpr(std::unique_ptr<Variable> param, std::unique_ptr<Expression> body)
    : param(std::move(param)), body(std::move(body))
{
}

Value::Value(int integer) : val(integer) {};

Value::Value(bool boolean) : val(boolean) {};

Value Value::makeFunction(Variable &&param, Expression &&body, Environment &&env)
{
    Value value;
    value.val = Function(std::make_unique<Variable>(param), std::make_unique<Expression>(std::move(body)),
                         std::make_unique<Environment>(std::move(env)));
    return value;
}

Value Value::makePair(Value &&left, Value &&right)
{
    Value value;
    value.val = Pair(std::make_unique<Value>(std::move(left)), std::make_unique<Value>(std::move(right)));
    return value;
}

Variable::Variable(std::string &&identifier) : identifier(std::move(identifier)) {};

Expression::Expression(Value &&val) : expr(std::move(val)) {};
Expression::Expression(Variable &&var) : expr(std::move(var)) {};

Expression Expression::makeIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
{
    Expression expression;
    expression.expr =
        IfExpr(std::make_unique<Expression>(std::move(pred)), std::make_unique<Expression>(std::move(dotrue)),
               std::make_unique<Expression>(std::move(dofalse)));
    return expression;
}

Expression Expression::makeBinary(Expression &&left, Expression &&right, BinOp &&op)
{
    Expression expression;
    expression.expr =
        BinaryExpr(std::make_unique<Expression>(std::move(left)), std::make_unique<Expression>(std::move(right)), op);
    return expression;
}

Expression Expression::makePair(Expression &&left, Expression &&right)
{
    Expression expression;
    expression.expr =
        PairExpr{std::make_unique<Expression>(std::move(left)), std::make_unique<Expression>(std::move(right))};
    return expression;
}

Expression Expression::makeFunction(Variable &&param, Expression &&body)
{
    Expression expression;
    expression.expr =
        FunExpr(std::make_unique<Variable>(std::move(param)), std::make_unique<Expression>(std::move(body)));
    return expression;
}

Expression Expression::makeApp(Expression &&fun, Expression &&arg)
{
    Expression expression;
    expression.expr =
        AppExpr{std::make_unique<Expression>(std::move(fun)), std::make_unique<Expression>(std::move(arg))};
    return expression;
}

Binding::Binding(Variable &&var, Value &&val) : var(std::move(var)), val(std::make_unique<Value>(std::move(val)))
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

Evaluation::EvalConst::EvalConst(Value &&val) : val(std::make_unique<Value>(std::move(val)))
{
}

Evaluation::EvalVar::EvalVar(Variable &&var, Environment &&env)
    : var(std::make_unique<Variable>(std::move(var))), env(std::make_unique<Environment>(std::move(env)))
{
}

Evaluation::EvalPair::EvalPair(Value &&left, Value &&right)
    : left(std::make_unique<Value>(std::move(left))), right(std::make_unique<Value>(std::move(right)))
{
}

Evaluation::EvalPairFst::EvalPairFst(Expression &&left, Value &&right)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Value>(std::move(right)))
{
}

Evaluation::EvalPairSnd::EvalPairSnd(Expression &&left, Expression &&right)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right)))
{
}

Evaluation::EvalIfTrue::EvalIfTrue(Expression &&dotrue, Expression &&dofalse)
    : dotrue(std::make_unique<Expression>(std::move(dotrue))), dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Evaluation::EvalIfFalse::EvalIfFalse(Expression &&dotrue, Expression &&dofalse)
    : dotrue(std::make_unique<Expression>(std::move(dotrue))), dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Evaluation::EvalIf::EvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
    : pred(std::make_unique<Expression>(std::move(pred))), dotrue(std::make_unique<Expression>(std::move(dotrue))),
      dofalse(std::make_unique<Expression>(std::move(dofalse)))
{
}

Evaluation::EvalPrimOp::EvalPrimOp(Value &&left, Value &&right, BinOp op)
    : left(std::make_unique<Value>(std::move(left))), right(std::make_unique<Value>(std::move(right))),
      op(std::move(op))
{
}

Evaluation::EvalPrimOpL::EvalPrimOpL(Expression &&left, Value &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Value>(std::move(right))),
      op(std::move(op))
{
}

Evaluation::EvalPrimOpR::EvalPrimOpR(Expression &&left, Expression &&right, BinOp op)
    : left(std::make_unique<Expression>(std::move(left))), right(std::make_unique<Expression>(std::move(right))),
      op(std::move(op))
{
}

Evaluation::EvalApp::EvalApp(Variable &&param, Expression &&body, Environment &&captured, Value &&arg)
    : param(std::make_unique<Variable>(std::move(param))), body(std::make_unique<Expression>(std::move(body))),
      captured(std::make_unique<Environment>(std::move(captured))), arg(std::make_unique<Value>(std::move(arg)))
{
}

Evaluation::EvalAppFun::EvalAppFun(Expression &&fun, Environment &&outside, Value &&arg)
    : fun(std::make_unique<Expression>(std::move(fun))), outside(std::make_unique<Environment>(std::move(outside))),
      arg(std::make_unique<Value>(std::move(arg)))
{
}

Evaluation::EvalAppArg::EvalAppArg(Expression &&fun, Expression &&arg, Environment &&outside)
    : fun(std::make_unique<Expression>(std::move(fun))), arg(std::make_unique<Expression>(std::move(arg))),
      outside(std::make_unique<Environment>(std::move(outside)))
{
}

Evaluation::EvalFun::EvalFun(Variable &&param, Expression &&body, Environment &&env)
    : param(std::make_unique<Variable>(std::move(param))), body(std::make_unique<Expression>(std::move(body))),
      env(std::make_unique<Environment>(std::move(env)))
{
}

Evaluation::Evaluation(EvalConst &&eval) : eval(std::move(eval))
{
}

Evaluation::Evaluation(EvalVar &&eval) : eval(std::move(eval))
{
}

Evaluation Evaluation::makeEvalPair(Value &&left, Value &&right)
{
    Evaluation evaluation;
    evaluation.eval = EvalPair(std::move(left), std::move(right));
    return evaluation;
}

Evaluation Evaluation::makeEvalPairFst(Expression &&left, Value &&right)
{
    Evaluation evaluation;
    evaluation.eval = EvalPairFst(std::move(left), std::move(right));
    return evaluation;
}

Evaluation Evaluation::makeEvalPairSnd(Expression &&left, Expression &&right)
{
    Evaluation evaluation;
    evaluation.eval = EvalPairSnd(std::move(left), std::move(right));
    return evaluation;
}

Evaluation Evaluation::makeEvalIfTrue(Expression &&dotrue, Expression &&dofalse)
{
    Evaluation evaluation;
    evaluation.eval = EvalIfTrue(std::move(dotrue), std::move(dofalse));
    return evaluation;
}

Evaluation Evaluation::makeEvalIfFalse(Expression &&dotrue, Expression &&dofalse)
{
    Evaluation evaluation;
    evaluation.eval = EvalIfFalse(std::move(dotrue), std::move(dofalse));
    return evaluation;
}

Evaluation Evaluation::makeEvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse)
{
    Evaluation evaluation;
    evaluation.eval = EvalIf(std::move(pred), std::move(dotrue), std::move(dofalse));
    return evaluation;
}

Evaluation Evaluation::makeEvalPrimOp(Value &&left, Value &&right, BinOp op)
{
    Evaluation evaluation;
    evaluation.eval = EvalPrimOp(std::move(left), std::move(right), std::move(op));
    return evaluation;
}

Evaluation Evaluation::makeEvalPrimOpL(Expression &&left, Value &&right, BinOp op)
{
    Evaluation evaluation;
    evaluation.eval = EvalPrimOpL(std::move(left), std::move(right), std::move(op));
    return evaluation;
}

Evaluation Evaluation::makeEvalPrimOpR(Expression &&left, Expression &&right, BinOp op)
{
    Evaluation evaluation;
    evaluation.eval = EvalPrimOpR(std::move(left), std::move(right), std::move(op));
    return evaluation;
}

Evaluation Evaluation::makeEvalApp(Variable &&param, Expression &&body, Environment &&captured, Value &&arg)
{
    Evaluation evaluation;
    evaluation.eval = EvalApp(std::move(param), std::move(body), std::move(captured), std::move(arg));
    return evaluation;
}

Evaluation Evaluation::makeEvalAppFun(Expression &&fun, Environment &&outside, Value &&arg)
{
    Evaluation evaluation;
    evaluation.eval = EvalAppFun(std::move(fun), std::move(outside), std::move(arg));
    return evaluation;
}

Evaluation Evaluation::makeEvalAppArg(Expression &&fun, Expression &&arg, Environment &&outside)
{
    Evaluation evaluation;
    evaluation.eval = EvalAppArg(std::move(fun), std::move(arg), std::move(outside));
    return evaluation;
}

Evaluation Evaluation::makeEvalFun(Variable &&param, Expression &&body, Environment &&env)
{
    Evaluation evaluation;
    evaluation.eval = EvalFun(std::move(param), std::move(body), std::move(env));
    return evaluation;
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
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        expression.expr);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Binding &binding)
{
    return os << binding.var << " -> " << *binding.val;
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
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Evaluation::EvalConst>)
                os << "EvalConst(" << *arg.val << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalVar>)
                os << "EvalVar(" << *arg.var << ", " << *arg.env << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalPair>)
                os << "EvalPair(Val " << *arg.left << ", Val " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalPairFst>)
                os << "EvalPairFst(" << *arg.left << ", Val " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalPairSnd>)
                os << "EvalPairSnd(" << *arg.left << ", " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalIfTrue>)
                os << "EvalIfTrue(if Val true then " << *arg.dotrue << " else " << *arg.dofalse << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalIfFalse>)
                os << "EvalIfFalse(if Val false then " << *arg.dotrue << " else " << *arg.dofalse << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalIf>)
                os << "EvalIf(if " << *arg.pred << " then " << *arg.dotrue << " else " << *arg.dofalse << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalPrimOp>)
                os << "EvalPrimOp(Val " << *arg.left << " " << arg.op << " Val " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalPrimOpL>)
                os << "EvalPrimOpL(" << *arg.left << " " << arg.op << " Val " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalPrimOpR>)
                os << "EvalPrimOpR(" << *arg.left << " " << arg.op << " " << *arg.right << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalApp>)
                os << "EvalApp(Val <" << *arg.param << " -> " << *arg.body << ", " << *arg.captured << "> Val "
                   << *arg.arg << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalAppFun>)
                os << "EvalAppFun(" << *arg.fun << " Val " << *arg.arg << ", " << *arg.outside << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalAppArg>)
                os << "EvalAppArg(" << *arg.fun << " " << *arg.arg << ", " << *arg.outside << ")";
            else if constexpr (std::is_same_v<T, Evaluation::EvalFun>)
                os << "EvalFun(fun " << *arg.param << " -> " << *arg.body << ", " << *arg.env << ")";
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        eval.eval);
    return os;
}
