#include "picoml.h"

#include <memory>

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
