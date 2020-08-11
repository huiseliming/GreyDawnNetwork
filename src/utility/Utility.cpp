#include "Utility.h"
#include <fstream>
#ifdef __linux__
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#endif


namespace GreyDawn
{
namespace Utility
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

	bool ReadJsonFile(std::string file_path, Json::Value& json_value)
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

	bool WriteJsonFile(std::string file_path, Json::Value& json_value)
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
	std::string GetExecuteFileDirectoryAbsolutePath()
	{
		std::string execute_file_directory_path;
		std::string execute_file_path = GetExecuteFileAbsolutePath(); 
		if (!execute_file_path.empty())
		{
			std::size_t pos =  execute_file_path.rfind('/');
			if(pos != std::string::npos)
			{
				execute_file_directory_path = execute_file_path.substr(0ul , pos);
			}
		}
		return execute_file_directory_path;
	}

	std::string GetExecuteFileAbsolutePath()
	{
		std::string absolute_path;
		std::vector<char> current_absolute_path(PATH_MAX);
		int cnt = readlink("/proc/self/exe", &current_absolute_path[0], PATH_MAX);
		if(cnt < 0){
			GD_LOG_ERROR("[errno:{:d},strerror:{}]",errno,strerror(errno));
		}else if(cnt >= PATH_MAX){
			int max_path = PATH_MAX;
			for (size_t i = 0; i < 4; i++)
			{
				max_path = max_path * 2;
				current_absolute_path.resize(max_path);
				cnt = readlink("/proc/self/exe", &current_absolute_path[0], max_path);
				if(cnt < 0){
					GD_LOG_ERROR("[errno:{:d},strerror:{}]",errno,strerror(errno));
				}else if(cnt >= PATH_MAX){

				}else{
					current_absolute_path[cnt] = '\0';
					absolute_path = &current_absolute_path[0]; 
					return absolute_path;
				}
			}
			GD_LOG_ERROR("[path length exceeds {:d}]",max_path);
		} else {
			current_absolute_path[cnt] = '\0';
			absolute_path = &current_absolute_path[0]; 
			return absolute_path;
		}
		return absolute_path;
	}
}
}
