#pragma once
#include <exception>
#include <string>
#include <sstream>

#define EXCEPT(...) GreyDawn::Exception(__LINE__ , __FILE__, ##__VA_ARGS__)
#define THROW_EXCEPT(...) throw EXCEPT(##__VA_ARGS__)

namespace GreyDawn
{
    class Exception : public std::exception
    {
    public:
        Exception(int line, const char* file) noexcept :
            line_(line), file_(file)
        {}
        explicit Exception(int line, const char* file, char const* const message) noexcept :
            line_(line), file_(file), message_(message)
        {}
        const char* what() const noexcept override
        {
            std::ostringstream oss;
            oss << "[ExceptionType]: " << GetType() << std::endl
                << "[  Description]: " << message_ << std::endl
                << "[       Origin]: " << GetOriginString() << std::endl;
            what_buffer_ = oss.str();
            return what_buffer_.c_str();
        }
        virtual const char* GetType() const noexcept
        {
            return "Exception";
        }
        int GetLine() const noexcept
        {
            return line_;
        }
        const std::string& GetFile() const noexcept
        {
            return file_;
        }
        std::string GetOriginString() const noexcept
        {
            std::ostringstream oss;
            oss << file_ << "(" << line_ << ")";
            return oss.str();
        }
    protected:
        int line_;
        std::string file_;
        std::string message_;
        mutable std::string what_buffer_;
    };
}