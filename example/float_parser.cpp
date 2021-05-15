#include <charconv>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iostream>
#include <optional>
#include <parser.hpp>
#include <string>
#include <variant>
inline double move_to_decimal(int i, int l) {
  return ((double) i) / (pow(10, l));
}

int main() {
  const auto digit = Predicate([](char i) { return std::isdigit(i); });
  // digit = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
  // number = digit+
  const auto number = Transform(
    [](auto i) {
      int result;
      std::from_chars(i.begin(), i.end(), result);
      return result;
    },
    Plus(digit));
  const auto decimal = Transform(
    [](auto i) {
      int result;
      std::from_chars(i.begin(), i.end(), result);
      return move_to_decimal(result, i.length());
    },
    Plus(digit));

  // point = "."
  const auto point = Literal(".");

  // frac = number point number? | point number
  const auto frac = Choice(
    Transform([](auto i) { return std::get<0>(i) + std::get<2>(i).value_or(0.); },
              Parser(number, point, Optional(decimal))),
    Transform([](auto i) { return std::get<1>(i); }, Parser(point, decimal)));

  // optional_sign = "-"?
  const auto optional_sign =
    Transform([](auto i) { return i.has_value() ? -1 : 1; }, Optional(Literal("-")));

  // exp = ("e"|"E") optional_sign? number
  const auto exp =
    Transform([](auto i) { return std::get<1>(i) * std::get<2>(i); },
              Parser(Choice(Literal("e"), Literal("E")), optional_sign, number));

  // float_parser = optional_sign frac exp? | optional_sign number exp
  const auto float_parser = Choice(
    Transform(
      [](auto i) {
        return std::get<0>(i) * std::get<1>(i) * pow(10, std::get<2>(i).value_or(0));
      },
      Parser(optional_sign, frac, Optional(exp))),
    Transform(
      [](auto i) { return std::get<0>(i) * std::get<1>(i) * pow(10, std::get<2>(i)); },
      Parser(optional_sign, number, exp)));

  std::string input;
  std::cin >> input;
  const auto parsed = float_parser.parse(input);
  if(parsed) {
    fmt::print("Parsed: {}\t\"{}\" remaing\n", parsed->result, parsed->remainder);
  } else {
    fmt::print("Failed parsing\n");
  }
}