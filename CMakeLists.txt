cmake_minimum_required(VERSION 3.24)
project(kex VERSION 1.0 LANGUAGES CXX)
add_executable(
    kex
    src/main.cpp
    src/lexer/Lexer.cpp
    src/parser/Parser.cpp
    src/parser/AST.cpp
)

target_include_directories(kex PRIVATE include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)