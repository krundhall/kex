#pragma once

#include "AST.h"
#include "Token.h"
#include <memory>
#include <string>
#include <unordered_map>

class Environment : public std::enable_shared_from_this<Environment>
{
private:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, LiteralValue> values;

public:
    // Global scope constructor
    Environment();

    // Nested scope constructor (takes parent scope)
    explicit Environment(std::shared_ptr<Environment> enclosing);

    // Defines a variable in the CURRENT scope
    void define(const std::string& name, const LiteralValue& value);

    // Assigns a value to an EXISTING variable (searches current and enclosing scopes)
    void assign(const Token& name, const LiteralValue& value);

    // Look up an existing variable's value (searches current and enclosing scopes)
    LiteralValue get(const Token& name) const;
};