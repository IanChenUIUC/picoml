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

Value::Value(int integer) : val(integer) {};

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

Expression Expression::makeMixedPair(Expression &&left, Value &&right)
{
    Expression expression;
    expression.expr =
        MixedPair{std::make_unique<Expression>(std::move(left)), std::make_unique<Value>(std::move(right))};
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

Evaluation::Evaluation(EvalConst &&eval) : eval(std::move(eval))
{
}

Evaluation::EvalConst::EvalConst(Value &&val) : val(std::move(val))
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
            else if constexpr (std::is_same_v<T, Expression::MixedPair>)
                os << "(" << *arg.left << ", " << *arg.right << ")";
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
                os << "EvalConst(" << arg.val << ")";
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        eval.eval);
    return os;
}
