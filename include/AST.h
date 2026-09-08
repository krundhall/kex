#pragma once
#include "Token.h"
#include <memory>
#include <variant>
#include <string>

// Forward declarations of AST nodes for ExprVisitor
struct Binary;
struct Unary;
struct Literal;
struct Variable;
struct Grouping;
struct UpdateExpr;
struct ArrayAccess;
struct Assign;

class ExprVisitor
{
public:
    virtual ~ExprVisitor() = default;
    virtual void visitBinary(Binary& expr) = 0;
    virtual void visitUnary(Unary& expr) = 0;
    virtual void visitLiteral(Literal& expr) = 0;
    virtual void visitVariable(Variable& expr) = 0;
    virtual void visitGrouping(Grouping& expr) = 0;
    virtual void visitUpdateExpr(UpdateExpr& expr) = 0;
    virtual void visitArrayAccess(ArrayAccess& expr) = 0;
    virtual void visitAssign(Assign& expr) = 0;
};

using LiteralValue = std::variant<
    std::monostate,   // null/void
    int,              // INT_LITERAL
    double,           // FLOAT_LITERAL
    char,             // CHAR_LITERAL
    std::string,      // STRING_LITERAL
    bool>;            // true/false

struct Expr
{
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
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

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitBinary(*this);
    }
};

struct Unary : public Expr
{
    Token op;
    ExprPtr right;

    Unary(Token op, ExprPtr right)
        : op(std::move(op)), right(std::move(right)) {}

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitUnary(*this);
    }
};

struct Literal : public Expr
{
    LiteralValue value; // double, string, bool, null

    explicit Literal(LiteralValue value)
        : value(std::move(value)) {}

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitLiteral(*this);
    }
};

struct Variable : public Expr
{
    Token name;

    explicit Variable(Token name)
        : name(std::move(name)) {}

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitVariable(*this);
    }
};

struct Grouping : public Expr
{
    ExprPtr expression;

    explicit Grouping(ExprPtr expression)
        : expression(std::move(expression)) {}

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitGrouping(*this);
    }
};

struct UpdateExpr : public Expr
{
    Token op;        // INCREMENT or DECREMENT
    ExprPtr operand; // Target
    bool postfix;    // true=x++, false=++x

    UpdateExpr(Token op, ExprPtr operand, bool postfix)
        : op(std::move(op)), operand(std::move(operand)), postfix(postfix) {}

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitUpdateExpr(*this);
    }
};

struct ArrayAccess : public Expr
{
    ExprPtr target;
    ExprPtr index;

    ArrayAccess(ExprPtr target, ExprPtr index)
        : target(std::move(target)), index(std::move(index)) {}

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitArrayAccess(*this);
    }
};

struct Assign : public Expr
{
    Token name;
    ExprPtr value;

    Assign(Token name, ExprPtr value)
        : name(std::move(name)), value(std::move(value)) {}

    void accept(ExprVisitor& visitor) override
    {
        visitor.visitAssign(*this);
    }
};