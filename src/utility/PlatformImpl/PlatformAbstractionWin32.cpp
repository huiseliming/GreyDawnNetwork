#include "../PlatformAbstraction.h"
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

	std::string TranslateErrorCode(uint32_t error_code)
	{
		char* message_buffer = nullptr;
		//��ȡ��ʽ������
		const DWORD nMsgLen = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&message_buffer), 0, nullptr
			);
		//����С��0˵��δ��ȡ����ʽ���Ĵ���
		if (nMsgLen <= 0)
			return "Unidentified error code";
		// �����ַ���
		std::string errorMsg = message_buffer;
		// ����ַ�����Windows��ϵͳ�ڴ棬�黹ϵͳ
		LocalFree(message_buffer);
		return errorMsg;
	}
}























