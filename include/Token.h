#pragma once
#include <string>

enum class TokenType
{
    // Keywords
    KEYWORD_LET,        // let

    // Literals & Names
    IDENTIFIER,          // "total"
    NUMBER_LITERAL,      // "10", "20"

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

#include <iostream>

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    // Adjust field names according to your Token struct definition
    os << "Token(Type: " << static_cast<int>(token.type) 
       << ", Lexeme: \"" << token.lexeme << "\""
       << ", Line: " << token.line 
       << ", Col: " << token.column << ")";
    return os;
}