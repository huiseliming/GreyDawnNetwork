#pragma once
#include <optional>
#include <json/json.h>
#include <spdlog/spdlog.h>
#include "Utf8.h"
#include "Logger.h"
#include "SystemErrorException.h"
#include "Singleton.h"
#include "ThreadPool.h"
#include "DynamicLibrary.h"
#include "SystemAbstraction.h"
#include "Subprocess.h"
#include "DirectoryMonitor.h"
#include "PipeSignal.h"
#include "Service.h"

namespace GreyDawn
{
    namespace Utility
    {
        //JsonString Convert To JsonObject
        bool ParseJsonStringToJsonValue(std::string json_string, Json::Value& json_value);

        template<typename T>
        std::optional<T> GetJsonField(Json::Value& json_value, std::string_view field)
        {
            std::optional<T> opt;
            if (json_value[field.data()].is<T>())
                opt = json_value[field.data()].as<T>();
            return opt;
        }

        bool ReadJsonFile(std::string file_path, Json::Value& json_value);

        bool WriteJsonFile(std::string file_path, Json::Value& json_value);
    }
}






