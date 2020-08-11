#pragma once
#include <string>

namespace GreyDawn
{
	
	std::string& GetPathSeparators();
	/**
		* @brief Get the absolute path of the executable file directory
		*
		* @return  If failed return empty string else return the path
		*
		* @note
		*/
	std::string GetExecuteFileDirectoryAbsolutePath();
	std::string GetExecuteFileAbsolutePath();

	std::string TranslateErrorCode(uint32_t error_code);
}








