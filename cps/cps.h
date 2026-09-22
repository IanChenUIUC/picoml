#ifndef __CPS_H
#define __CPS_H

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

struct Rewrite
{
    std::unique_ptr<Expression> expr;
    explicit Rewrite(Expression &&expr);
};

struct Hole
{
    struct IfR
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dofalse;
        IfR(Expression &&pred, Expression &&dofalse);
    };

    struct If
    {
        std::unique_ptr<Expression> pred;
        explicit If(Expression &&pred);
    };

    struct App
    {
        std::unique_ptr<Expression> arg;
        explicit App(Expression &&arg);
    };

    struct BinOp
    {
        std::unique_ptr<Expression> rhs;
        explicit BinOp(Expression &&arg);
    };

    struct Fun
    {
    };

    struct LetIn
    {
        std::unique_ptr<Expression> pre;
        explicit LetIn(Expression &&pre);
    };

    using Alternative = std::variant<IfR, If, App, BinOp, Fun, LetIn>;

    Alternative hole;

    explicit Hole(Alternative &&alternative);
};

struct AppliedRule
{
    using Alternative = std::variant<Value, Variable, Rewrite, Hole>;

    Alternative result;
    std::unique_ptr<Expression> continuation;

    explicit AppliedRule(Alternative &&alernative);
    AppliedRule(Alternative &&alernative, Expression &&continuation);
};

struct Applier
{
    std::unique_ptr<Expression> continuation;

    AppliedRule operator()(Rule::TransVar &rule);
    AppliedRule operator()(Rule::TransConst &rule);
    AppliedRule operator()(Rule::TransIf &rule);
    AppliedRule operator()(Rule::TransApp &rule);
    AppliedRule operator()(Rule::TransBinop &rule);
    AppliedRule operator()(Rule::TransMonop &rule);
    AppliedRule operator()(Rule::TransFun &rule);
    AppliedRule operator()(Rule::TransLetIn &rule);
};

#endif
