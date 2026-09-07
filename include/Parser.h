#pragma once
#include "Token.h"
#include <vector>
#include "AST.h"
#include <initializer_list>

class Parser
{
private:
    const std::vector<Token>& tokens;
    int current = 0;

    bool is_at_end();
    const Token& peek();
    const Token& previous();
    const Token& advance();
    bool check(TokenType type);
    bool match(std::initializer_list<TokenType> types);

public:
    Parser(const std::vector<Token>& tokens)
        : tokens(tokens) {}



    ExprPtr expression();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();
};