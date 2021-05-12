#include <parser.hpp>
#include <catch2/catch.hpp>

SCENARIO("Literal should parse a correct input", "[literal]"){
    GIVEN("A parser 'Literal(\"a\")"){
        const auto parser = Literal("a");
        WHEN("The value 'ab' is parsed"){
            const auto parsed = parser.parse("ab");
            THEN("the return should hold a value"){
                REQUIRE(parsed.has_value());
            }
            AND_THEN("the result value should be 'a'"){
                REQUIRE(parsed->result == "a");
            }
            AND_THEN("the remainder should be 'b'"){
                REQUIRE(parsed->remainder == "b");
            }
        }
    }
}

SCENARIO("Literal should not parse a incorrect input", "[literal]"){
    GIVEN("A parser 'Literal(\"a\")"){
        const auto parser = Literal("a");
        WHEN("The value 'b' is parsed"){
            const auto parsed = parser.parse("b");
            THEN("the return should not hold a value"){
                REQUIRE_FALSE(parsed.has_value());
            }
        }
    }
}