#include <fmt/core.h>
#include <iostream>
#include "parser.hpp"

int main()
{
    auto parser = Parser(Literal("Hello"), Star(Choice(Literal("a"), Literal("o"), Literal("e"))));
    std::string input;
    std::cin >> input;
    auto parsed = parser.parse(input);
    if (parsed.has_value())
    {
        fmt::print("Left after parsing {}\n", *parsed);
    }
    else
    {
        fmt::print("Failed parsing\n");
    }
    return 0;
}