#pragma once
#include <json/json.h>
#include <spdlog/spdlog.h>
#include "Logger.h"
#include "Exception.h"
#include "ThreadPool.h"


namespace GreyDawn
{
	//JsonString Convert To JsonObject
	bool ParseJsonStringToJsonValue(std::string json_string, Json::Value& json_value);

	bool JsonStringFieldExist(Json::Value& json_value,std::string_view field);

	bool ReadJsonFile(std::string file_path, Json::Value& json_value);

	bool WriteJsonFile(std::string file_path, Json::Value& json_value);
}






