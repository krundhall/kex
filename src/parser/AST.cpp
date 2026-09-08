#include "AST.h"
#include <iostream>

class AstPrinter : public ExprVisitor
{
public:
    void print(const Expr* expr)
    {
        if (expr)
        {
            const_cast<Expr*>(expr)->accept(*this);
        }
    }

    void visitBinary(Binary& expr) override
    {
        std::cout << "(" << expr.op.lexeme << " ";
        print(expr.left.get());
        std::cout << " ";
        print(expr.right.get());
        std::cout << ")";
    }

    void visitUnary(Unary& expr) override
    {
        std::cout << "(" << expr.op.lexeme << " ";
        print(expr.right.get());
        std::cout << ")";
    }

    void visitLiteral(Literal& expr) override
    {
        if (std::holds_alternative<int>(expr.value))
            std::cout << std::get<int>(expr.value);
        else if (std::holds_alternative<double>(expr.value))
            std::cout << std::get<double>(expr.value);
        else if (std::holds_alternative<char>(expr.value))
            std::cout << "'" << std::get<char>(expr.value) << "'";
        else if (std::holds_alternative<std::string>(expr.value))
            std::cout << "\"" << std::get<std::string>(expr.value) << "\"";
        else if (std::holds_alternative<bool>(expr.value))
            std::cout << (std::get<bool>(expr.value) ? "true" : "false");
        else if (std::holds_alternative<std::monostate>(expr.value))
            std::cout << "null";
    }

    void visitVariable(Variable& expr) override
    {
        std::cout << expr.name.lexeme;
    }

    void visitAssign(Assign& expr) override
    {
        std::cout << "(" << expr.name.lexeme << " = ";
        print(expr.value.get());
        std::cout << ")";
    }

    void visitGrouping(Grouping& expr) override
    {
        std::cout << "(group ";
        print(expr.expression.get());
        std::cout << ")";
    }

    void visitUpdateExpr(UpdateExpr& expr) override
    {
        std::cout << "(";
        if (expr.postfix)
        {
            print(expr.operand.get());
            std::cout << " " << expr.op.lexeme;
        }
        else
        {
            std::cout << expr.op.lexeme << " ";
            print(expr.operand.get());
        }
        std::cout << ")";
    }

    void visitArrayAccess(ArrayAccess& expr) override
    {
        std::cout << "(index ";
        print(expr.target.get());
        std::cout << " ";
        print(expr.index.get());
        std::cout << ")";
    }
};

void print_ast(const Expr* expr)
{
    AstPrinter printer;
    printer.print(expr);
}