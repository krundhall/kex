#include "AST.h"
#include <memory>
#include <iostream>
#include <string>

void print_ast(const Expr* expr)
{
    if (!expr) return;

    // Literal
    if (auto l = dynamic_cast<const Literal*>(expr))
    {
        if (std::holds_alternative<int>(l->value))
            std::cout << std::get<int>(l->value);

        else if (std::holds_alternative<double>(l->value))
            std::cout << std::get<double>(l->value);

        else if (std::holds_alternative<char>(l->value))
            std::cout << "'" << std::get<char>(l->value) << "'";

        else if (std::holds_alternative<std::string>(l->value))
            std::cout << "\"" << std::get<std::string>(l->value) << "\"";

        else if (std::holds_alternative<bool>(l->value))
            std::cout << (std::get<bool>(l->value) ? "true" : "false");

        else if (std::holds_alternative<std::monostate>(l->value))
            std::cout << "null";
    }
    // Unary ( -x )
    else if (auto u = dynamic_cast<const Unary*>(expr))
    {
        std::cout << "(" << u->op.lexeme << " ";
        print_ast(u->right.get());
        std::cout << ")";
    }

    // Binary ( x + y )
    else if (auto b = dynamic_cast<const Binary*>(expr))
    {
        std::cout << "(" << b->op.lexeme << " ";
        print_ast(b->left.get());
        std::cout << " ";
        print_ast(b->right.get());
        std::cout << ")";
    }

    // Grouping ( (x) )
    else if (auto g = dynamic_cast<const Grouping*>(expr))
    {
        std::cout << "(group ";
        print_ast(g->expression.get());
        std::cout << ")";
    }

    else if (auto u = dynamic_cast<const UpdateExpr*>(expr))
    {
        std::cout << "(";
        if (u->postfix) // x++
        {
            print_ast(u->operand.get());
            std::cout << " " << u->op.lexeme;
        }
        else // ++x
        {
            std::cout << u->op.lexeme << " ";
            print_ast(u->operand.get());
        }
        std::cout << ")";
    }

    else if (auto a = dynamic_cast<const ArrayAccess*>(expr))
    {
        std::cout << "(index ";
        print_ast(a->target.get());
        std::cout << " ";
        print_ast(a->index.get());
        std::cout << ")";
    }
}