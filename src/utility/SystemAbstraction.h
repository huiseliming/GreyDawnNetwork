#pragma once
#include <string>
#ifdef  _WIN32
#include <Windows.h>
#endif

namespace GreyDawn
{
        /**
     * Get the absolute path of the executable file directory
     *
     * @param[in] error_code
     *     The error code
     *
     * @return
     *     The translated string 
     */
#ifdef  _WIN32
    std::string TranslateErrorCode(DWORD error_code);
#elif __linux__
    std::string TranslateErrorCode(int error_code);
#endif
    /**
     * Get current operating system path separators
     *
     * @return  Path separators
     */
    std::string& GetPathSeparators();

    /**
     * Get the absolute path of the executable file directory
     *
     * @return  
     *     If failed return empty string else return the path
     */
    std::string GetExecuteFileDirectoryAbsolutePath();

    /**
     * Get the absolute path of the executable file
     *
     * @return  If failed return empty string else return the path
     */
    std::string GetExecuteFileAbsolutePath();

}








