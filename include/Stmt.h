#pragma once
#include "AST.h"
#include <memory>
#include <vector>
#include "Token.h"
// #include "Expr.h"

struct ExpressionStmt;
struct VarStmt;
struct BlockStmt;
struct PrintStmt;

class StmtVisitor
{
public:
    virtual ~StmtVisitor() = default;
    virtual void visitExpressionStmt(ExpressionStmt& stmt) = 0;
    virtual void visitVarStmt(VarStmt& stmt) = 0;
    virtual void visitBlockStmt(BlockStmt& stmt) = 0;
    virtual void visitPrintStmt(PrintStmt& stmt) = 0;
};

struct Stmt
{
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;


// Expression Statement ("x = 5;")
struct ExpressionStmt : public Stmt
{
    ExprPtr expression;

    explicit ExpressionStmt(ExprPtr expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override
    {
        visitor.visitExpressionStmt(*this);
    }
};

// Variable Declaration Statement ("int x = 5;" or "auto x = 5;")
struct VarStmt : public Stmt
{
    Token typeToken;    // int,float,auto,struct
    Token name;         // var identifier 
    ExprPtr initializer; // optional init expression (can be nullptr)

    VarStmt(Token typeToken, Token name, ExprPtr initializer)
        : typeToken(std::move(typeToken)), name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) override
    {
        visitor.visitVarStmt(*this);
    }
};

// Block Statement ("{ let x = 5; ... }")
struct BlockStmt : public Stmt 
{
    std::vector<StmtPtr> statements;

    explicit BlockStmt(std::vector<StmtPtr> statements)
        : statements(std::move(statements)) {}

    void accept(StmtVisitor& visitor) override 
    {
        visitor.visitBlockStmt(*this);
    }
};

struct PrintStmt : public Stmt
{
    ExprPtr expression;

    explicit PrintStmt(ExprPtr expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) override
    {
        visitor.visitPrintStmt(*this);
    }
};