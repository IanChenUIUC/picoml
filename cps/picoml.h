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
        std::unique_ptr<Expression> continuation;

        TransVar(Variable &&var, Expression &&continuation);
    };

    struct TransConst
    {
        std::unique_ptr<Value> val;
        std::unique_ptr<Expression> continuation;

        TransConst(Value &&val, Expression &&continuation);
    };

    struct TransIf
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;
        std::unique_ptr<Expression> continuation;

        TransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Expression &&continuation);
    };

    struct TransApp
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Expression> arg;
        std::unique_ptr<Expression> continuation;

        TransApp(Expression &&fun, Expression &&arg, Expression &&continuation);
    };

    struct TransBinop
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        BinOp op;
        std::unique_ptr<Expression> continuation;

        TransBinop(Expression &&left, Expression &&right, BinOp op, Expression &&continuation);
    };

    struct TransMonop
    {
        std::unique_ptr<Expression> right;
        BinOp op;
        std::unique_ptr<Expression> continuation;

        TransMonop(Expression &&right, BinOp op, Expression &&continuation);
    };

    using Alternative = std::variant<TransVar, TransConst, TransIf, TransApp, TransBinop, TransMonop>;

    Alternative rule;
    explicit Rule(Alternative &&alternative);

    static Rule makeTransVar(Variable &&var, Expression &&continuation);
    static Rule makeTransConst(Value &&val, Expression &&continuation);
    static Rule makeTransIf(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Expression &&continuation);
    static Rule makeTransApp(Expression &&fun, Expression &&arg, Expression &&continuation);
    static Rule makeTransBinop(Expression &&left, Expression &&right, BinOp op, Expression &&continuation);
    static Rule makeTransMonop(Expression &&right, BinOp op, Expression &&continuation);
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
std::ostream &operator<<(std::ostream &os, const Rule &rule);

#endif
