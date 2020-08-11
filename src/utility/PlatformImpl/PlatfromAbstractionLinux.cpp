#include "../PlatfromAbstraction.h"
#include <unistd.h>
#include <limits.h>
#include <errno.h>

namespace GreyDawn
{
	std::string GetExecuteFileDirectoryAbsolutePath()
	{
		std::string execute_file_directory_path;
		std::string execute_file_path = GetExecuteFileAbsolutePath();
		if (!execute_file_path.empty())
		{
			std::size_t pos = execute_file_path.rfind('/');
			if (pos != std::string::npos)
			{
				execute_file_directory_path = execute_file_path.substr(0ul, pos);
			}
		}
		return execute_file_directory_path;
	}

	std::string GetExecuteFileAbsolutePath()
	{
		std::string absolute_path;
		std::vector<char> current_absolute_path(MAX_PATH);
		int cnt = readlink("/proc/self/exe", &current_absolute_path[0], MAX_PATH);
		if (cnt < 0) {
			GD_LOG_ERROR("[errno:{:d},strerror:{}]", errno, strerror(errno));
		}
		else if (cnt >= MAX_PATH) {
			int max_path = MAX_PATH;
			for (size_t i = 0; i < 4; i++)
			{
				max_path = max_path * 2;
				current_absolute_path.resize(max_path);
				cnt = readlink("/proc/self/exe", &current_absolute_path[0], max_path);
				if (cnt < 0) {
					GD_LOG_ERROR("[errno:{:d},strerror:{}]", errno, strerror(errno));
				}
				else if (cnt >= max_path) {

				}
				else {
					current_absolute_path[cnt] = '\0';
					absolute_path = &current_absolute_path[0];
					return absolute_path;
				}
			}
			GD_LOG_ERROR("[path length exceeds {:d}]", max_path);
		}
		else {
			current_absolute_path[cnt] = '\0';
			absolute_path = &current_absolute_path[0];
			return absolute_path;
		}
		return absolute_path;
	}

}
























