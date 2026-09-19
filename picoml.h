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

        Function(Variable &&param, Expression &&body, Environment &&env);
    };

    struct Pair
    {
        std::unique_ptr<Value> left;
        std::unique_ptr<Value> right;

        Pair(Value &&left, Value &&right);
    };

    using Alternative = std::variant<int, bool, Function, Pair>;

    Alternative val;

    explicit Value(Alternative &&alternative);

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

        IfExpr(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    };

    struct BinaryExpr
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        BinOp op;

        BinaryExpr(Expression &&left, Expression &&right, BinOp op);
    };

    struct PairExpr
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        PairExpr(Expression &&left, Expression &&right);
    };

    struct FunExpr
    {
        std::unique_ptr<Variable> param;
        std::unique_ptr<Expression> body;

        FunExpr(Variable &&param, Expression &&body);
    };

    struct AppExpr
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Expression> arg;

        AppExpr(Expression &&fun, Expression &&arg);
    };

    struct LetExpr
    {
        std::unique_ptr<Variable> var;
        std::unique_ptr<Expression> pre;
        std::unique_ptr<Expression> body;

        LetExpr(Variable &&var, Expression &&pre, Expression &&body);
    };

    using Alternative = std::variant<Value, Variable, IfExpr, BinaryExpr, PairExpr, FunExpr, AppExpr, LetExpr>;

    Alternative expr;

    explicit Expression(Alternative &&alternative);

    static Expression makeIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    static Expression makeBinary(Expression &&left, Expression &&right, BinOp op);
    static Expression makePair(Expression &&left, Expression &&right);
    static Expression makeFunction(Variable &&param, Expression &&body);
    static Expression makeApp(Expression &&fun, Expression &&arg);
    static Expression makeLet(Variable &&var, Expression &&pre, Expression &&body);
};

struct Binding
{
    std::unique_ptr<Variable> var;
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

struct Rule
{
    struct EvalConst
    {
        std::unique_ptr<Value> val;

        EvalConst(Value &&val);
    };

    struct EvalVar
    {
        std::unique_ptr<Variable> var;

        EvalVar(Variable &&var);
    };

    struct EvalPair
    {
        std::unique_ptr<Value> left;
        std::unique_ptr<Value> right;

        EvalPair(Value &&left, Value &&right);
    };

    struct EvalPairFst
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Value> right;

        EvalPairFst(Expression &&left, Value &&right);
    };

    struct EvalPairSnd
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        EvalPairSnd(Expression &&left, Expression &&right);
    };

    struct EvalIfTrue
    {
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        EvalIfTrue(Expression &&dotrue, Expression &&dofalse);
    };

    struct EvalIfFalse
    {
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        EvalIfFalse(Expression &&dotrue, Expression &&dofalse);
    };

    struct EvalIf
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        EvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    };

    struct EvalPrimOp
    {
        std::unique_ptr<Value> left;
        std::unique_ptr<Value> right;
        BinOp op;

        EvalPrimOp(Value &&left, Value &&right, BinOp op);
    };

    struct EvalPrimOpL
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Value> right;
        BinOp op;

        EvalPrimOpL(Expression &&left, Value &&right, BinOp op);
    };

    struct EvalPrimOpR
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        BinOp op;

        EvalPrimOpR(Expression &&left, Expression &&right, BinOp op);
    };

    struct EvalApp
    {
        std::unique_ptr<Variable> param;
        std::unique_ptr<Expression> body;
        std::unique_ptr<Environment> captured;
        std::unique_ptr<Value> arg;

        EvalApp(Variable &&param, Expression &&body, Environment &&captured, Value &&arg);
    };

    struct EvalAppFun
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Value> arg;

        EvalAppFun(Expression &&fun, Value &&arg);
    };

    struct EvalAppArg
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Expression> arg;

        EvalAppArg(Expression &&fun, Expression &&arg);
    };

    struct EvalFun
    {
        std::unique_ptr<Variable> param;
        std::unique_ptr<Expression> body;

        EvalFun(Variable &&param, Expression &&body);
    };

    struct EvalLet
    {
        std::unique_ptr<Variable> var;
        std::unique_ptr<Value> val;
        std::unique_ptr<Expression> body;

        EvalLet(Variable &&var, Value &&val, Expression &&body);
    };

    struct EvalLetBinding
    {
        std::unique_ptr<Variable> var;
        std::unique_ptr<Expression> pre;
        std::unique_ptr<Expression> body;

        EvalLetBinding(Variable &&var, Expression &&pre, Expression &&body);
    };

    using Alternative =
        std::variant<EvalConst, EvalVar, EvalPair, EvalPairFst, EvalPairSnd, EvalIfTrue, EvalIfFalse, EvalIf,
                     EvalPrimOp, EvalPrimOpL, EvalPrimOpR, EvalApp, EvalAppFun, EvalAppArg, EvalFun, EvalLet,
                     EvalLetBinding>;

    Alternative rule;

    explicit Rule(Alternative &&alternative);

    static Rule makeEvalConst(Value &&val);
    static Rule makeEvalVar(Variable &&var);
    static Rule makeEvalPair(Value &&left, Value &&right);
    static Rule makeEvalPairFst(Expression &&left, Value &&right);
    static Rule makeEvalPairSnd(Expression &&left, Expression &&right);
    static Rule makeEvalIfTrue(Expression &&dotrue, Expression &&dofalse);
    static Rule makeEvalIfFalse(Expression &&dotrue, Expression &&dofalse);
    static Rule makeEvalIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    static Rule makeEvalPrimOp(Value &&left, Value &&right, BinOp op);
    static Rule makeEvalPrimOpL(Expression &&left, Value &&right, BinOp op);
    static Rule makeEvalPrimOpR(Expression &&left, Expression &&right, BinOp op);
    static Rule makeEvalApp(Variable &&param, Expression &&body, Environment &&captured, Value &&arg);
    static Rule makeEvalAppFun(Expression &&fun, Value &&arg);
    static Rule makeEvalAppArg(Expression &&fun, Expression &&arg);
    static Rule makeEvalFun(Variable &&param, Expression &&body);
    static Rule makeEvalLet(Variable &&var, Value &&val, Expression &&body);
    static Rule makeEvalLetBinding(Variable &&var, Expression &&pre, Expression &&body);
};

struct Evaluation
{
    Rule rule;
    std::unique_ptr<Environment> env;

    Evaluation(Rule &&rule, Environment &&env);
};

std::ostream &operator<<(std::ostream &os, const BinOp &binop);
std::ostream &operator<<(std::ostream &os, const Value &value);
std::ostream &operator<<(std::ostream &os, const Variable &variable);
std::ostream &operator<<(std::ostream &os, const Expression &expression);
std::ostream &operator<<(std::ostream &os, const Binding &binding);
std::ostream &operator<<(std::ostream &os, const Bindings &bindings);
std::ostream &operator<<(std::ostream &os, const Environment &env);
std::ostream &operator<<(std::ostream &os, const Rule &rule);
std::ostream &operator<<(std::ostream &os, const Evaluation &eval);

#endif
