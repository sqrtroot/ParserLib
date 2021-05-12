#include <catch2/catch.hpp>
#include <parser.hpp>

SCENARIO("Optional should parse a correct input", "[optional]") {
  GIVEN("A parser Optional(Literal(\"a\")") {
    auto parser = Optional(Literal("a"));
    WHEN("The input 'ab' is parsed") {
      const auto parsed = parser.parse("ab");
      THEN("the return should hold a value") { REQUIRE(parsed.has_value()); }
      AND_THEN("the result value should hold a value") {
        REQUIRE(parsed->result.has_value());
      }
      AND_THEN("the result value value should be 'a'") {
        REQUIRE(*parsed->result == "a");
      }
      AND_THEN("the remainder should be 'b'") { REQUIRE(parsed->remainder == "b"); }
    }
  }
}

SCENARIO("Optional should parse an incorrect input", "[optional]") {
  GIVEN("A parser Optional(Literal(\"a\")") {
    auto parser = Optional(Literal("a"));
    WHEN("The input 'ab' is parsed") {
      const auto parsed = parser.parse("ba");
      THEN("the return should hold a value") { REQUIRE(parsed.has_value()); }
      AND_THEN("the result value should not hold a value") {
        REQUIRE_FALSE(parsed->result.has_value());
      }
      AND_THEN("the remainder should be ba") { REQUIRE(parsed->remainder == "ba"); }
    }
  }
}