#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

inline static const std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::KEYWORD_LET},
    {"let", TokenType::KEYWORD_LET},
    {"if", TokenType::KEYWORD_LET},
    {"else", TokenType::KEYWORD_LET},
    {"while", TokenType::KEYWORD_LET},
    {"return", TokenType::KEYWORD_LET}
};

class Lexer
{
private:
    std::string source;
    size_t cursor = 0;
    int line = 1;
    int column = 1;

    bool is_at_end();
    char peek();
    char peek_next();
    char advance();

    bool is_whitespace();
    bool is_newline();
    void skip_whitespace();

    Token scan_number(char first_digit);
    Token scan_identifier(char first_char);

public:
    Lexer(std::string source);
    std::vector<Token> tokenize();
};