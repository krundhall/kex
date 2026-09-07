#pragma once
#include "Token.h"
#include <memory>
#include <variant>
#include <string>

using LiteralValue = std::variant<
    std::monostate,   // null/void
    int,         // INT_LITERAL
    double,         // FLOAT_LITERAL
    char,           // CHAR_LITERAL
    std::string,   // STRING_LITERAL
    bool>;        // true/false

struct Expr
{
    virtual ~Expr() = default;
};

using ExprPtr = std::unique_ptr<Expr>;

void print_ast(const Expr* expr);

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

struct UpdateExpr : public Expr
{
    Token op; // INCREMENT or DECREMENT
    ExprPtr operand; // Target
    bool postfix; // true=x++, false=++x

    UpdateExpr(Token op, ExprPtr operand, bool postfix)
        : op(std::move(op)), operand(std::move(operand)), postfix(postfix) {}
};

struct ArrayAccess : public Expr
{
    ExprPtr target;
    ExprPtr index;

    ArrayAccess(ExprPtr target, ExprPtr index)
        : target(std::move(target)), index(std::move(index)) {}
};