#pragma once




namespace GreyDawn
{
    class Utf8
    {
    public:
        static std::vector<uint32_t> AsciiToUnicode(const std::string& ascii)
        {
            return std::vector<uint32_t>(ascii.begin(),ascii.end());
        }

        static std::vector<uint8_t> Encode(std::vector<uint32_t> unicode)
        {
            std::vector<uint8_t> utf8_encode;
            return utf8_encode;
        }
    };
}









