#pragma once
#include <string>
#include <string_view>
#include <iostream>
#include <unordered_map>

enum class TokenType
{
    // Keywords
    KEYWORD_INT, KEYWORD_FLOAT, KEYWORD_CHAR, KEYWORD_STRING, KEYWORD_VOID,
    KEYWORD_LET, KEYWORD_IF, KEYWORD_ELSE, KEYWORD_FOR, KEYWORD_WHILE, KEYWORD_RETURN,
    KEYWORD_NULL, KEYWORD_TRUE, KEYWORD_FALSE, KEYWORD_STRUCT, KEYWORD_FN, KEYWORD_BOOL,

    // Literals
    IDENTIFIER, INT_LITERAL, FLOAT_LITERAL,
    CHAR_LITERAL, STRING_LITERAL,   

    // Single Character Operators & Punctuation
    PLUS, MINUS, STAR, SLASH, PERCENT,
    SEMICOLON, COLON, COMMA, DOT, BANG,

    // Multi Character Operators & Comparison
    EQUAL, EQUAL_EQUAL, BANG_EQUAL,
    LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
    INCREMENT, DECREMENT, POW,

    // Delimiters
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    LEFT_BRACE, RIGHT_BRACE,

    // System
    END_OF_FILE, UNKNOWN, TOKEN_ERROR, PRINT
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};


inline std::string_view token_type_to_string(TokenType type)
{
    static const std::unordered_map<TokenType, std::string_view> type_names = {
        // Keywords
        {TokenType::KEYWORD_INT, "KEYWORD_INT"},
        {TokenType::KEYWORD_FLOAT, "KEYWORD_FLOAT"},
        {TokenType::KEYWORD_CHAR, "KEYWORD_CHAR"},
        {TokenType::KEYWORD_STRING, "KEYWORD_STRING"},
        {TokenType::KEYWORD_VOID, "KEYWORD_VOID"},
        {TokenType::KEYWORD_BOOL, "KEYWORD_BOOL"},
        {TokenType::KEYWORD_LET, "KEYWORD_LET"},
        {TokenType::KEYWORD_IF, "KEYWORD_IF"},
        {TokenType::KEYWORD_ELSE, "KEYWORD_ELSE"},
        {TokenType::KEYWORD_FOR, "KEYWORD_FOR"},
        {TokenType::KEYWORD_WHILE, "KEYWORD_WHILE"},
        {TokenType::KEYWORD_RETURN, "KEYWORD_RETURN"},
        {TokenType::KEYWORD_NULL, "KEYWORD_NULL"},
        {TokenType::KEYWORD_TRUE, "KEYWORD_TRUE"},
        {TokenType::KEYWORD_FALSE, "KEYWORD_FALSE"},
        {TokenType::KEYWORD_STRUCT, "KEYWORD_STRUCT"},
        {TokenType::KEYWORD_FN, "KEYWORD_FN"},

        // Literals
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::INT_LITERAL, "INT_LITERAL"},
        {TokenType::FLOAT_LITERAL, "FLOAT_LITERAL"},
        {TokenType::CHAR_LITERAL, "CHAR_LITERAL"},
        {TokenType::STRING_LITERAL, "STRING_LITERAL"},

        // Single Character Operators & Punctuation
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::STAR, "STAR"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::PERCENT, "PERCENT"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COLON, "COLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::BANG, "BANG"},

        // Multi Character Operators & Comparison
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::BANG_EQUAL, "BANG_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::INCREMENT, "INCREMENT"},
        {TokenType::DECREMENT, "DECREMENT"},
        {TokenType::POW, "POW"},

        // Delimiters
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},

        // System
        {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::UNKNOWN, "UNKNOWN"},
        {TokenType::TOKEN_ERROR, "TOKEN_ERROR"},
        {TokenType::PRINT, "PRINT"}
    };

    auto it = type_names.find(type);
    if (it != type_names.end()) { return it->second; };

    return "UNHANDLED_TOKEN_TYPE";
}

// thank you gemini for this operator overload!
inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    std::string printable = "";
    for (char c : token.lexeme)
    {
        switch (c)
        {
            case '\n': printable += "\\n"; break;
            case '\t': printable += "\\t"; break;
            case '\r': printable += "\\r"; break;
            case '\\': printable += "\\\\"; break;
            case '"':  printable += "\\\""; break;
            default:   printable += c; break;
        }
    }

    os << "[TOKEN] " << token_type_to_string(token.type)
       << ", \"" << printable << "\""
       << ", L" << token.line 
       << " C" << token.column << ")";
    return os;
}
