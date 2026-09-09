#include "Interpreter.h"
#include <iostream>
#include <stdexcept>

LiteralValue Interpreter::evaluate(Expr& expr)
{
    expr.accept(*this);
    return last_value;
}

void Interpreter::execute(Stmt& stmt)
{
    stmt.accept(*this);
}

void Interpreter::interpret(const std::vector<StmtPtr>& statements)
{
    try
    {
        for (const auto& stmt : statements)
        {
            if (stmt)
            {
                execute(*stmt);
            }
        }
    }
    catch (const std::runtime_error& error)
    {
        std::cerr << "[Runtime Error] " << error.what() << "\n";
    }
}

// ============================================================================
// Statement Visitors
// ============================================================================

void Interpreter::visitExpressionStmt(ExpressionStmt& stmt)
{
    if (stmt.expression)
    {
        evaluate(*stmt.expression);
    }
}

void Interpreter::visitVarStmt(VarStmt& stmt)
{
    LiteralValue value = std::monostate{};

    if (stmt.initializer != nullptr)
    {
        value = evaluate(*stmt.initializer);
    }

    environment->define(stmt.name.lexeme, value);
}

void Interpreter::visitBlockStmt(BlockStmt& stmt)
{
    std::shared_ptr<Environment> previous = this->environment;
    this->environment = std::make_shared<Environment>(previous);

    try
    {
        for (const auto& statement : stmt.statements)
        {
            if (statement)
            {
                execute(*statement);
            }
        }
    }
    catch (...)
    {
        this->environment = previous;
        throw;
    }

    this->environment = previous;
}

void Interpreter::visitPrintStmt(PrintStmt& stmt)
{
    LiteralValue value = evaluate(*stmt.expression);

    if (std::holds_alternative<int>(value))
        std::cout << std::get<int>(value) << std::endl;
    else if (std::holds_alternative<double>(value))
        std::cout << std::get<double>(value) << std::endl;
    else if (std::holds_alternative<std::string>(value))
        std::cout << std::get<std::string>(value) << std::endl;
    else if (std::holds_alternative<bool>(value))
        std::cout << (std::get<bool>(value) ? "true" : "false") << std::endl;
    else if (std::holds_alternative<std::monostate>(value))
        std::cout << "null" << std::endl;
}

void Interpreter::visitIfStmt(IfStmt& stmt)
{
    LiteralValue condValue = evaluate(*stmt.condition);

    // Truthiness helper
    auto is_truthy = [](const LiteralValue& val) -> bool {
        if (std::holds_alternative<std::monostate>(val)) return false;
        if (std::holds_alternative<bool>(val)) return std::get<bool>(val);
        return true; // Any non-null, non-false value is truthy
    };

    if (is_truthy(condValue))
    {
        execute(*stmt.thenBranch);
    }
    else if (stmt.elseBranch != nullptr)
    {
        execute(*stmt.elseBranch);
    }
}

// ============================================================================
// Expression Visitors
// ============================================================================

void Interpreter::visitLiteral(Literal& expr)
{
    last_value = expr.value;
}

void Interpreter::visitVariable(Variable& expr)
{
    last_value = environment->get(expr.name);
}

void Interpreter::visitAssign(Assign& expr)
{
    LiteralValue value = evaluate(*expr.value);
    environment->assign(expr.name, value);
    last_value = value;
}

void Interpreter::visitGrouping(Grouping& expr)
{
    if (expr.expression)
    {
        last_value = evaluate(*expr.expression);
    }
}

void Interpreter::visitBinary(Binary& expr)
{
    LiteralValue left = evaluate(*expr.left);
    LiteralValue right = evaluate(*expr.right);

    // Numeric arithmetic operations (int and double)
    if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
    {
        int l = std::get<int>(left);
        int r = std::get<int>(right);

        if (expr.op.type == TokenType::PLUS) last_value = l + r;
        else if (expr.op.type == TokenType::MINUS) last_value = l - r;
        else if (expr.op.type == TokenType::STAR) last_value = l * r;
        else if (expr.op.type == TokenType::SLASH)
        {
            if (r == 0) throw std::runtime_error("Division by zero.");
            last_value = l / r;
        }
        return;
    }

    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
    {
        double l = std::get<double>(left);
        double r = std::get<double>(right);

        if (expr.op.type == TokenType::PLUS) last_value = l + r;
        else if (expr.op.type == TokenType::MINUS) last_value = l - r;
        else if (expr.op.type == TokenType::STAR) last_value = l * r;
        else if (expr.op.type == TokenType::SLASH)
        {
            if (r == 0.0) throw std::runtime_error("Division by zero.");
            last_value = l / r;
        }
        return;
    }

    throw std::runtime_error("Operands must be two numbers of matching type for binary operation '" + expr.op.lexeme + "'.");
}

void Interpreter::visitUnary(Unary& expr)
{
    LiteralValue right = evaluate(*expr.right);

    if (expr.op.type == TokenType::MINUS)
    {
        if (std::holds_alternative<int>(right))
        {
            last_value = -std::get<int>(right);
            return;
        }
        if (std::holds_alternative<double>(right))
        {
            last_value = -std::get<double>(right);
            return;
        }
        throw std::runtime_error("Operand must be a number for unary minus.");
    }

    if (expr.op.type == TokenType::BANG)
    {
        if (std::holds_alternative<bool>(right))
        {
            last_value = !std::get<bool>(right);
            return;
        }
        if (std::holds_alternative<std::monostate>(right))
        {
            last_value = true;
            return;
        }
        last_value = false;
        return;
    }
}

void Interpreter::visitUpdateExpr(UpdateExpr& expr)
{
    // Update operations (e.g. ++x or x++) require target variable lookup
    auto var_expr = dynamic_cast<Variable*>(expr.operand.get());
    if (!var_expr)
    {
        throw std::runtime_error("Invalid target for update operator '" + expr.op.lexeme + "'.");
    }

    LiteralValue current = environment->get(var_expr->name);
    if (!std::holds_alternative<int>(current))
    {
        throw std::runtime_error("Increment/decrement operand must be an integer.");
    }

    int val = std::get<int>(current);
    int updated = (expr.op.type == TokenType::INCREMENT) ? val + 1 : val - 1;

    environment->assign(var_expr->name, updated);
    last_value = expr.postfix ? val : updated;
}

void Interpreter::visitArrayAccess(ArrayAccess& expr)
{
    // Placeholder stub until native array / sequence types are integrated
    throw std::runtime_error("Array index access is not yet supported in interpreter.");
}