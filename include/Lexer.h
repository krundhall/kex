#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

inline static const std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::KEYWORD_INT},
    {"float", TokenType::KEYWORD_FLOAT},
    {"char", TokenType::KEYWORD_CHAR},
    {"string", TokenType::KEYWORD_STRING},
    {"void", TokenType::KEYWORD_VOID},
    {"let", TokenType::KEYWORD_LET},
    {"if", TokenType::KEYWORD_IF},
    {"else", TokenType::KEYWORD_ELSE},
    {"while", TokenType::KEYWORD_WHILE},
    {"return", TokenType::KEYWORD_RETURN}
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
    char advance();

    bool is_whitespace();
    bool is_newline();
    void skip_whitespace();

    Token scan_number(char first_digit, int start_column);
    Token scan_identifier(char first_char, int start_column);
    Token scan_char(char first_char, int start_column);
    Token scan_string(int start_column);

    bool match(char expected);

public:
    Lexer(std::string source);
    std::vector<Token> tokenize();
};