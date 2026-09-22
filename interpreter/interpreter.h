#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include <memory>
#include <sstream>
#include <variant>

#include "picoml.h"

template <typename T> std::string to_string(const T &value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

struct Atom
{
    std::unique_ptr<Value> val;

    explicit Atom(Value &&val);
};

struct Rewrite
{
    std::unique_ptr<Expression> expr;

    explicit Rewrite(Expression &&expr);
};

struct Hole
{
    struct PairLeft
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Value> right;

        PairLeft(Expression &&left, Value &&right);
    };

    struct PairRight
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;

        PairRight(Expression &&left, Expression &&right);
    };

    struct MonOpR
    {
        std::unique_ptr<Expression> right;
        BinOp op;

        MonOpR(Expression &&right, BinOp op);
    };

    struct PrimOpL
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Value> right;
        BinOp op;

        PrimOpL(Expression &&left, Value &&right, BinOp op);
    };

    struct PrimOpR
    {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        BinOp op;

        PrimOpR(Expression &&left, Expression &&right, BinOp op);
    };

    struct If
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;

        If(Expression &&pred, Expression &&dotrue, Expression &&dofalse);
    };

    struct AppFun
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Value> arg;

        AppFun(Expression &&fun, Value &&arg);
    };

    struct AppArg
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Expression> arg;

        AppArg(Expression &&fun, Expression &&arg);
    };

    struct LetBindings
    {
        std::unique_ptr<Variable> var;
        std::unique_ptr<Expression> pre;
        std::unique_ptr<Expression> body;

        LetBindings(Variable &&var, Expression &&pre, Expression &&body);
    };

    using Alternative = std::variant<PairLeft, PairRight, MonOpR, PrimOpL, PrimOpR, If, AppFun, AppArg, LetBindings>;

    Alternative hole;

    explicit Hole(Alternative &&alternative);

    std::string before() const;
    std::string after() const;
    std::string cursor() const;
};

struct AppliedRule
{
    using Alternative = std::variant<Atom, Rewrite, Hole>;

    Alternative result;
    std::unique_ptr<Environment> env;

    explicit AppliedRule(Alternative &&alternative);
    AppliedRule(Alternative &&alternative, Environment &&env);
};

struct Applier
{
    Environment env;

    AppliedRule operator()(Rule::EvalConst &rule);
    AppliedRule operator()(Rule::EvalVar &rule);
    AppliedRule operator()(Rule::EvalPair &rule);
    AppliedRule operator()(Rule::EvalPairFst &rule);
    AppliedRule operator()(Rule::EvalPairSnd &rule);
    AppliedRule operator()(Rule::EvalIfTrue &rule);
    AppliedRule operator()(Rule::EvalIfFalse &rule);
    AppliedRule operator()(Rule::EvalIf &rule);
    AppliedRule operator()(Rule::EvalMonOp &rule);
    AppliedRule operator()(Rule::EvalMonOpR &rule);
    AppliedRule operator()(Rule::EvalPrimOp &rule);
    AppliedRule operator()(Rule::EvalPrimOpL &rule);
    AppliedRule operator()(Rule::EvalPrimOpR &rule);
    AppliedRule operator()(Rule::EvalApp &rule);
    AppliedRule operator()(Rule::EvalAppFun &rule);
    AppliedRule operator()(Rule::EvalAppArg &rule);
    AppliedRule operator()(Rule::EvalFun &rule);
    AppliedRule operator()(Rule::EvalLet &rule);
    AppliedRule operator()(Rule::EvalLetBinding &rule);
};

#endif
