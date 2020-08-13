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
    {
        //中文
        const std::vector<uint8_t> expected_encoding{ 0xE4, 0xB8 , 0xAD, 0xE6 , 0x96, 0x87 };
        const auto actual_encoding = Utf8::Encode({ 0x4E2D,0x6587 });
        ASSERT_EQ(expected_encoding, actual_encoding);
    }
    {
        //不
        const std::vector<uint8_t> expected_encoding{ 0xF0, 0xA3 , 0x8E, 0xB4 };
        const auto actual_encoding = Utf8::Encode({ 0x233B4 });
        ASSERT_EQ(expected_encoding, actual_encoding);
    }
}

TEST(tests, Symbols)
{
    //A≢Α.
    const std::vector<uint8_t> expected_encoding{ 0x41, 0xE2 , 0x89, 0xA2 , 0xCE, 0x91, 0x2E };
    const auto actual_encoding = Utf8::Encode({ 0x0041, 0x2262, 0x0391, 0x002E });
    ASSERT_EQ(expected_encoding, actual_encoding);
}

TEST(tests, InvalidCodePoint)
{
    const std::vector<uint8_t> expected_encoding(Utf8::kUtf8EncodeReplacementCharacter.begin(), Utf8::kUtf8EncodeReplacementCharacter.end());
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0xD801 }));
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0xDFFE }));
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0x2000000 }));
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0xFFFFFFFF }));
}

