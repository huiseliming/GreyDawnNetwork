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
    const std::vector<uint8_t> expected_encoding{ 0x48, 0x65 , 0x6C, 0x6C , 0x6F };
    const auto actual_encoding = Utf8::Encode(Utf8::AsciiToUnicode("Hello"));
    ASSERT_EQ(expected_encoding, actual_encoding);
}


TEST(tests, EncodeChinese)
{
    //Utf8 "你好"
    const std::vector<uint8_t> expected_encoding{ 0xE4, 0xBD , 0xA0, 0xE5 , 0xA5, 0xBD};
    //Unicode "你好"
    const auto actual_encoding = Utf8::Encode({ 0x4F60,0x597d });
    ASSERT_EQ(expected_encoding, actual_encoding);
}





