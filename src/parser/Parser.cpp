#include "Parser.h"

bool Parser::is_at_end()
{
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::peek()
{
    return this->tokens[current];
}

const Token& Parser::previous()
{
    if (current <= 0)
        throw std::runtime_error("[PARSER ERROR] Attempted to previous() at start of stream");
    return this->tokens[current - 1];
}

const Token& Parser::advance()
{
    if (!is_at_end())
        current++;

    return previous();
}

bool Parser::check(TokenType type)
{
    if (is_at_end())
        return false;

    return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types)
{
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

Token Parser::consume(TokenType type, const std::string& message)
{
    if (check(type)) return advance();

    // Throws the exception created by error()
    throw error(peek(), message);
}

// reports syntax error and returns parsererror exception object for consume() to throw
Parser::ParseError Parser::error(const Token& token, const std::string& message)
{
    this->panic_mode = true;

    std::cerr << "[line " << token.line << "] Error";

    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }

    std::cerr << ": " << message << "\n";

    return ParseError(message);
}

void Parser::synchronize()
{
    this->panic_mode = false;
    advance();

    while (!is_at_end())
    {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type)
        {
            case TokenType::KEYWORD_STRUCT:
            case TokenType::KEYWORD_FN:
            case TokenType::KEYWORD_LET:
            case TokenType::KEYWORD_FOR:
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_WHILE:
            case TokenType::KEYWORD_RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}

std::vector<StmtPtr> Parser::parse()
{
    std::vector<StmtPtr> statements;
    while (!is_at_end())
    {
        StmtPtr stmt = declaration();
        if (stmt)
        {
            statements.push_back(std::move(stmt));
        }
    }
    return statements;
}

ExprPtr Parser::expression()
{
    return assignment();
}

ExprPtr Parser::assignment()
{
    // Parse left side (evaluates down through equality, comparison, term, etc.)
    ExprPtr expr = equality();

    if (match({TokenType::EQUAL}))
    {
        Token equals = previous();
        ExprPtr value = assignment(); // Right-recursive for assignment chaining (a = b = 5)

        // Ensure the left-hand target is a valid lvalue (Variable node)
        if (auto v = dynamic_cast<Variable*>(expr.get()))
        {
            Token name = v->name;
            return std::make_unique<Assign>(name, std::move(value));
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}
ExprPtr Parser::equality()
{
    ExprPtr expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        Token op = previous();
        ExprPtr right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
}

ExprPtr Parser::comparison()
{
    ExprPtr expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}))
    {
        Token op = previous();
        ExprPtr right = term();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

ExprPtr Parser::term()
{
    ExprPtr expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS}))
    {
        Token op = previous();
        ExprPtr right = factor();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

ExprPtr Parser::factor()
{
    ExprPtr expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR}))
    {
        Token op = previous();
        ExprPtr right = unary();
        expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

ExprPtr Parser::unary()
{
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
        Token op = previous();
        ExprPtr right = unary();
        return std::make_unique<Unary>(std::move(op), std::move(right));
    }

    return primary();
}

ExprPtr Parser::primary()
{
    if (match({TokenType::KEYWORD_FALSE})) return std::make_unique<Literal>(false);
    if (match({TokenType::KEYWORD_TRUE})) return std::make_unique<Literal>(true);
    if (match({TokenType::KEYWORD_NULL})) return std::make_unique<Literal>(std::monostate{});

    // Variable lookup 
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<Variable>(previous());
    }

    // INT_LITERAL
    if (match({TokenType::INT_LITERAL})) {
        return std::make_unique<Literal>(std::stoi(previous().lexeme));
    }

    // FLOAT_LITERAL
    if (match({TokenType::FLOAT_LITERAL})) {
        return std::make_unique<Literal>(std::stod(previous().lexeme));
    }

    // STRING_LITERAL
    if (match({TokenType::STRING_LITERAL})) {
        return std::make_unique<Literal>(previous().lexeme);
    }

    if (match({TokenType::LEFT_PAREN}))
    {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

bool Parser::is_type()
{
    // Native types or 'auto'
    if (check(TokenType::KEYWORD_INT) || 
        check(TokenType::KEYWORD_FLOAT) || 
        check(TokenType::KEYWORD_CHAR) || 
        check(TokenType::KEYWORD_BOOL) || 
        check(TokenType::KEYWORD_STRING) || 
        check(TokenType::KEYWORD_LET)) 
    {
        return true;
    }

    // Custom struct/class types (e.g., "Vector3 v;")
    // Requires checking if current and next tokens are both IDENTIFIERs
    if (check(TokenType::IDENTIFIER) && check_next(TokenType::IDENTIFIER)) 
    {
        return true;
    }

    return false;
}

bool Parser::check_next(TokenType type)
{
    if (is_at_end() || current + 1 >= static_cast<int>(tokens.size())) return false;
    if (tokens[current + 1].type == TokenType::END_OF_FILE) return false;
    return tokens[current + 1].type == type;
}

StmtPtr Parser::declaration()
{
    try
    {
        if (is_type())
            return var_declaration();

        return statement();
    }
    catch(const ParseError& error)
    {
        synchronize();
        return nullptr;
    }
    
}

StmtPtr Parser::var_declaration()
{
    // 1. Consume type keyword or 'let'
    Token type_token = advance();

    // 2. Consume variable name
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    // 3. Parse optional initializer (= expression)
    ExprPtr initializer = nullptr;
    if (match({TokenType::EQUAL})) 
    {
        initializer = expression();
    } 
    else if (type_token.type == TokenType::KEYWORD_LET) 
    {
        // 'let' variables MUST have an initializer for type inference
        throw error(name, "Variables declared with 'let' must be initialized.");
    }

    // 4. Consume trailing semicolon
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

    return std::make_unique<VarStmt>(type_token, name, std::move(initializer));
}

StmtPtr Parser::statement()
{
    if (match({TokenType::PRINT}))
    {
        ExprPtr value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return std::make_unique<PrintStmt>(std::move(value));
    }

    if (match({TokenType::LEFT_BRACE}))
    {
        return std::make_unique<BlockStmt>(block());
    }

    return expr_statement();
}

StmtPtr Parser::expr_statement()
{
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::vector<StmtPtr> Parser::block()
{
    std::vector<StmtPtr> statements;

    while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) 
    {
        StmtPtr stmt = declaration();
        if (stmt) 
        {
            statements.push_back(std::move(stmt));
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}
