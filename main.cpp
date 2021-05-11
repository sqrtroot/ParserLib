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

template<typename... T>
struct fmt::formatter<std::variant<T...>> {
  template<typename FormatContext>
  auto parse(FormatContext &ctx) {
    return ctx.begin();
  }
  template<typename FormatContext>
  auto format(std::variant<T...> const &v, FormatContext &ctx) {
    return std::visit(
      [&](auto &&arg) {
        return fmt::format_to(ctx.out(), "{}::({})", arg, typeid(arg).name());
      },
      v);
  }
};

int main() {
  // auto parser = Parser(Transform([](auto) { return 0; }, Literal("0")),
  //                      Plus(Literal("o")));
  // auto parser = Transform([](auto){return 0;}, Literal("0"));
  auto        parser = Choice(Transform([](auto) { return 0; }, Literal("i")),
                       Transform([](auto) { return true; }, Literal("b")));
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