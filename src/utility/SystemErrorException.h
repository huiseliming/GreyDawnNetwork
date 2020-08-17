#pragma once
#include "Exception.h"
#include "SystemAbstraction.h"
#include <spdlog/spdlog.h>

#define SYSTEM_ERROR_EXCEPT(...) GreyDawn::SystemErrorExpetion(__LINE__ , __FILE__, GetLastError(), ##__VA_ARGS__)
#define THROW_SYSTEM_ERROR_EXCEPT(...) throw SYSTEM_ERROR_EXCEPT(##__VA_ARGS__)
#define THROW_SYSTEM_ERROR_IF_FAILED(FailedExpr) if(FailedExpr) THROW_SYSTEM_ERROR_EXCEPT(#FailedExpr)



namespace GreyDawn 
{
class SystemErrorExpetion :public Exception
{
public:
	SystemErrorExpetion(int line, const char* file, uint32_t error_code, std::string failed_expr = "Unknow") noexcept
		: Exception(line, file, TranslateErrorCode(error_code).c_str())
		, failed_expr_(failed_expr)
	{
	}
	const char* what() const noexcept override
	{
		std::ostringstream oss;
		oss << "[ExceptionType]: " << GetType() << std::endl
			<< "[   FailedExpr]: " << failed_expr_ << std::endl
			<< "[  Description]: " << "(" << fmt::format("{0:#X}", error_code_) << ")" << message_ << std::endl
			<< "[       Origin]: " << GetOriginString() << std::endl;
		what_buffer_ = oss.str();
		return what_buffer_.c_str();
	}
	const char* GetType() const noexcept
	{
		return "SystemErrorException";
	}
private:
	uint32_t error_code_;
	std::string failed_expr_;
};
}















