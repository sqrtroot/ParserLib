#include <fmt/core.h>
#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <variant>

template<typename T>
struct Result {
  using result_t = T;
  result_t         result;
  std::string_view remainder;

  constexpr Result(T result, std::string_view sv): result(result), remainder(sv){};
};

template<typename T>
using Parsed = std::optional<Result<T>>;

template<typename T, typename F>
struct Transform {
  using result_t = std::result_of_t<F(typename T::result_t)>;

  F transformer;
  T parser;

  Transform(F transformer, T &&t):
      transformer(transformer), parser(std::forward<T>(t)) {}

  constexpr Parsed<result_t> parse(std::string_view input) const {
    if(auto parsed = parser.parse(input)) {
      return Result(transformer(parsed->result), parsed->remainder);
    }
    return std::nullopt;
  }
};
struct Literal {
  using result_t = std::string_view;
  std::string_view literal;
  constexpr Literal(std::string_view literal): literal(literal){};

  constexpr Parsed<result_t> parse(std::string_view input) const {
    if(input.starts_with(literal)) {
      input.remove_prefix(literal.size());
      return Result(std::string_view(input.begin(), literal.length()), input);
    }
    return std::nullopt;
  };
};

template<typename... T>
struct Parser {
  std::tuple<T...> parsers;
  using result_t = std::tuple<typename T::result_t...>;
  Parser(T &&...ts): parsers(std::move(ts)...) {}

  template<typename P>
  constexpr Parsed<std::tuple<typename P::result_t>>
    parse_impl(std::string_view input, const P &parser) const {
    if(auto parsed = parser.parse(input)) {
      return Result(std::make_tuple(parsed->result), parsed->remainder);
    }
    return std::nullopt;
  }

  template<typename P, typename... Ps>
  constexpr Parsed<std::tuple<typename P::result_t, typename Ps::result_t...>>
    parse_impl(std::string_view input, const P &parser, const Ps &...parsers) const {
    if(auto parsed = parser.parse(input)) {
      auto next_result = parse_impl(parsed->remainder, parsers...);
      if(next_result) {
        return Result(std::tuple_cat(std::make_tuple(parsed->result), next_result->result),
                      next_result->remainder);
      }
    }
    return std::nullopt;
  }

  constexpr auto parse(std::string_view input) const {
    return std::apply([&](auto &...ts) { return parse_impl(input, ts...); }, parsers);
  };
};

template<typename T>
struct Optional {
  using result_t = std::optional<typename T::result_t>;

  T t;

  constexpr Optional(T &&t): t(std::forward<T>(t)){};

  constexpr Parsed<result_t> parse(std::string_view input) const {
    if(auto v = t.parse(input)) {
      return Result(std::make_optional(v->result), v->remainder);
    }
    return Result(result_t(std::nullopt), input);
  };
};

template<typename T, typename F>
struct Plus {
  using result_t = std::vector<typename T::result_t>;
  T t;
  constexpr Plus(T &&t): t(t){};

  constexpr Parsed<result_t> parse(std::string_view input) const {
    auto v = t.parse(input);
    if(!v.has_value()) {
      return std::nullopt;
    }
    result_t out;
    auto     last_good = v;
    do {
      out.emplace_back(std::move(v->result));
      last_good = v;
    } while((v = t.parse(v->remainder)));
    return Result(out, last_good->remainder);
  };
};

template<typename T>
Plus(T) -> Plus<T, typename T::result_t>;

template<typename T>
struct Plus<T, std::string_view> {
  using result_t = std::string_view;
  T t;
  constexpr Plus(T &&t): t(t){};
  constexpr Parsed<result_t> parse(std::string_view input) const {
    auto v = t.parse(input);
    if(!v.has_value()) {
      return std::nullopt;
    }
    auto last_good = v;
    while((v = t.parse(v->remainder))) {
      last_good = v;
    }
    return Result(std::string_view(&input.front(), &last_good->result.back()),
                  last_good->remainder);
  };
};

template<typename T>
struct Star {
  Optional<Plus<T, typename T::result_t>> t;
  using result_t = typename decltype(t)::result_t;
  constexpr Star(T &&t): t(Optional(Plus(std::move(t)))){};
  constexpr Parsed<result_t> parse(std::string_view input) const {
    return t.parse(input);
  }
};
