#include "parser.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <iostream>

template<typename T>
struct fmt::formatter<std::optional<T>> {
  template<typename FormatContext>
  auto parse(FormatContext &ctx) {
    return ctx.begin();
  }
  template<typename FormatContext>
  auto format(std::optional<T> const &opt, FormatContext &ctx) {
    if(opt.has_value()) {
      //   return formatter<T>::format(*opt, ctx);
      return fmt::format_to(ctx.out(), "{}", *opt);
    }
    return fmt::format_to(ctx.out(), "(null)");
  }
};

int main() {
  auto        parser = Parser(Literal("h"), Plus(Literal("o")));
  std::string input;
  std::cin >> input;
  auto parsed = parser.parse(input);
  if(parsed) {
    // auto x = fmt::join(parsed->result, ", ");
    fmt::print("Parsed:    {}\nRemainder: {}\n", parsed->result, parsed->remainder);
  } else {
    fmt::print("Failed parsing\n");
  }
  return 0;
}