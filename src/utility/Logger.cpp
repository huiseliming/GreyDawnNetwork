#include "Logger.h"
#include <spdlog/cfg/env.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>

namespace GreyDawn
{
    std::shared_ptr<spdlog::logger> default_logger()
    {
        static std::shared_ptr<spdlog::logger> default_Logger;
        spdlog::init_thread_pool(8192, 1);
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily_logger_mt.log", 23, 59));
        //auto defaultLogger = std::make_shared<spdlog::logger>("default",std::begin(sinks),std::end(sinks));
        default_Logger = std::make_shared<spdlog::async_logger>("default_Logger", std::begin(sinks), std::end(sinks), spdlog::thread_pool());
        //spdlog::register_logger(default_Logger);
        //spdlog::set_default_logger(default_Logger);
        return default_Logger;
    }

}