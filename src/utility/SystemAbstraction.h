#pragma once
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

}








