#pragma once
#include "Token.h"
#include <memory>
#include <variant>
#include <string>

using LiteralValue = std::variant<std::monostate, double, std::string, bool>;

struct Expr
{
    virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

struct Binary : public Expr
{
    ExprPtr left;
    Token op;
    ExprPtr right;

    Binary(ExprPtr left, Token op, ExprPtr right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

struct Unary : public Expr
{
    Token op;
    ExprPtr right;

    Unary(Token op, ExprPtr right)
        : op(std::move(op)), right(std::move(right)) {}
};

struct Literal : public Expr
{
    LiteralValue value; //double, string, bool, null

    explicit Literal(LiteralValue value)
        : value(std::move(value)) {}
};

struct Grouping : public Expr
{
    ExprPtr expression;

    explicit Grouping(ExprPtr expression)
        : expression(std::move(expression)) {}
};