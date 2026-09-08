#include "Parser.h"

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

Token Parser::consume(TokenType type, const std::string& message)
{
    if (check(type)) return advance();

    // Throws the exception created by error()
    throw error(peek(), message);
}

// reports syntax error and returns parsererror exception object for consume() to throw
Parser::ParseError Parser::error(const Token& token, const std::string& message)
{
    std::cerr << "[line " << token.line << "] Error";

    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }

    std::cerr << ": " << message << "\n";

    return ParseError(message);
}

void Parser::synchronize()
{
    this->panic_mode = false;
    advance();

    while (!is_at_end())
    {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type)
        {
            case TokenType::KEYWORD_STRUCT:
            case TokenType::KEYWORD_FN:
            case TokenType::KEYWORD_LET:
            case TokenType::KEYWORD_FOR:
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_WHILE:
            case TokenType::KEYWORD_RETURN:
                return;
            default:
                break;
        }

        advance();
    }
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
    ExprPtr expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}))
    {
        Token op = previous();
        ExprPtr right = term();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

ExprPtr Parser::term()
{
    ExprPtr expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS}))
    {
        Token op = previous();
        ExprPtr right = factor();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

ExprPtr Parser::factor()
{
    ExprPtr expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR}))
    {
        Token op = previous();
        ExprPtr right = unary();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

ExprPtr Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
        Token op = previous();
        ExprPtr right = unary();
        return std::make_unique<Unary>(std::move(op), std::move(right));
    }

    return primary();
}

ExprPtr Parser::primary()
{
    if (match({TokenType::KEYWORD_FALSE})) return std::make_unique<Literal>(false);
    if (match({TokenType::KEYWORD_TRUE})) return std::make_unique<Literal>(true);
    if (match({TokenType::KEYWORD_NULL})) return std::make_unique<Literal>(std::monostate{});

    if (match({TokenType::INT_LITERAL, TokenType::FLOAT_LITERAL, 
               TokenType::CHAR_LITERAL, TokenType::STRING_LITERAL}))
    {
        return std::make_unique<Literal>(previous().lexeme);
    }

    if (match({TokenType::LEFT_PAREN}))
    {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw error(peek(), "Expect expression");
}