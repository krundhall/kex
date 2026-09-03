#include "Lexer.h"

Lexer::Lexer(std::string source)
: source(source)
{
}

bool Lexer::is_at_end()
{
    return this->cursor >= source.length();
}

char Lexer::peek()
{
    char c = is_at_end() ? '\0' : source[cursor];
    return c;
}

char Lexer::advance()
{
    char c = source[cursor++];
    column++;
    return c;
}

bool Lexer::is_whitespace()
{
    char c = peek();
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::is_newline()
{
    return peek() == '\n';
}

void Lexer::skip_whitespace()
{
    while (!is_at_end())
    {
        if (is_whitespace())
        {
            advance();
        }

        else if (is_newline())
        {
            line++;
            advance();
            column = 1;
        }
        else
        {
            break; //return to tokenize
        }
    }
}

Token Lexer::scan_number(char first_digit)
{
    std::string lexeme(1, first_digit);

    while (!is_at_end() && std::isdigit(peek()))
    {
        lexeme += advance();
    }

    return Token{TokenType::NUMBER_LITERAL, lexeme, line, column};
}

Token Lexer::scan_identifier(char first_char)
{
    std::string text(1, first_char);

    while (!is_at_end() && (std::isalnum(peek()) || peek() == '_'))
    {
        text += advance();
    }

    TokenType type = TokenType::IDENTIFIER;
    auto it = keywords.find(text);
    if (it != keywords.end())
        type = it->second;

    return Token{type, text, line, column};
}


std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (!is_at_end())
    {
        skip_whitespace();
        if (is_at_end()) { break; }

        char c = advance();

        switch (c)
        {
            case '+': tokens.push_back({TokenType::PLUS, "+", line, column}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", line, column}); break;
            case '=':
                if (peek() == '=')
                {
                    advance(); // Consume second '='
                    tokens.push_back({TokenType::EQUAL_EQUAL, "==", line, column});
                }
                else
                {
                    tokens.push_back({TokenType::EQUAL, "=", line, column});
                }
                break;

            default:
                if (std::isdigit(c))
                {
                    tokens.push_back(scan_number(c));
                }
                else if (std::isalpha(c) || c == '_')
                {
                    tokens.push_back(scan_identifier(c));
                }
                else
                {
                    tokens.push_back({TokenType::UNKNOWN, std::string(1, c), line, column});
                }
                break;
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}
