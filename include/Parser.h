#pragma once
#include "AST.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <initializer_list>

class Parser
{
public:
    const std::vector<Token>& tokens;
    int current = 0;

    bool panic_mode = false;

    struct ParseError : public std::runtime_error
    {
        explicit ParseError(const std::string& message)
            : std::runtime_error(message) {}
    };

    bool is_at_end();
    const Token& peek();
    const Token& previous();
    const Token& advance();
    bool check(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);

    void synchronize();
    
    Parser(const std::vector<Token>& tokens)
        : tokens(tokens) {}

    ExprPtr parse();
    bool had_error = false;

    ExprPtr expression();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();
};