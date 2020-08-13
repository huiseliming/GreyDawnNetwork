#pragma once
#include <array>
#include <vector>

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
        static const std::array< uint8_t , 3> kUtf8BytesReplacementCharacter;
        static const UnicodeCodePoint kUnicodeReplacementCharacter;
        static const UnicodeCodePoint kReserveRangeBegin;
        static const UnicodeCodePoint kReserveRangeEnd;
        thread_local static UnicodeCodePoint current_unicode_code_point;
        thread_local static size_t current_unicode_remaining_bytes;
        thread_local static size_t current_unicode_total_bytes;
        thread_local static bool tlsm_is_valid;
    public:
        static std::vector<UnicodeCodePoint> AsciiToUnicode(const std::string& ascii);
        static std::vector<uint8_t> Encode(const std::vector<UnicodeCodePoint> code_points);
        static std::vector<UnicodeCodePoint> Decode(const std::vector<uint8_t>& utf8_bytes);
        static std::vector<UnicodeCodePoint> Decode(const std::string& utf8_string);
        static bool IsValidEncoding(const std::string& encoding) ;
    };
}





