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
    {
        //A≢Α.
        const std::vector<uint8_t> expected_encoding{ 0x41, 0xE2 , 0x89, 0xA2 , 0xCE, 0x91, 0x2E };
        const auto actual_encoding = Utf8::Encode({ 0x0041, 0x2262, 0x0391, 0x002E });
        ASSERT_EQ(expected_encoding, actual_encoding);
    }
    {
        //¢
        const std::vector<uint8_t> expected_encoding{ 0xC2, 0xA2 };
        const auto actual_encoding = Utf8::Encode({ 0xA2 });
        ASSERT_EQ(expected_encoding, actual_encoding);
    }
}

TEST(tests, InvalidCodePoint)
{
    const std::vector<uint8_t> expected_encoding(Utf8::kUtf8BytesReplacementCharacter.begin(), Utf8::kUtf8BytesReplacementCharacter.end());
    ASSERT_EQ((std::vector<uint8_t>{0xED, 0x9F, 0xBF}), Utf8::Encode({ 0xD7FF }));
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0xD801 }));
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0xDFFE }));
    ASSERT_EQ((std::vector<uint8_t>{0xEE, 0x80, 0x80}), Utf8::Encode({ 0xE000 }));
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0x2000000 }));
    ASSERT_EQ(expected_encoding, Utf8::Encode({ 0xFFFFFFFF }));
}

TEST(tests,DecodeValidSequences) 
{
    //struct TestVector{
    //    std::string encoding;
    //    std::vector<Utf8::UnicodeCodePoint> expected_encoding;
    //};
    //const std::vector<TestVector> test_vectors{
    //    { "不", { 0x233B4 } },
    //    { "中文", { 0x4E2D,0x6587 } },
    //    { "A≢Α.", { 0x0041, 0x2262, 0x0391, 0x002E } },
    //    { "¢", { 0xA2 } }
    //};
    //for (const auto& test_vector : test_vectors)
    //{
    //    const auto actual_encoding = Utf8::Decode(test_vector.encoding);
    //    ASSERT_EQ(test_vector.expected_encoding, actual_encoding);
    //}
}

TEST(tests,DecodeFromInputVector) 
{
    const auto actual_encoding = Utf8::Decode(std::vector<uint8_t>{0xE6, 0x97, 0xA5, 0xE6, 0X9C, 0xAC, 0xE8, 0xAA, 0x9E});
    ASSERT_EQ((std::vector<Utf8::UnicodeCodePoint>{0x65E5, 0x672C, 0x8A9E}), actual_encoding);
}







