#ifndef __PICOML_H
#define __PICOML_H

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

struct BinOp;
struct Value;
struct Variable;
struct Expression;

struct BinOp
{
    enum Op
    {
        ADD,
        SUB,
        MUL,
        DIV,
        LT,
        GT,
        LEQ,
        GEQ,
        EQ,
        NEQ,
    } op;

    BinOp() = default;
    BinOp(Op op);
};

struct Value
{
    using Alternative = std::variant<int, bool>;

    Alternative val;

    explicit Value(Alternative &&alternative);
};

struct Variable
{
    std::string identifier;

    Variable() = default;
    Variable(std::string &&identifier);
};

bool operator<(const Variable &lhs, const Variable &rhs);

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

    struct UnaryExpr
    {
        std::unique_ptr<Expression> right;
        BinOp op;

        UnaryExpr(Expression &&right, BinOp op);
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

    using Alternative =
        std::variant<Value, Variable, IfExpr, BinaryExpr, UnaryExpr, PairExpr, FunExpr, AppExpr, LetExpr>;

    Alternative expr;

    explicit Expression(Alternative &&alternative);

    static Expression makeIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    static Expression makeBinary(Expression &&left, Expression &&right, BinOp op);
    static Expression makeUnary(Expression &&right, BinOp op);
    static Expression makePair(Expression &&left, Expression &&right);
    static Expression makeFunction(Variable &&param, Expression &&body);
    static Expression makeApp(Expression &&fun, Expression &&arg);
    static Expression makeLet(Variable &&var, Expression &&pre, Expression &&body);
};

struct Rule
{
    struct TransVar
    {
        std::unique_ptr<Variable> var;

        TransVar(Variable &&var);
    };

    struct TransConst
    {
        std::unique_ptr<Value> val;

        TransConst(Value &&val);
    };

    struct TransIf
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        TransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    };

    struct TransApp
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Expression> arg;

        TransApp(Expression &&fun, Expression &&arg);
    };

    struct TransBinop
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        BinOp op;

        TransBinop(Expression &&left, Expression &&right, BinOp op);
    };

    struct TransMonop
    {
        std::unique_ptr<Expression> right;
        BinOp op;

        TransMonop(Expression &&right, BinOp op);
    };

    struct TransFun
    {
        std::unique_ptr<Variable> param;
        std::unique_ptr<Expression> body;

        TransFun(Variable &&param, Expression &&body);
    };

    struct TransLetIn
    {
        std::unique_ptr<Variable> var;
        std::unique_ptr<Expression> pre;
        std::unique_ptr<Expression> body;

        TransLetIn(Variable &&var, Expression &&pre, Expression &&body);
    };

    using Alternative =
        std::variant<TransVar, TransConst, TransIf, TransApp, TransBinop, TransMonop, TransFun, TransLetIn>;

    Alternative rule;
    std::unique_ptr<Expression> continuation;

    Rule(Alternative &&alternative, Expression &&continuation);

    static Rule makeTransVar(Variable &&var, Expression &&continuation);
    static Rule makeTransConst(Value &&val, Expression &&continuation);
    static Rule makeTransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Expression &&continuation);
    static Rule makeTransApp(Expression &&fun, Expression &&arg, Expression &&continuation);
    static Rule makeTransBinop(Expression &&left, Expression &&right, BinOp op, Expression &&continuation);
    static Rule makeTransMonop(Expression &&right, BinOp op, Expression &&continuation);
    static Rule makeTransFun(Variable &&param, Expression &&body, Expression &&continuation);
    static Rule makeTransLetIn(Variable &&var, Expression &&pre, Expression &&body, Expression &&continuation);
};

struct Paren
{
    const Expression &expression;
};

std::ostream &operator<<(std::ostream &os, Paren paren);
std::ostream &operator<<(std::ostream &os, const BinOp &binop);
std::ostream &operator<<(std::ostream &os, const Value &value);
std::ostream &operator<<(std::ostream &os, const Variable &variable);
std::ostream &operator<<(std::ostream &os, const Expression &expression);
std::ostream &operator<<(std::ostream &os, const Rule::Alternative &alternative);
std::ostream &operator<<(std::ostream &os, const Rule &rule);

#endif
