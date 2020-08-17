#include "utility/SystemAbstraction.h"
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <vector>
#include "utility/Logger.h"

namespace GreyDawn
{
    std::string TranslateErrorCode(int error_code)
    {
        return strerror(error_code);
    }

    std::string& GetPathSeparators()
    {
        static std::string path_separators = "/";
        return path_separators;
    }

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
        std::vector<char> current_absolute_path(PATH_MAX);
        int cnt = readlink("/proc/self/exe", &current_absolute_path[0], PATH_MAX);
        if (cnt < 0) {
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        } else if (cnt >= PATH_MAX) {
            int max_path = PATH_MAX;
            for (size_t i = 0; i < 4; i++)
            {
                max_path = max_path * 2;
                current_absolute_path.resize(max_path);
                cnt = readlink("/proc/self/exe", &current_absolute_path[0], max_path);
                if (cnt < 0) {
                    GD_LOG_OUTPUT_SYSTEM_ERROR();
                } else if (cnt >= max_path) {

                } else {
                    current_absolute_path[cnt] = '\0';
                    absolute_path = &current_absolute_path[0];
                    return absolute_path;
                }
            }
            GD_LOG_ERROR("[path length exceeds {:d}]", max_path);
        } else {
            current_absolute_path[cnt] = '\0';
            absolute_path = &current_absolute_path[0];
            return absolute_path;
        }
        return absolute_path;
    }
}
























