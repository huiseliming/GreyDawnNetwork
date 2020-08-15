#pragma once
#include <spdlog/spdlog.h>
#include "SystemAbstraction.h"

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)

#define GD_LOG_TRACE(fmt,...)       GreyDawn::default_logger()->trace   (__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)
#define GD_LOG_DEBUG(fmt, ...)      GreyDawn::default_logger()->debug   (__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)
#define GD_LOG_INFO(fmt, ...)       GreyDawn::default_logger()->info    (fmt, ##__VA_ARGS__)
#define GD_LOG_WARN(fmt, ...)       GreyDawn::default_logger()->warn    (fmt, ##__VA_ARGS__)
#define GD_LOG_ERROR(fmt, ...)      GreyDawn::default_logger()->error   (__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)
#define GD_LOG_CRITICAL(fmt, ...)   GreyDawn::default_logger()->critical(__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)

#ifdef _WIN32
#define GD_LOG_OUTPUT_SYSTEM_ERROR()   GreyDawn::default_logger()->error(__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" "[LastError>{}]",TranslateErrorCode(GetLastError()))
#elif __linux__
#include <errno.h>
#define GD_LOG_OUTPUT_SYSTEM_ERROR()   GreyDawn::default_logger()->error(__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" "[errno>{:d} | strerror>{}]",errno,strerror(errno))
#else
#endif

namespace GreyDawn
{
    std::shared_ptr<spdlog::logger> default_logger();
}






















