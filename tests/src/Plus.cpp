#include <catch2/catch.hpp>
#include <parser.hpp>

SCENARIO("Plus of parser which return string_view should return string_view",
         "[plus, string_view]") {
  GIVEN("A parser 'Literal(\"a\")'") {
    WHEN("A plus parser is made from that parser") {
      auto plus = Plus(Literal("a"));
      THEN("The return type of the plus parser should be string_view") {
        REQUIRE(std::is_same_v<typename decltype(plus)::result_t, std::string_view>);
      }
    }
  }
}

SCENARIO("Plus should parse one correct input into a string_view", "[plus, string_view]") {
  GIVEN("A parser 'Plus(Literal(\"a\"))") {
    const auto parser = Plus(Literal("a"));
    WHEN("The value 'ab' is parsed") {
      const auto parsed = parser.parse("ab");
      THEN("the return should hold a value") { REQUIRE(parsed.has_value()); }
      AND_THEN("the result value should be 'a'") { REQUIRE(parsed->result == "a"); }
      AND_THEN("the remainder should be 'b'") { REQUIRE(parsed->remainder == "b"); }
    }
  }
}

SCENARIO("Plus should parse multiple correct inputs into a string_view", "[plus, string_view]") {
  GIVEN("A parser 'Plus(Literal(\"a\"))") {
    const auto parser = Plus(Literal("a"));
    WHEN("The value 'aab' is parsed") {
      const auto parsed = parser.parse("aab");
      THEN("the return should hold a value") { REQUIRE(parsed.has_value()); }
      AND_THEN("the result value should be 'aa'") {
        REQUIRE(parsed->result == "aa");
      }
      AND_THEN("the remainder should be 'b'") { REQUIRE(parsed->remainder == "b"); }
    }
  }
}

SCENARIO("Plus should not parse incorrect inputs into a string_view", "[plus, string_view]") {
  GIVEN("A parser 'Plus(Literal(\"a\"))") {
    const auto parser = Plus(Literal("a"));
    WHEN("The value 'baa' is parsed") {
      const auto parsed = parser.parse("baa");
      THEN("the return should hold a value") { REQUIRE_FALSE(parsed.has_value()); }
    }
  }
}

SCENARIO("Plus should return a vector for non stringview types", "[plus, vector]") {
  GIVEN("A parser 'Transform([](auto)->int{return 1;}, Literal(\"1\"))'") {
    WHEN("A plus parser is made from that parser") {
      auto plus = Plus(Transform([](auto) -> int { return 1; }, Literal("1")));
      THEN("The return type of the plus parser should be vector of ints") {
        REQUIRE(std::is_same_v<typename decltype(plus)::result_t, std::vector<int>>);
      }
    }
  }
}

SCENARIO("Plus should parse one correct input into a vector", "[plus, vector]") {
  GIVEN("A parser 'Plus(Transform([](auto)->int{return 1;},Literal(\"1\")))") {
    auto parser = Plus(Transform([](auto) -> int { return 1; }, Literal("1")));
    WHEN("The value '10' is parsed") {
      const auto parsed = parser.parse("10");
      THEN("the return should hold a value") { REQUIRE(parsed.has_value()); }
      AND_THEN("the result value should be {1}") {
        REQUIRE(parsed->result == std::vector({1}));
      }
      AND_THEN("the remainder should be '0'") { REQUIRE(parsed->remainder == "0"); }
    }
  }
}

SCENARIO("Plus should parse multiple correct inputs into a vector", "[plus, vector]") {
  GIVEN("A parser 'Plus(Transform([](auto)->int{return 1;},Literal(\"1\")))") {
    auto parser = Plus(Transform([](auto) -> int { return 1; }, Literal("1")));
    WHEN("The value '110' is parsed") {
      const auto parsed = parser.parse("110");
      THEN("the return should hold a value") { REQUIRE(parsed.has_value()); }
      AND_THEN("the result value should be {1,1}") {
        REQUIRE(parsed->result == std::vector({1, 1}));
      }
      AND_THEN("the remainder should be '0'") { REQUIRE(parsed->remainder == "0"); }
    }
  }
}

SCENARIO("Plus should not parse incorrect inputs into a vector", "[plus, vector]") {
  GIVEN("A parser 'Plus(Transform([](auto)->int{return 1;},Literal(\"1\")))") {
    auto parser = Plus(Transform([](auto) -> int { return 1; }, Literal("1")));
    WHEN("The value '001' is parsed") {
      const auto parsed = parser.parse("001");
      THEN("the return should not hold a value") {
        REQUIRE_FALSE(parsed.has_value());
      }
    }
  }
}