#include "gtest/gtest.h"

#include "Parser.hpp"

#include <string>
#include <string_view>
#include <tuple>

namespace {

using namespace std::string_view_literals;
using namespace std::string_literals;

using namespace parser;

TEST(StringConvertTest, TrimSpaces_RemovesLeadingAndTrailingWhitespace) {
    EXPECT_EQ(TrimSpaces(" \t expense\n "sv), "expense"sv);
}

TEST(StringConvertTest, TrimSpaces_WhitespaceOnly_ReturnsEmptyView) {
    EXPECT_EQ(TrimSpaces(" \t\n\r"sv), ""sv);
}

TEST(StringConvertTest, ToLower_ConvertsAsciiLetters) {
    EXPECT_EQ(ToLower("  FoOd 42"sv), "  food 42"s);
}

TEST(StringConvertTest, ToLower_PreservesNonAsciiBytes) {
    EXPECT_EQ(ToLower("Hello"sv), "hello"s);
}

class TestTrimSpacesLeft : public ::testing::TestWithParam<
                               std::tuple<std::string_view, std::string_view>> {
};

TEST_P(TestTrimSpacesLeft, RemovesSpacesFromLeft) {
    const auto& [input, expected] = GetParam();
    EXPECT_EQ(TrimSpacesLeft(input), expected);
}

INSTANTIATE_TEST_SUITE_P(
    TrimSpacesLeft, TestTrimSpacesLeft,
    ::testing::Values(std::make_tuple("  Hello  ", "Hello  "),
                      std::make_tuple("\tHello", "Hello"),
                      std::make_tuple("\nHello", "Hello"),
                      std::make_tuple("\rHello", "Hello"),
                      std::make_tuple("\vHello", "Hello"),
                      std::make_tuple("\fHello", "Hello"),
                      std::make_tuple("  \t\nHello", "Hello"),
                      std::make_tuple("Hello", "Hello"),
                      std::make_tuple("", ""), std::make_tuple("   ", ""),
                      std::make_tuple("\t\t\t", ""),
                      std::make_tuple("  \t\n\r\v\f", ""),
                      std::make_tuple("  \t\n  Hello  \t\n", "Hello  \t\n"),
                      std::make_tuple("\r\nHello", "Hello"),
                      std::make_tuple("  Привет  ", "Привет  "),
                      std::make_tuple("\tМир", "Мир")));

class TestTrimSpacesRigth
    : public ::testing::TestWithParam<
          std::tuple<std::string_view, std::string_view>> {};

TEST_P(TestTrimSpacesRigth, RemovesSpacesFromLeft) {
    const auto& [input, expected] = GetParam();
    EXPECT_EQ(TrimSpacesRight(input), expected);
}

INSTANTIATE_TEST_SUITE_P(
    TrimSpacesRigth, TestTrimSpacesRigth,
    ::testing::Values(std::make_tuple("  Hello  ", "  Hello"),
                      std::make_tuple("Hello\t", "Hello"),
                      std::make_tuple("Hello\n", "Hello"),
                      std::make_tuple("Hello\r", "Hello"),
                      std::make_tuple("Hello\v", "Hello"),
                      std::make_tuple("Hello\f", "Hello"),
                      std::make_tuple("Hello  \t\n", "Hello"),
                      std::make_tuple("Hello", "Hello"),
                      std::make_tuple("", ""), std::make_tuple("   ", ""),
                      std::make_tuple("\t\t\t", ""),
                      std::make_tuple("  \t\n\r\v\f", ""),
                      std::make_tuple("  \t\n  Hello  \t\n", "  \t\n  Hello"),
                      std::make_tuple("Hello\r\n", "Hello"),
                      std::make_tuple("  Привет  ", "  Привет"),
                      std::make_tuple("Мир\t", "Мир")));

class TestTrimSpaces : public ::testing::TestWithParam<
                           std::tuple<std::string_view, std::string_view>> {};

TEST_P(TestTrimSpaces, RemovesSpaces) {
    const auto& [input, expected] = GetParam();
    EXPECT_EQ(TrimSpaces(input), expected);
}

INSTANTIATE_TEST_SUITE_P(
    TrimSpaces, TestTrimSpaces,
    ::testing::Values(std::make_tuple("  Hello  ", "Hello"),
                      std::make_tuple("\tHello\t", "Hello"),
                      std::make_tuple("\nHello\n", "Hello"),
                      std::make_tuple("\rHello\r", "Hello"),
                      std::make_tuple("\vHello\v", "Hello"),
                      std::make_tuple("\fHello\f", "Hello"),
                      std::make_tuple("  \t\nHello  \t\n", "Hello"),
                      std::make_tuple("   ", ""), std::make_tuple("\t\t\t", ""),
                      std::make_tuple("  \t\n\r\v\f", ""),
                      std::make_tuple("Hello", "Hello"),
                      std::make_tuple("", ""),
                      std::make_tuple("  \t\n  Hello World  \t\n  ",
                                      "Hello World"),
                      std::make_tuple("  Hello  World  ", "Hello  World"),
                      std::make_tuple("   A   ", "A"),
                      std::make_tuple("  Привет Мир  ", "Привет Мир"),
                      std::make_tuple("\tРусский текст\t", "Русский текст"),
                      std::make_tuple("  \n\r\v\fHello\n\r\v\f  ", "Hello"),
                      std::make_tuple("\u00A0Hello", "Hello"),
                      std::make_tuple("Hello\u00A0", "Hello"),
                      std::make_tuple("\u00A0Hello\u00A0", "Hello")));

}  // namespace
