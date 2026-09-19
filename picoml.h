#ifndef __PICOML_H
#define __PICOML_H

#include <memory>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

struct Value;
struct Variable;
struct Expression;
struct Environment;

struct Value
{
    struct Function
    {
        std::unique_ptr<Variable> param;
        std::unique_ptr<Expression> body;
        std::unique_ptr<Environment> env;

        Function(std::unique_ptr<Variable> param, std::unique_ptr<Expression> body, std::unique_ptr<Environment> env);
    };

    std::variant<int, Function> val;

    Value() = default;
    Value(int integer);
    Value(Variable &&param, Expression &&body, Environment &&env);
};

struct Variable
{
    std::string identifier;

    Variable() = default;
    Variable(std::string &&identifier);
};

struct Expression
{
    struct IfExpr
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        IfExpr(std::unique_ptr<Expression> pred, std::unique_ptr<Expression> dotrue,
               std::unique_ptr<Expression> dofalse);
    };

    std::variant<Value, Variable, IfExpr> expr;

    Expression() = default;
    Expression(Value &&val);
    Expression(Variable &&var);
    Expression(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
};

struct Binding
{
    Variable var;
    Value val;

    Binding() = default;
    Binding(Variable &&var, Value &&val);
};

struct Bindings
{
    std::vector<Binding> bindings;

    Bindings() = default;
    Bindings(Binding &&binding);
    Bindings(Bindings &&bindings, Binding &&binding);
};

struct Environment
{
    Bindings bindings;

    Environment() = default;
    Environment(Bindings &&bindings);
};

struct Evaluation
{
    struct EvalConst
    {
        Value val;

        EvalConst() = default;
        EvalConst(Value &&val);
    };

    std::variant<EvalConst> eval;

    Evaluation() = default;
    Evaluation(EvalConst &&eval);
};

std::ostream &operator<<(std::ostream &os, const Value &value);
std::ostream &operator<<(std::ostream &os, const Variable &variable);
std::ostream &operator<<(std::ostream &os, const Expression &expression);
std::ostream &operator<<(std::ostream &os, const Binding &binding);
std::ostream &operator<<(std::ostream &os, const Bindings &bindings);
std::ostream &operator<<(std::ostream &os, const Environment &env);
std::ostream &operator<<(std::ostream &os, const Evaluation &eval);

#endif
