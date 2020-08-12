#include <gtest/gtest.h>
#include <utility/Utility.h>
#include <filesystem>
#include <vector>

using namespace GreyDawn;



TEST(tests, AsciiToUnicode)
{
    const std::vector<uint32_t> expected_encoding{ 0x48, 0x65 , 0x6C, 0x6C , 0x6F };
    const auto actual_encoding = Utf8::AsciiToUnicode("Hello");
    ASSERT_EQ(expected_encoding,actual_encoding);
}


TEST(tests, EncodeAscii)
{
    //const std::vector<uint8_t> expected_encoding{ 0x48, 0x65 , 0x6C, 0x6C , 0x6F };
    //const auto actual_ encoding = "Hello"
}
