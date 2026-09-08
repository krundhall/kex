#include "Environment.h"
#include <stdexcept>

Environment::Environment()
    : enclosing(nullptr)
{
}

Environment::Environment(std::shared_ptr<Environment> enclosing)
    : enclosing(std::move(enclosing))
{
}

void Environment::define(const std::string& name, const LiteralValue& value)
{
    values[name] = value;
}

void Environment::assign(const Token& name, const LiteralValue& value)
{
    // 1. Check if variable exists in current scope
    auto it = values.find(name.lexeme);
    if (it != values.end())
    {
        it->second = value;
        return;
    }

    // 2. Delegate to parent scope if available
    if (enclosing != nullptr)
    {
        enclosing->assign(name, value);
        return;
    }

    // 3. Fail if variable was never defined
    throw std::runtime_error("Undefined variable '" + name.lexeme + "' at line " + std::to_string(name.line) + ".");
}

LiteralValue Environment::get(const Token& name) const
{
    // 1. Check current scope
    auto it = values.find(name.lexeme);
    if (it != values.end())
    {
        return it->second;
    }

    // 2. Look up parent scope chain
    if (enclosing != nullptr)
    {
        return enclosing->get(name);
    }

    // 3. Fail if variable does not exist
    throw std::runtime_error("Undefined variable '" + name.lexeme + "' at line " + std::to_string(name.line) + ".");
}