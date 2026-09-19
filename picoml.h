#ifndef __PICOML_H
#define __PICOML_H

#include <memory>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

struct BinOp;
struct Value;
struct Variable;
struct Expression;
struct Environment;

struct BinOp
{
    enum Op
    {
        ADD,
        SUB,
        MUL,
        DIV,
    } op;

    BinOp() = default;
    BinOp(Op op);
};

struct Value
{
    struct Function
    {
        std::unique_ptr<Variable> param;
        std::unique_ptr<Expression> body;
        std::unique_ptr<Environment> env;

        Function(std::unique_ptr<Variable> param, std::unique_ptr<Expression> body, std::unique_ptr<Environment> env);
    };

    struct Pair
    {
        std::unique_ptr<Value> left;
        std::unique_ptr<Value> right;

        Pair(std::unique_ptr<Value> left, std::unique_ptr<Value> right);
    };

    std::variant<int, bool, Function, Pair> val;

    Value() = default;
    Value(int integer);
    Value(bool boolean);

    static Value makeFunction(Variable &&param, Expression &&body, Environment &&env);
    static Value makePair(Value &&left, Value &&right);
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

    struct BinaryExpr
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        BinOp op;

        BinaryExpr(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, BinOp op);
    };

    struct PairExpr
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
    };

    std::variant<Value, Variable, IfExpr, BinaryExpr, PairExpr> expr;

    Expression() = default;
    Expression(Value &&val);
    Expression(Variable &&var);

    static Expression makeIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    static Expression makeBinary(Expression &&left, Expression &&right, BinOp &&op);
    static Expression makePair(Expression &&left, Expression &&right);
};

struct Binding
{
    Variable var;
    std::unique_ptr<Value> val;

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
        std::unique_ptr<Value> val;

        EvalConst() = default;
        EvalConst(Value &&val);
    };

    struct EvalVar
    {
        std::unique_ptr<Variable> var;
        std::unique_ptr<Environment> env;

        EvalVar() = default;
        EvalVar(Variable &&var, Environment &&env);
    };

    struct EvalPair
    {
        std::unique_ptr<Value> left;
        std::unique_ptr<Value> right;

        EvalPair() = default;
        EvalPair(Value &&left, Value &&right);
    };

    struct EvalPairFst
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Value> right;

        EvalPairFst() = default;
        EvalPairFst(Expression &&left, Value &&right);
    };

    struct EvalPairSnd
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        EvalPairSnd() = default;
        EvalPairSnd(Expression &&left, Expression &&right);
    };

    struct EvalIfTrue
    {
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        EvalIfTrue() = default;
        EvalIfTrue(Expression &&dotrue, Expression &&dofalse);
    };

    struct EvalIfFalse
    {
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        EvalIfFalse() = default;
        EvalIfFalse(Expression &&dotrue, Expression &&dofalse);
    };

    struct EvalIf
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        EvalIf() = default;
        EvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    };

    std::variant<EvalConst, EvalVar, EvalPair, EvalPairFst, EvalPairSnd, EvalIfTrue, EvalIfFalse, EvalIf> eval;

    Evaluation() = default;
    Evaluation(EvalConst &&eval);
    Evaluation(EvalVar &&eval);

    static Evaluation makeEvalPair(Value &&left, Value &&right);
    static Evaluation makeEvalPairFst(Expression &&left, Value &&right);
    static Evaluation makeEvalPairSnd(Expression &&left, Expression &&right);
    static Evaluation makeEvalIfTrue(Expression &&dotrue, Expression &&dofalse);
    static Evaluation makeEvalIfFalse(Expression &&dotrue, Expression &&dofalse);
    static Evaluation makeEvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
};

std::ostream &operator<<(std::ostream &os, const BinOp &binop);
std::ostream &operator<<(std::ostream &os, const Value &value);
std::ostream &operator<<(std::ostream &os, const Variable &variable);
std::ostream &operator<<(std::ostream &os, const Expression &expression);
std::ostream &operator<<(std::ostream &os, const Binding &binding);
std::ostream &operator<<(std::ostream &os, const Bindings &bindings);
std::ostream &operator<<(std::ostream &os, const Environment &env);
std::ostream &operator<<(std::ostream &os, const Evaluation &eval);

#endif
