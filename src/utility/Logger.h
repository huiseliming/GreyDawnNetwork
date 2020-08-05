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
#include <spdlog/spdlog.h>

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)

#define GD_LOG_TRACE(fmt,...)       GreyDawn::default_logger()->trace   (__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)
#define GD_LOG_DEBUG(fmt, ...)      GreyDawn::default_logger()->debug   (__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)
#define GD_LOG_INFO(fmt, ...)       GreyDawn::default_logger()->info    (fmt, ##__VA_ARGS__)
#define GD_LOG_WARN(fmt, ...)       GreyDawn::default_logger()->warn    (fmt, ##__VA_ARGS__)
#define GD_LOG_ERROR(fmt, ...)      GreyDawn::default_logger()->error   (__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)
#define GD_LOG_CRITICAL(fmt, ...)   GreyDawn::default_logger()->critical(__FILE__ "(" STRINGIFY_BUILTIN(__LINE__) ") :" fmt,##__VA_ARGS__)

namespace GreyDawn
{
    std::shared_ptr<spdlog::logger> default_logger();
}






















