#include <fmt/core.h>
#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <variant>
/**
 * @file parser.hpp
 * @brief File containing all the basic parsing to construct usefull parser combinators.
 *
 * Some types have 2 implementations. One for general types and one for string_views.
 * String_views are the basic non transformed representation of a match for this library.
 * They give us the abbility to pass safe substrings without allocating memory, and are easy to work with (have == operator, etc.)
 */
// IDEAS: if a choice parser gets a choice, we can unnest them

/**
 * @brief Result of parsing.
 *
 * For parsers it should be wrapped in a Parsed object
 * @see Parsed
 * @tparam T result type
 */
template<typename T>
struct Result {
  using result_t = T;
  result_t         result;       //!< the result
  std::string_view remainder;    //!< what's left after parsing

  constexpr Result(T result, std::string_view remainder):
      result(result), remainder(remainder){};
};

/**
 * @brief Output of parsing
 * This will hold a value if parsing was succesfull and no value if parsing failed
 * @tparam T Type of parse output made into Result<T>
 */
template<typename T>
using Parsed = std::optional<Result<T>>;

/**
 * @brief Transform the type of a parser
 *
 * With this you can transform from a basic string_view (or any other parsing result) into another type.
 *
 * @tparam T Original Parser
 * @tparam F Transformer result type
 */
template<typename T, typename F>
struct Transform {
  using result_t = std::result_of_t<F(typename T::result_t)>;

  F transformer;
  T parser;

  Transform(F transformer, T t):
      transformer(transformer), parser(std::forward<T>(t)) {}

  constexpr Parsed<result_t> parse(std::string_view input) const {
    if(auto parsed = parser.parse(input)) {
      return Result(transformer(parsed->result), parsed->remainder);
    }
    return std::nullopt;
  }
};
/**
 * @brief Parser that parses a Literal
 *
 * This is the most basic of parser, parsing one or multiple characters.
 *
 */
struct Literal {
  using result_t = std::string_view;
  std::string_view literal;
  constexpr Literal(std::string_view literal): literal(literal){};

  constexpr Parsed<result_t> parse(std::string_view input) const {
    if(input.starts_with(literal)) {
      const std::string_view result_view{input.begin(), literal.length()};
      input.remove_prefix(literal.size());
      return Result(result_view, input);
    }
    return std::nullopt;
  };
};
/**
 * @brief Parser that parses based on a predicate
 *
 * If the predicate is returns true this parser will succeed and give a result with the first character. Otherwise it will fail
 * @tparam T
 */
template<typename T>
struct Predicate {
  using result_t = std::string_view;
  T predicate;
  constexpr Predicate(T predicate): predicate(predicate){};

  constexpr Parsed<result_t> parse(std::string_view input) const {
    if(predicate(input.front())) {
      return Result(std::string_view(input.begin(), 1), input.substr(1));
    };
    return std::nullopt;
  };
};
template<typename only_stringviews, typename... T>
struct Parser;

template<typename... T>
struct Parser<std::false_type, T...> {
  std::tuple<T...> parsers;
  using result_t = std::tuple<typename T::result_t...>;
  Parser(T... ts): parsers(ts...) {}

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

template<typename... T>
struct Parser<std::true_type, T...> {
  std::tuple<T...> parsers;
  using result_t = std::string_view;
  Parser(T... ts): parsers(ts...) {}

  template<typename P>
  constexpr Parsed<std::string_view> parse_impl(std::string_view input, const P &parser) const {
    if(auto parsed = parser.parse(input)) {
      return Result(std::string_view(input.begin(), parsed->result.end()), parsed->remainder);
    }
    return std::nullopt;
  }

  template<typename P, typename... Ps>
  constexpr Parsed<std::string_view>
    parse_impl(std::string_view input, const P &parser, const Ps &...parsers) const {
    if(auto parsed = parser.parse(input)) {
      auto next_result = parse_impl(parsed->remainder, parsers...);
      if(next_result) {
        return Result(std::string_view(input.begin(), next_result->result.end()),
                      next_result->remainder);
      }
    }
    return std::nullopt;
  }

  constexpr auto parse(std::string_view input) const {
    return std::apply([&](auto &...ts) { return parse_impl(input, ts...); }, parsers);
  };
};

template<typename... Ts>
Parser(Ts...)
  -> Parser<std::conditional_t<std::conjunction_v<std::is_same<std::string_view, typename Ts::result_t>...>, std::true_type, std::false_type>, Ts...>;

template<typename T>
struct Optional {
  using result_t = std::optional<typename T::result_t>;

  T t;

  constexpr Optional(T t): t(std::forward<T>(t)){};

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
  constexpr Plus(T t): t(t){};

  constexpr Parsed<result_t> parse(std::string_view input) const {
    auto v = t.parse(input);
    if(!v.has_value()) {
      return std::nullopt;
    }
    result_t out;
    auto     last_good = v;
    do {
      out.push_back(v->result);
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
  constexpr Plus(T t): t(t){};
  constexpr Parsed<result_t> parse(std::string_view input) const {
    auto v = t.parse(input);
    if(!v.has_value()) {
      return std::nullopt;
    }
    auto last_good = v;
    while((v = t.parse(v->remainder))) {
      last_good = v;
    }
    return Result(std::string_view(input.begin(), last_good->result.end()), last_good->remainder);
  };
};

template<typename T>
struct Star {
  Optional<Plus<T, typename T::result_t>> t;
  using result_t = typename decltype(t)::result_t;
  constexpr Star(T t): t(Optional(Plus(t))){};
  constexpr Parsed<result_t> parse(std::string_view input) const {
    return t.parse(input);
  }
};

template<typename one_type, typename... Ts>
struct Choice;
template<typename... Ts>
struct Choice<std::false_type, Ts...> {
  std::tuple<Ts...> choices;
  using result_t = std::variant<typename Ts::result_t...>;
  constexpr Choice(Ts... choices): choices(choices...){};

  template<size_t I, size_t... Is>
  constexpr Parsed<result_t>
    parse_impl(std::string_view input, std::index_sequence<I, Is...>) const {
    if(auto output = std::get<I>(choices).parse(input)) {
      return Result(result_t(std::in_place_index_t<I>{}, output->result), output->remainder);
    };

    if constexpr(sizeof...(Is) > 0) {
      return parse_impl(input, std::index_sequence<Is...>{});
    } else {
      return std::nullopt;
    }
  }

  constexpr Parsed<result_t> parse(std::string_view input) const {
    return parse_impl(input, std::make_index_sequence<sizeof...(Ts)>{});
  };
};

template<typename... Ts>
struct Choice<std::true_type, Ts...> {
  std::tuple<Ts...> choices;
  using result_t = typename std::tuple_element_t<0, std::tuple<Ts...>>::result_t;
  constexpr Choice(Ts... choices): choices(choices...){};

  template<size_t I, size_t... Is>
  constexpr Parsed<result_t>
    parse_impl(std::string_view input, std::index_sequence<I, Is...>) const {
    if(auto output = std::get<I>(choices).parse(input)) {
      return Result(output->result, output->remainder);
    };

    if constexpr(sizeof...(Is) > 0) {
      return parse_impl(input, std::index_sequence<Is...>{});
    } else {
      return std::nullopt;
    }
  }

  constexpr Parsed<result_t> parse(std::string_view input) const {
    return parse_impl(input, std::make_index_sequence<sizeof...(Ts)>{});
  };
};

template<typename... Ts>
Choice(Ts...)
  -> Choice<std::conditional_t<std::conjunction_v<std::is_same<typename std::tuple_element_t<0, std::tuple<Ts...>>::result_t, typename Ts::result_t>...>, std::true_type, std::false_type>,
            Ts...>;
