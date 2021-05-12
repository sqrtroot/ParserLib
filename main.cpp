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
  // auto        parser = Choice(Transform([](auto) { return false; }, Literal("f")),
  //                      Transform([](auto) { return true; }, Literal("t")));
  auto parser = Parser(Literal("h"), Choice(Literal("o"), Literal("a")), Literal("i"));

  // TODO: This is broken h doesn't appear in output
  // auto parser =
  //   Parser(Literal("h"),
  //          Transform([](auto e) { return e == "o"; }, Choice(Literal("o"), Literal("a"))),
  //          Literal("i"));
  std::string input;
  std::cin >> input;
  auto parsed = parser.parse(input);
  if(parsed) {
    fmt::print("Parsed:    {}\nRemainder: {}\n", parsed->result, parsed->remainder);
  } else {
    fmt::print("Failed parsing\n");
  }
  return 0;
}