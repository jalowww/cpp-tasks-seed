#include "base85ed.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace
{

std::vector<std::uint8_t> str_to_vec(const std::string &s)
{
    return std::vector<std::uint8_t>(s.begin(), s.end());
}

const std::vector<std::pair<std::string, std::string>> short_cases =
{
    {"", ""},        {"F#", "1"},       {"F){", "12"},
    {"F)}j", "123"}, {"F)}kW", "1234"}, {"Xk~0{Zy<MXa%^M", "hello world"},
};

} // namespace

TEST(Base85Encode, ShortKnownValues)
{
    for (const auto &[encoded, decoded] : short_cases)
    {
        EXPECT_EQ(base85::encode(str_to_vec(decoded)), str_to_vec(encoded));
    }
}

TEST(Base85Decode, ShortKnownValues)
{
    for (const auto &[encoded, decoded] : short_cases)
    {
        EXPECT_EQ(base85::decode(str_to_vec(encoded)), str_to_vec(decoded));
    }
}

TEST(Base85RoundTrip, EmptyInput)
{
    const std::vector<std::uint8_t> data;
    EXPECT_EQ(base85::decode(base85::encode(data)), data);
}

TEST(Base85RoundTrip, TextInput)
{
    const auto data = str_to_vec("The quick brown fox jumps over the lazy dog");
    EXPECT_EQ(base85::decode(base85::encode(data)), data);
}

TEST(Base85RoundTrip, BinaryInputAllByteValues)
{
    std::vector<std::uint8_t> data;

    for (int i = 0; i <= 255; ++i)
    {
        data.push_back(static_cast<std::uint8_t>(i));
    }

    EXPECT_EQ(base85::decode(base85::encode(data)), data);
}

TEST(Base85RoundTrip, DifferentInputLengths)
{
    for (std::size_t len = 0; len < 100; ++len)
    {
        std::vector<std::uint8_t> data;

        for (std::size_t i = 0; i < len; ++i)
        {
            data.push_back(static_cast<std::uint8_t>((i * 31 + 17) % 256));
        }

        EXPECT_EQ(base85::decode(base85::encode(data)), data);
    }
}

TEST(Base85Decode, AcceptsShortFinalBlocksLikePythonB85)
{
    EXPECT_EQ(base85::decode(str_to_vec("F")), std::vector<std::uint8_t> {});
    EXPECT_EQ(base85::decode(str_to_vec("F#")), str_to_vec("1"));
    EXPECT_EQ(base85::decode(str_to_vec("F){")), str_to_vec("12"));
    EXPECT_EQ(base85::decode(str_to_vec("F)}j")), str_to_vec("123"));
}

TEST(Base85Decode, ThrowsOnInvalidCharacters)
{
    EXPECT_THROW(base85::decode(str_to_vec(" ")), std::invalid_argument);
    EXPECT_THROW(base85::decode(str_to_vec("\n")), std::invalid_argument);
    EXPECT_THROW(base85::decode(str_to_vec("F#\t")), std::invalid_argument);
}

TEST(Base85Decode, ThrowsOnOverflow)
{
    EXPECT_THROW(base85::decode(str_to_vec("~~~~~")), std::invalid_argument);
}
