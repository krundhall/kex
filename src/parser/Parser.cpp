#include "Parser.h"
#include "Token.h"
#include <cassert>

bool Parser::is_at_end()
{
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::peek()
{
    return this->tokens[current];
}

const Token& Parser::previous()
{
    if (current <= 0)
        throw std::runtime_error("[PARSER ERROR] Attempted to previous() at start of stream");
    return this->tokens[current - 1];
}

const Token& Parser::advance()
{
    if (!is_at_end())
        current++;

    return previous();
}

bool Parser::check(TokenType type)
{
    if (is_at_end())
        return false;

    return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types)
{
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

ExprPtr Parser::expression()
{
    return equality();
}

ExprPtr Parser::equality()
{
    ExprPtr expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        Token op = previous();
        ExprPtr right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

ExprPtr Parser::comparison()
{
    return ExprPtr{};
}

ExprPtr Parser::term()
{
    return ExprPtr{};
}

ExprPtr Parser::factor()
{
    return ExprPtr{};
}

ExprPtr Parser::unary()
{
    return ExprPtr{};
}

ExprPtr Parser::primary()
{
    return ExprPtr{};
}