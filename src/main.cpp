#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

static std::string read_file(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << path << "\n";
        exit(74);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static void run(const std::string& source, Interpreter& interpreter)
{
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto statements = parser.parse();

    if (parser.had_error) return;

    interpreter.interpret(statements);
}

static void run_file(const std::string& path)
{
    std::string source = read_file(path);
    Interpreter interpreter;
    run(source, interpreter);
}

static void run_prompt()
{
    Interpreter interpreter;
    std::string line;

    std::cout << "Kex REPL (type 'exit' or press Ctrl+C to quit)\n> ";

    while (std::getline(std::cin, line))
    {
        if (line == "exit") break;
        if (line.empty()) continue;

        run(line, interpreter);
        std::cout << "> ";
    }
}

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        std::cout << "Usage: kex [script]\n";
        return 64;
    }
    else if (argc == 2)
    {
        run_file(argv[1]);
    }
    else
    {
        run_prompt();
    }

    return 0;
}