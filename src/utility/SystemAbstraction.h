#pragma once
#ifdef  _WIN32
#include <Windows.h>
#endif //  _WIN32


#include <string>

namespace GreyDawn
{
    
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
    /**
     * Get the absolute path of the executable file directory
     *
     * @param[in] error_code
     *     The error code
     *
     * @return 
     *     The translated string (node: return value is thread_local local static)
     */
    std::string& TranslateErrorCode(DWORD error_code);

}








