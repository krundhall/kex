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

Token Lexer::scan_number(char first_digit, int start_column)
{
    std::string lexeme(1, first_digit);

    bool seen_dot = false;
    while (!is_at_end())
    {
        if (isdigit(peek()))
        {
            lexeme += advance();
        }
        else if (peek() == '.' && seen_dot == false)
        {
            lexeme += advance();
            seen_dot = true;
        }
        else
        {
            break;
        }
    }

    if (seen_dot)
        return Token{TokenType::FLOAT_LITERAL, lexeme, line, start_column};
    else
        return Token{TokenType::INT_LITERAL, lexeme, line, start_column};
}

Token Lexer::scan_identifier(char first_char, int start_column)
{
    std::string lexeme(1, first_char);

    while (!is_at_end() && (std::isalnum(peek()) || peek() == '_'))
    {
        lexeme += advance();
    }

    TokenType type = TokenType::IDENTIFIER;
    auto it = keywords.find(lexeme);
    if (it != keywords.end())
        type = it->second;

    return Token{type, lexeme, line, start_column};
}

Token Lexer::scan_char(char first_char, int start_column)
{
    // TODO: Implement escape-sequence support
    std::string lexeme(1, first_char);

    if (peek() == '\'')
    {
        lexeme += advance();
        return Token{TokenType::UNKNOWN, lexeme, line, start_column};
    }
    if (!is_at_end())
    {
        lexeme += advance();
    }
    if (peek() == '\'')
    {
        lexeme += advance();
        return Token{TokenType::CHAR_LITERAL, lexeme, line, start_column};
    }

    return Token{TokenType::UNKNOWN, lexeme, line, start_column};
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (!is_at_end())
    {
        skip_whitespace();
        if (is_at_end()) { break; }

        int start_column = column;
        char c = advance();

        switch (c)
        {
            case '+': tokens.push_back({TokenType::PLUS, "+", line, start_column}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", line, start_column}); break;
            case '=':
                if (peek() == '=')
                {
                    advance(); // Consume second '='
                    tokens.push_back({TokenType::EQUAL_EQUAL, "==", line, start_column});
                }
                else
                {
                    tokens.push_back({TokenType::EQUAL, "=", line, start_column});
                }
                break;
            case '\'': tokens.push_back(scan_char(c, start_column)); break;
            default:
                if (std::isdigit(c))
                {
                    tokens.push_back(scan_number(c, start_column));
                }
                else if (std::isalpha(c) || c == '_')
                {
                    tokens.push_back(scan_identifier(c, start_column));
                }
                else
                {
                    tokens.push_back({TokenType::UNKNOWN, std::string(1, c), line, start_column});
                }
                break;
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}
