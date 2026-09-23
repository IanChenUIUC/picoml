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

struct Rewrite;
struct Hole;
using AppliedRule = std::variant<Expression, Rewrite, Hole>;

struct Rewrite
{
    std::unique_ptr<Expression> expr;
    std::unique_ptr<Expression> continuation;

    Rewrite(Expression &&expr, Expression &&continuation);
};

struct Hole
{
    struct IfR
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;
        Variable binder;
        IfR(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Variable &&binder);
    };

    struct If
    {
        std::unique_ptr<Expression> pred;
        std::unique_ptr<Expression> dotrue;
        std::unique_ptr<Expression> dofalse;
        Variable binder;
        If(Expression &&pred, Expression &&dotrue, Expression &&dofalse, Variable &&binder);
    };

    struct App
    {
        std::unique_ptr<Expression> fun;
        std::unique_ptr<Expression> arg;
        Variable binder1;
        Variable binder2;
        App(Expression &&fun, Expression &&arg, Variable &&binder1, Variable &&binder2);
    };

    struct BinOp
    {
        std::unique_ptr<Expression> lhs;
        std::unique_ptr<Expression> rhs;
        Variable binder1;
        Variable binder2;
        ::BinOp binop;
        BinOp(Expression &&lhs, Expression &&rhs, Variable &&binder1, Variable &&binder2, ::BinOp binop);
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
    std::unique_ptr<Expression> continuation;

    AppliedRule plug(Expression &&expr);
    std::string getNextCursor();
    std::unique_ptr<Expression> getNextContinuation();
    std::string render(const std::string &inner) const;

    Hole(Alternative &&alternative, Expression &&continuation);
};

struct Applier
{
    std::unique_ptr<Expression> continuation;
    int &epoch;

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
