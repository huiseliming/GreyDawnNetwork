/*
 * MIT License
 *
 * Copyright(c) 2020 DaiMingze
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
			oss << "[ExceptionType]: " << getType() << std::endl
				<< "[  Description]: " << message_ << std::endl
				<< "[       Origin]: " << getOriginString() << std::endl;
			what_buffer_ = oss.str();
			return what_buffer_.c_str();
		}
		virtual const char* getType() const noexcept
		{
			return "Exception";
		}
		int getLine() const noexcept
		{
			return line_;
		}
		const std::string& getFile() const noexcept
		{
			return file_;
		}
		std::string getOriginString() const noexcept
		{
			std::ostringstream oss;
			oss << file_ << "(" << line_ << ")";
			return oss.str();
		}
	private:
		int line_;
		std::string file_;
		std::string message_;
	protected:
		mutable std::string what_buffer_;
	};
}