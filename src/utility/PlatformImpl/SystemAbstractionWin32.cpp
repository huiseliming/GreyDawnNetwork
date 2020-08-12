#include "utility/SystemAbstraction.h"
#include <Windows.h>
#include <vector>
#include "utility/Logger.h"

namespace GreyDawn
{
    std::string& GetPathSeparators()
    {
        static std::string path_separators = "\\";
        return path_separators;
    }

    std::string GetExecuteFileDirectoryAbsolutePath()
    {
        std::string execute_file_directory_path;
        std::string execute_file_path = GetExecuteFileAbsolutePath();
        if (!execute_file_path.empty())
        {
            std::size_t pos = execute_file_path.rfind(GetPathSeparators());
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
        DWORD path_length = GetModuleFileNameA(NULL, &current_absolute_path[0], MAX_PATH);
        if (path_length == 0) {
            DWORD error_code = GetLastError();
            GD_LOG_ERROR("[ErrorCode:{:d},ErrorFormat:{}]", error_code, TranslateErrorCode(error_code));
        }
        else if (path_length >= MAX_PATH) {
            int max_path = MAX_PATH;
            for (size_t i = 0; i < 4; i++)
            {
                max_path = max_path * 2;
                current_absolute_path.resize(max_path);
                path_length = GetModuleFileNameA(NULL, &current_absolute_path[0], MAX_PATH);
                if (path_length < 0) {
                    GD_LOG_ERROR("[errno:{:d},strerror:{}]", errno, strerror(errno));
                }
                else if (path_length >= max_path) {

                }
                else {
                    current_absolute_path[path_length] = '\0';
                    absolute_path = &current_absolute_path[0];
                    return absolute_path;
                }
            }
            GD_LOG_ERROR("[path length exceeds {:d}]", max_path);
        }
        else {
            current_absolute_path[path_length] = '\0';
            absolute_path = &current_absolute_path[0];
            return absolute_path;
        }
        return absolute_path;
    }

    const char* TranslateErrorCode(uint32_t error_code)
    {
        char* message_buffer = nullptr;
        //获取格式化错误
        const DWORD message_len = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&message_buffer), 0, nullptr
            );
        //长度小于0说明未获取到格式化的错误
        if (message_len <= 0)
            return "Unidentified error code";
        //拷贝字符串
        thread_local static std::string error_message = message_buffer;
        //这个字符串是Windows的系统内存，归还系统
        LocalFree(message_buffer);
        return error_message.c_str();
    }
}























