#pragma once
#include <stddef.h>
#define REPLACEMENT_INVALID_CODE_POINT

namespace GreyDawn
{
    class Utf8
    {
    private:
        Utf8() = delete;
        Utf8(const Utf8&) = delete;
        Utf8(Utf8&&) = delete;
        Utf8& operator=(const Utf8&) = delete;
        Utf8& operator=(Utf8&&) = delete;
    public:
        using UnicodeCodePoint = uint32_t;
        inline static const std::array< uint8_t , 3> kUtf8BytesReplacementCharacter = { 0xEF, 0xBF, 0xBD};
        inline static const UnicodeCodePoint kUnicodeReplacementCharacter = 0xFFFD;
        inline static const UnicodeCodePoint kReserveRangeBegin = 0xD800;
        inline static const UnicodeCodePoint kReserveRangeEnd = 0xDFFF;
        inline thread_local static UnicodeCodePoint current_unicode_code_point = 0;
        inline thread_local static size_t current_unicode_remaining_bytes = 0;
        inline thread_local static size_t current_unicode_total_bytes = 0;
        inline thread_local static bool tlsm_is_valid = true;
    public:
        static std::vector<UnicodeCodePoint> AsciiToUnicode(const std::string& ascii)
        {
            return std::vector<uint32_t>(ascii.begin(),ascii.end());
        }

        static std::vector<uint8_t> Encode(const std::vector<UnicodeCodePoint> code_points)
        {
            std::vector<uint8_t> utf8_bytes;
            for(auto code_point : code_points)
            {
                if(code_point < 0x80){
                    utf8_bytes.push_back(uint8_t(code_point & 0x7F));
                } else if (code_point < 0x800) {
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x1F) + 0xC0));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                } else if (code_point < 0x10000) {
                    if ((code_point >= kReserveRangeBegin) && (code_point <= kReserveRangeEnd)) { // Unicode在范围D800-DFFF中不存在任何字符
                        utf8_bytes.insert(utf8_bytes.end(), kUtf8BytesReplacementCharacter.begin(), kUtf8BytesReplacementCharacter.end());
                    } else {
                        utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x0F) + 0xE0));
                        utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                        utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                    }
#ifdef REPLACEMENT_INVALID_CODE_POINT
                } else if (code_point < 0x110000 /*&& code_point < 0x2000000*/) { //Unicode目前只使用到了10FFFF
                    utf8_bytes.push_back(uint8_t(((code_point >> 18) & 0x07) + 0xF0));
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                } else {
                    utf8_bytes.insert(utf8_bytes.end(), kUtf8BytesReplacementCharacter.begin(), kUtf8BytesReplacementCharacter.end());
                }
#else
                } else if (code_point < 0x2000000) {
                    utf8_bytes.push_back(uint8_t(((code_point >> 18) & 0x07) + 0xF0));
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                } else if (code_point < 0x40000000) {
                    utf8_bytes.insert(utf8_bytes.end(), kUtf8BytesReplacementCharacter.begin(), kUtf8BytesReplacementCharacter.end());

                    utf8_bytes.push_back(uint8_t(((code_point >> 24) & 0x03) + 0xF8));
                    utf8_bytes.push_back(uint8_t(((code_point >> 18) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                } else {
                    utf8_bytes.insert(utf8_bytes.end(), kUtf8BytesReplacementCharacter.begin(), kUtf8BytesReplacementCharacter.end());
                    utf8_bytes.push_back(uint8_t(((code_point >> 30) & 0x01) + 0xFC));
                    utf8_bytes.push_back(uint8_t(((code_point >> 24) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 18) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                }
#endif
            }
            return utf8_bytes;
        }
        
        static std::vector<UnicodeCodePoint> Decode(const std::vector<uint8_t>& utf8_bytes)
        {
            std::vector<UnicodeCodePoint> unicode_output;
            current_unicode_code_point = 0;
            current_unicode_remaining_bytes = 0;
            current_unicode_total_bytes = 0;
            tlsm_is_valid = true;
            for (auto utf8_byte : utf8_bytes)
            {
                if (current_unicode_remaining_bytes == 0) {
                    if ((utf8_byte & 0x80) == 0) {
                        unicode_output.push_back(utf8_byte);
                    } else if ((utf8_byte & 0xE0) == 0xC0) {
                        current_unicode_remaining_bytes = 1;
                        current_unicode_code_point += utf8_byte & 0x1F;
                    } else if ((utf8_byte & 0xF0) == 0xE0) {
                        current_unicode_remaining_bytes = 2;
                        current_unicode_code_point += utf8_byte & 0xF;
                    } else if ((utf8_byte & 0xF8) == 0xF0) {
                        current_unicode_remaining_bytes = 3;
                        current_unicode_code_point += utf8_byte & 0x7;
                    } else {
                        tlsm_is_valid = false;
                        unicode_output.push_back(kUnicodeReplacementCharacter);
                    }
                    current_unicode_total_bytes = current_unicode_remaining_bytes + 1;
                } else if ((utf8_byte & 0xC0) != 0x80) { //检测不应出现的二进制格式
                    tlsm_is_valid = false;
                    unicode_output.push_back(kUnicodeReplacementCharacter);
                    current_unicode_remaining_bytes = 0;
                    current_unicode_code_point = 0;
                    //重新将这个字节当作起始字节处理
                    if ((utf8_byte & 0x80) == 0) {
                        unicode_output.push_back(utf8_byte);
                    }
                    else if ((utf8_byte & 0xE0) == 0xC0) {
                        current_unicode_remaining_bytes = 1;
                        current_unicode_code_point += utf8_byte & 0x1F;
                    }
                    else if ((utf8_byte & 0xF0) == 0xE0) {
                        current_unicode_remaining_bytes = 2;
                        current_unicode_code_point += utf8_byte & 0xF;
                    }
                    else if ((utf8_byte & 0xF8) == 0xF0) {
                        current_unicode_remaining_bytes = 3;
                        current_unicode_code_point += utf8_byte & 0x7;
                    }
                    else {
                        tlsm_is_valid = false;
                        unicode_output.push_back(kUnicodeReplacementCharacter);
                    }
                } else {
                    current_unicode_code_point <<= 6;
                    current_unicode_code_point += (utf8_byte & 0x3F);
                    if (--current_unicode_remaining_bytes == 0) {
                        //检测非最简小长度的实现
                        if (((current_unicode_total_bytes >= 2) && (current_unicode_code_point < 0x00080))
                            ||((current_unicode_total_bytes >= 3) && (current_unicode_code_point < 0x00800))
                            ||((current_unicode_total_bytes >= 4) && (current_unicode_code_point < 0x10000))
                        ) {
                            tlsm_is_valid = false;
                            unicode_output.push_back(kUnicodeReplacementCharacter);
                        } else {
                            unicode_output.push_back(current_unicode_code_point);
                        }
                        current_unicode_code_point = 0;
                    }
                }
            }
            return unicode_output;
        }

        static std::vector<UnicodeCodePoint> Decode(const std::string& utf8_string)
        {
            return Decode(std::vector<uint8_t>(utf8_string.begin(), utf8_string.end()));
        }

        static bool IsValidEncoding(const std::string& encoding) 
        {
            Decode(encoding);
            return (tlsm_is_valid && !current_unicode_remaining_bytes);
        }
    };
}


#undef REPLACEMENT_INVALID_CODE_POINT





