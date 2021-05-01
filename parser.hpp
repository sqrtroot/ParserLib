#include <string_view>
#include <optional>
#include <tuple>

template <typename... T>
struct Parser
{
    std::tuple<T...> parser;

    constexpr Parser(T &&...t) : parser(std::move(t)...){};

    template <typename nT>
    constexpr auto then(nT &&t)
    {
        const auto const_arg_tuple = std::tuple_cat(parser, std::make_tuple(std::move(t)));
        return std::make_from_tuple<Parser<T..., nT>>(const_arg_tuple);
    }

    constexpr std::optional<std::string_view> _parse_impl(std::string_view input) const
    {
        return input;
    }

    template <typename P, typename... Ps>
    constexpr std::optional<std::string_view> _parse_impl(std::string_view input, const P &parser, const Ps &...parsers) const
    {
        auto next_input = parser.parse(input);
        if constexpr (sizeof...(Ps) > 0)
        {
            if (next_input)
            {
                return _parse_impl(*next_input, parsers...);
            }
        }
        return next_input;
    }
    constexpr std::optional<std::string_view> parse(std::string_view input) const
    {
        return std::apply([&](T... tupleArgs) { return _parse_impl(input, tupleArgs...); }, parser);
    }
};

struct Literal
{
    std::string_view literal;
    constexpr Literal(std::string_view literal) : literal(literal){};

    constexpr std::optional<std::string_view> parse(std::string_view input) const
    {
        if (input.starts_with(literal))
        {
            input.remove_prefix(literal.size());
            return input;
        }
        return std::nullopt;
    };
};
template <typename T>
struct Optional
{
    T t;
    constexpr Optional(T &&t) : t(t){};

    constexpr std::optional<std::string_view> parse(std::string_view input) const
    {
        if (auto v = t.parse(input))
        {
            return *v;
        }
        return input;
    };
};

template <typename T>
struct Plus
{
    T t;
    constexpr Plus(T &&t) : t(t){};

    constexpr std::optional<std::string_view> parse(std::string_view input) const
    {
        auto v = t.parse(input);
        if (!v.has_value())
        {
            return std::nullopt;
        }
        while (auto new_input = t.parse(*v))
        {
            v = *new_input;
        };
        return v;
    };
};

template <typename T>
struct Star
{
    Optional<Plus<T>> t;
    constexpr Star(T &&t) : t(Optional(Plus(std::move(t)))){};
    constexpr std::optional<std::string_view> parse(std::string_view input) const
    {
        return t.parse(input);
    }
};

template <typename... Parsers>
struct Choice
{
    std::tuple<Parsers...> parsers;

    Choice(Parsers &&...parsers) : parsers(std::move(parsers)...){};

    template <typename P, typename... Ps>
    constexpr std::optional<std::string_view> _parse_impl(std::string_view input, const P &parser, const Ps &...parsers) const
    {
        if (auto next_input = parser.parse(input))
        {
            return *next_input;
        };

        if constexpr (sizeof...(Ps) > 0)
        {
            return _parse_impl(input, parsers...);
        }
        else
        {
            return std::nullopt;
        }
    }
    constexpr std::optional<std::string_view> parse(std::string_view input) const
    {
        return std::apply([&](Parsers... tupleArgs) { return _parse_impl(input, tupleArgs...); }, parsers);
    }
};