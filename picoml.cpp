#include "picoml.h"

#include <memory>
#include <type_traits>

Value::Function::Function(std::unique_ptr<Variable> param, std::unique_ptr<Expression> body,
                          std::unique_ptr<Environment> env)
    : param(std::move(param)), body(std::move(body)), env(std::move(env))
{
}

Expression::IfExpr::IfExpr(std::unique_ptr<Expression> pred, std::unique_ptr<Expression> dotrue,
                           std::unique_ptr<Expression> dofalse)
    : pred(std::move(pred)), dotrue(std::move(dotrue)), dofalse(std::move(dofalse))
{
}

Value::Value(int integer) : val(integer) {};
Value::Value(Variable &&param, Expression &&body, Environment &&env)
    : val(Function(std::make_unique<Variable>(param), std::make_unique<Expression>(std::move(body)),
                   std::make_unique<Environment>(std::move(env))))
{
}

Variable::Variable(std::string &&identifier) : identifier(std::move(identifier)) {};

Expression::Expression(Value &&val) : expr(std::move(val)) {};
Expression::Expression(Variable &&var) : expr(std::move(var)) {};

Binding::Binding(Variable &&var, Value &&val) : var(std::move(var)), val(std::move(val))
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

std::ostream &operator<<(std::ostream &os, const Value &value)
{
    std::visit(
        [&](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>)
                os << "INTEGER(" << arg << ")";
            else if constexpr (std::is_same_v<T, Value::Function>)
                os << "FUNCTION(" << *arg.param << " -> " << *arg.body << ", " << *arg.env << ")";
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
            else
                static_assert(always_false_v<T>, "non-exhaustive visitor");
        },
        expression.expr);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Binding &binding)
{
    return os << binding.var << " -> " << binding.val;
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
