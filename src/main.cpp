#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <variant>

#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

std::string readFile(std::string filepath);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <path_to_file>\n";
        exit(1);
    }
    std::string source = readFile(argv[1]);
    std::cout << source << '\n';
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    for (const auto &token : tokens)
    {
        std::cout << token << '\n';
    }

    std::cout << "\n===========\n";

    // Construct the AST for: -123 * (45.67)
    // 
    //            Binary (*)
    //           /          \
    //   Unary (-)          Grouping
    //      |                  |
    // Literal (123.0)     Literal (45.67)

    auto ast = std::make_unique<Binary>(
        // Left side: Unary '-' applied to 123.0
        std::make_unique<Unary>(
            Token{TokenType::MINUS, "-", 1},
            std::make_unique<Literal>(123.0)
        ),
        // Operator: '*'
        Token{TokenType::STAR, "*", 1},
        // Right side: Grouping containing 45.67
        std::make_unique<Grouping>(
            std::make_unique<Literal>(45.67)
        )
    );

    print_ast(ast.get());
    std::cout << '\n';


    return 0;
}

std::string readFile(std::string filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "[ERROR]: Could not open file " << filepath << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();

    return source;
}