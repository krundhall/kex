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
    Parser parser(tokens);
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