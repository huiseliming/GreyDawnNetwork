#include "Utility.h"
#include <fstream>
#include <optional>

using namespace GreyDawn;
namespace GreyDawn
{
	bool ParseJsonStringToJsonValue(std::string json_string, Json::Value& json_value)
	{
		thread_local JSONCPP_STRING jsoncpp_error;
		thread_local Json::CharReaderBuilder builder; 
		Json::Value root; 
		const std::unique_ptr<Json::CharReader> reader(builder.newCharReader()); 
		if (!reader->parse(json_string.data(), json_string.data() + json_string.size(), &json_value, &jsoncpp_error))
		{
			GD_LOG_ERROR("<ParseJsonStringError>{}", jsoncpp_error);
			return false;
		}
		return true;
	}
	template<typename T>
	std::optional<T> GetJsonArrayField(Json::Value& json_value, std::string_view field)
	{
		std::optional<T> opt;
		if (!json_value[field.data()].is<T>())
		T = json_value[field.data()];
		return opt;
	}


	std::optional<std::string> GetJsonStringField(Json::Value& json_value, std::string_view field)
	{
		std::optional<std::string> opt;
		if (!json_value[field.data()].isString())
			opt = json_value[field.data()].asString();
		return opt;
	}



	bool GreyDawn::ReadJsonFile(std::string file_path, Json::Value& json_value)
	{
		std::ifstream ifs;
		ifs.open(file_path);
		if (!ifs.is_open())
		{
			GD_LOG_ERROR("ifstream Can't Open {}", file_path);
			return false;
		}
		thread_local Json::CharReaderBuilder builder;
		thread_local JSONCPP_STRING jsoncpp_error;
		if (!parseFromStream(builder, ifs, &json_value, &jsoncpp_error)) {
			GD_LOG_ERROR("<ParseJsonStreamError>{}", jsoncpp_error);
			return false;
		}
		return true;
	}

	bool GreyDawn::WriteJsonFile(std::string file_path, Json::Value& json_value)
	{
		std::ofstream ofs;
		ofs.open(file_path, std::ios::ate | std::ios::out);
		if (!ofs.is_open())
		{
			GD_LOG_ERROR("ofstream Can't Open {}", file_path);
			return false;
		}
		Json::StreamWriterBuilder builder;
		const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(json_value, &ofs);
		return true;
	}
}
