//
// Created by hutj on 09.11.26.
//

#include <gtest/gtest.h>

#include "utils/Parsers.h"

// Test that parseDouble returns correct numbers if a valid string is given and std::nullopt otherwise
TEST(ParsersTest, parseDouble) {
    // positive numbers
    ASSERT_TRUE(parseDouble("1.12345"));
    EXPECT_DOUBLE_EQ(parseDouble("1.12345").value(), 1.12345);

    // negative numbers
    ASSERT_TRUE(parseDouble("-1.12345"));
    EXPECT_DOUBLE_EQ(parseDouble("-1.12345").value(), -1.12345);

    // invalid options
    EXPECT_FALSE(parseDouble("1.12x345"));
    EXPECT_FALSE(parseDouble(""));

    // non-finite values are valid doubles but not valid simulation inputs
    EXPECT_FALSE(parseDouble("inf"));
    EXPECT_FALSE(parseDouble("-inf"));
    EXPECT_FALSE(parseDouble("nan"));
}

// Test that parseInt returns correct numbers if a valid string is given and std::nullopt otherwise
TEST(ParsersTest, parseInt) {
    // positive numbers
    ASSERT_TRUE(parseInt("123456"));
    EXPECT_EQ(parseInt("123456").value(), 123456);

    // negative numbers
    ASSERT_TRUE(parseInt("-123456"));
    EXPECT_EQ(parseInt("-123456").value(), -123456);

    // invalid options
    EXPECT_FALSE(parseInt("1.1"));
    EXPECT_FALSE(parseInt("1x234"));
    EXPECT_FALSE(parseInt(""));
}

// Test that trim removes all whitespace and whitespace control sequences from a given string but no whitespace in the
// middle, text or \n
TEST(ParsersTest, trim) {
    const std::string whitespace_front = "\r    \t\t\r ";
    const std::string whitespace_back = "     \t \r";
    const std::string test_string = "this is    a string 123.2 with \t lots of #,.- gaps and \r \n\n\n characters";

    const std::string full_string = whitespace_front + test_string + whitespace_back;

    const std::string trimmed(trim(full_string));

    EXPECT_EQ(trimmed, test_string);

    // empty string
    EXPECT_EQ(trim(""), std::string_view());

    // only whitespace
    EXPECT_EQ(trim("   \t\r "), std::string_view());

    // \n at the start and end
    EXPECT_EQ(trim("\n test \n"), "\n test \n");
}