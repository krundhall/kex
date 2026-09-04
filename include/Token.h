#pragma once
#include <string>
#include <iostream>

enum class TokenType
{
    // Keywords
    KEYWORD_INT,
    KEYWORD_FLOAT,
    KEYWORD_CHAR,
    KEYWORD_VOID,
    KEYWORD_LET,        // let
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,
    KEYWORD_RETURN,

    // Literals & Names
    IDENTIFIER,           // "total"
    INT_LITERAL,         // "10", "20"
    FLOAT_LITERAL,      // "3.5"
    CHAR_LITERAL,      // 'x'

    // Operators & Symbols
    EQUAL,
    EQUAL_EQUAL,
    PLUS,
    SEMICOLON,

    // System
    END_OF_FILE,
    UNKNOWN
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};


constexpr std::string_view token_type_to_string(TokenType type)
{
    switch (type)
    {
        case TokenType::KEYWORD_INT:    return "KEYWORD_INT";
        case TokenType::KEYWORD_FLOAT:  return "KEYWORD_FLOAT";
        case TokenType::KEYWORD_CHAR:   return "KEYWORD_CHAR";
        case TokenType::KEYWORD_VOID:   return "KEYWORD_VOID";
        case TokenType::KEYWORD_LET:    return "KEYWORD_LET";
        case TokenType::KEYWORD_IF:     return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE:   return "KEYWORD_ELSE";
        case TokenType::KEYWORD_WHILE:  return "KEYWORD_WHILE";
        case TokenType::KEYWORD_RETURN: return "KEYWORD_RETURN";

        case TokenType::IDENTIFIER:     return "IDENTIFIER";
        case TokenType::INT_LITERAL:    return "INT_LITERAL";
        case TokenType::FLOAT_LITERAL:  return "FLOAT_LITERAL";
        case TokenType::CHAR_LITERAL:   return "CHAR_LITERAL";
        
        case TokenType::PLUS:           return "PLUS";
        case TokenType::SEMICOLON:      return "SEMICOLON";
        case TokenType::EQUAL:          return "EQUAL";
        case TokenType::EQUAL_EQUAL:    return "EQUAL_EQUAL";

        case TokenType::UNKNOWN:        return "UNKNOWN";
        case TokenType::END_OF_FILE:    return "END_OF_FILE";
    }
    return "UNHANDLED_TOKEN_TYPE";
}

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    os << "[TOKEN] " << token_type_to_string(token.type)
       << ", \"" << token.lexeme << "\""
       << ", L" << token.line 
       << " C" << token.column << ")";
    return os;
}