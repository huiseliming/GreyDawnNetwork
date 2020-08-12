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

        static std::vector<uint8_t> Encode(const std::string& )
        {

        }
    };
}









