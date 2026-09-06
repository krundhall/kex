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
    std::string lexeme = "";

    if (peek() == '\'') // empty char literal
    {
        advance();
        std::cerr << "[Lexer Error ] Line " << line << ", Column " << start_column
                  << ": Empty character literal.\n";
        return Token{TokenType::TOKEN_ERROR, "''", line, start_column};
    }
    /* grabbed from some stack overflow comment */
    if (!is_at_end())
    {
        if (peek() == '\\')
        {
            advance(); // Consume '\'
            if (!is_at_end())
            {
                char escaped = advance();
                switch (escaped)
                {
                    case 'n':  lexeme += '\n'; break;
                    case 't':  lexeme += '\t'; break;
                    case 'r':  lexeme += '\r'; break;
                    case '\\': lexeme += '\\'; break;
                    case '\'': lexeme += '\''; break;
                    case '"':  lexeme += '"';  break;
                    default:   lexeme += escaped; break;
                }
            }
        }
        else
        {
            lexeme += advance();
        }
    }

    if (peek() == '\'')
    {
        advance(); // Consume closing '\''
        return Token{TokenType::CHAR_LITERAL, lexeme, line, start_column};
    }

    std::cerr << "[Lexer Error ] Line " << line << ", Column " << start_column
              << ": Unterminated character literal.\n";
    return Token{TokenType::UNKNOWN, lexeme, line, start_column};
}

Token Lexer::scan_string(int start_column)
{
    std::string lexeme = "";

    while (peek() != '"' && !is_at_end())
    {
        if (peek() == '\\')
        {
            advance(); // Consume '\'
            if (is_at_end()) break;

            char escaped = advance();
            switch (escaped)
            {
                case 'n':  lexeme += '\n'; break;
                case 't':  lexeme += '\t'; break;
                case 'r':  lexeme += '\r'; break;
                case '\\': lexeme += '\\'; break;
                case '"':  lexeme += '"';  break;
                case '\'': lexeme += '\''; break;
                default:   lexeme += escaped; break;
            }
        }
        else if (is_newline())
        {
            // Do NOT consume unescaped newlines inside strings if strings cannot span multiple lines,
            // OR advance past the newline so the string loop can terminate/continue properly.
            std::cerr << "[Lexer Error] Line " << line << ", Column " << start_column 
                      << ": Unterminated string literal (found newline).\n";
            return {TokenType::TOKEN_ERROR, lexeme, line, start_column};
        }
        else
        {
            lexeme += advance();
        }
    }

    if (is_at_end())
    {
        std::cerr << "[Lexer Error] Line " << line << ", Column " << start_column 
                  << ": Unterminated string literal.\n";
        return {TokenType::TOKEN_ERROR, lexeme, line, start_column};
    }

    advance(); // Consume closing quote '"'
    return {TokenType::STRING_LITERAL, lexeme, line, start_column};
}

bool Lexer::match(char expected)
{
    if (is_at_end() || source[cursor] != expected)
        return false;

    cursor++;
    column++;
    return true;
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
            // Punctuation & Delimiters
            case '(': tokens.push_back({TokenType::LEFT_PAREN, "(", line, start_column}); break;
            case ')': tokens.push_back({TokenType::RIGHT_PAREN, ")", line, start_column}); break;
            case '[': tokens.push_back({TokenType::LEFT_BRACKET, "[", line, start_column}); break;
            case ']': tokens.push_back({TokenType::RIGHT_BRACKET, "]", line, start_column}); break;
            case '{': tokens.push_back({TokenType::LEFT_BRACE, "{", line, start_column}); break;
            case '}': tokens.push_back({TokenType::RIGHT_BRACE, "}", line, start_column}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", line, start_column}); break;
            case ':': tokens.push_back({TokenType::COLON, ":", line, start_column}); break;
            case ',': tokens.push_back({TokenType::COMMA, ",", line, start_column}); break;
            case '.': tokens.push_back({TokenType::DOT, ".", line, start_column}); break;
            case '"': tokens.push_back(scan_string(start_column)); break;

            // Operators
            case '+':
                if (match('+'))
                    tokens.push_back({TokenType::INCREMENT, "++", line, start_column});
                else
                    tokens.push_back({TokenType::PLUS, "+", line, start_column});
                break;
            case '-':
                if (match('-'))
                    tokens.push_back({TokenType::DECREMENT, "--", line, start_column});
                else
                    tokens.push_back({TokenType::MINUS, "-", line, start_column});
                break;
            case '*':
                if (match('*'))
                    tokens.push_back({TokenType::POW, "**", line, start_column});
                else
                    tokens.push_back({TokenType::STAR, "*", line, start_column});
                break;
            case '/':
                if (match('/'))
                {
                    while (peek() != '\n' && !is_at_end())
                    {
                        advance();
                    }
                }
                else if (match('*'))
                {
                    bool closed = false;
                    while (!is_at_end())
                    {
                        if (peek() == '*')
                        {
                            advance();
                            if (match('/')) 
                            {
                                closed = true;
                                break;
                            }
                            continue;
                        }
                    
                        if (is_newline())
                        {
                            line++;
                            advance();
                            column = 1;
                        }
                        else
                        {
                            advance();
                        }
                    }
                
                    if (!closed)
                    {
                        std::cerr << "[Lexer Error] Line " << line << ", Column " << start_column 
                                  << ": Unterminated multi-line comment.\n";
                        tokens.push_back({TokenType::TOKEN_ERROR, "/*", line, start_column});
                    }
                }
                else
                {
                    tokens.push_back({TokenType::SLASH, "/", line, start_column});
                }
                break;
            case '%': tokens.push_back({TokenType::PERCENT, "%", line, start_column}); break;

            case '=':
                if (match('='))
                    tokens.push_back({TokenType::EQUAL_EQUAL, "==", line, start_column});
                else
                    tokens.push_back({TokenType::EQUAL, "=", line, start_column});
                break;
            case '!':
                if (match('='))
                    tokens.push_back({TokenType::BANG_EQUAL, "!=", line, start_column});
                else
                   tokens.push_back({TokenType::BANG, "!", line, start_column});
                break;
            case '<':
                if (match('='))
                    tokens.push_back({TokenType::LESS_EQUAL, "<=", line, start_column});
                else
                    tokens.push_back({TokenType::LESS, "<", line, start_column});
                break;
            case '>':
                if (match('='))
                    tokens.push_back({TokenType::GREATER_EQUAL, ">=", line, start_column});
                else
                    tokens.push_back({TokenType::GREATER, ">", line, start_column});
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
                    std::cerr << "[Lexer Error] Line " << line << ", Column " << start_column 
                              << ": Unexpected character '" << c << "'.\n";
                    tokens.push_back({TokenType::TOKEN_ERROR, std::string(1, c), line, start_column});
                }
                break;
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    return tokens;
}
