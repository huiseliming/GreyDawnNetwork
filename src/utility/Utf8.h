#pragma once
#include <stddef.h>



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
        static std::vector<UnicodeCodePoint> AsciiToUnicode(const std::string& ascii)
        {
            return std::vector<uint32_t>(ascii.begin(),ascii.end());
        }

        static std::vector<uint8_t> Encode(std::vector<UnicodeCodePoint> code_points)
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
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x0F) + 0xE0));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6)  & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(( code_point & 0x3F) + 0x80));
                } else if (code_point < 0x2000000) {
                    utf8_bytes.push_back(uint8_t(((code_point >> 18) & 0x07) + 0xF0));
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                } else if (code_point < 0x40000000) {
                    utf8_bytes.push_back(uint8_t(((code_point >> 24) & 0x03) + 0xF8));
                    utf8_bytes.push_back(uint8_t(((code_point >> 18) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                }else{
                    utf8_bytes.push_back(uint8_t(((code_point >> 30) & 0x01) + 0xFC));
                    utf8_bytes.push_back(uint8_t(((code_point >> 24) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 18) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 12) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t(((code_point >> 6) & 0x3F) + 0x80));
                    utf8_bytes.push_back(uint8_t((code_point & 0x3F) + 0x80));
                }
            }
            return utf8_bytes;
        }
    };
}









