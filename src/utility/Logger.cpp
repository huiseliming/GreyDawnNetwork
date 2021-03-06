﻿#include "Logger.h"
#include <spdlog/cfg/env.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <filesystem>

namespace GreyDawn
{
    std::shared_ptr<spdlog::logger> default_logger()
    {
        static std::function<std::shared_ptr<spdlog::logger>()> init = []
        {
            std::filesystem::path current_path = std::filesystem::current_path();
            std::filesystem::current_path(GetExecuteFileDirectoryAbsolutePath());
            spdlog::init_thread_pool(8192, 1);
            std::vector<spdlog::sink_ptr> sinks;
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
            sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily_logger_mt.log", 23, 59));
            std::filesystem::current_path(current_path);
            //auto defaultLogger = std::make_shared<spdlog::logger>("default",std::begin(sinks),std::end(sinks));
            return std::make_shared<spdlog::async_logger>("default_logger", std::begin(sinks), std::end(sinks), spdlog::thread_pool());
        };
        static std::shared_ptr<spdlog::logger> default_logger = init();
        return default_logger;
    }

}