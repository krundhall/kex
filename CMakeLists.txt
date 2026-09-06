cmake_minimum_required(VERSION 3.24)
project(kex VERSION 1.0 LANGUAGES CXX)
add_executable(
    kex
    src/main.cpp
    src/lexer/Lexer.cpp
    src/parser/Parser.cpp
)

target_include_directories(kex PRIVATE include)