#pragma once

#include "AST.h"
#include "Stmt.h"
#include "Environment.h"
#include <memory>
#include <vector>

class Interpreter : public ExprVisitor, public StmtVisitor
{
private:
    std::shared_ptr<Environment> environment = std::make_shared<Environment>();
    LiteralValue last_value = std::monostate{};

    LiteralValue evaluate(Expr& expr);
    void execute(Stmt& stmt);

public:
    Interpreter() = default;

    void interpret(const std::vector<StmtPtr>& statements);

    // StmtVisitor Overrides
    void visitExpressionStmt(ExpressionStmt& stmt) override;
    void visitVarStmt(VarStmt& stmt) override;
    void visitBlockStmt(BlockStmt& stmt) override;

    void visitPrintStmt(PrintStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    
    // ExprVisitor Overrides
    void visitBinary(Binary& expr) override;
    void visitUnary(Unary& expr) override;
    void visitLiteral(Literal& expr) override;
    void visitVariable(Variable& expr) override;
    void visitAssign(Assign& expr) override;
    void visitGrouping(Grouping& expr) override;
    void visitUpdateExpr(UpdateExpr& expr) override;
    void visitArrayAccess(ArrayAccess& expr) override;
};