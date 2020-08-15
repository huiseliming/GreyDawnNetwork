#include "utility/DynamicLibrary.h"
#include "utility/SystemAbstraction.h"
namespace GreyDawn
{

    bool DynamicLibrary::Load(const std::string& path, const std::string& name)
    {
        Unload();
        //保存当前路径,设置当前路径到动态库路径,防止动态库加载依赖文件路径
        std::vector< char > original_path(MAX_PATH);
        DWORD length = GetCurrentDirectoryA((DWORD)original_path.size(), &original_path[0]);
        if (length == 0) {
            GD_LOG_OUTPUT_SYSTEM_ERROR();
            return false;
        } else if (length > MAX_PATH) {
            original_path.resize(length);
            DWORD length = GetCurrentDirectoryA((DWORD)original_path.size(), &original_path[0]);
        }
        if(!SetCurrentDirectoryA(path.c_str()))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        const auto library = fmt::format("{}/{}.dll", path.c_str(), name.c_str());
        library_handle = LoadLibraryA(library.c_str());
        if (!library_handle)
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        if (!SetCurrentDirectoryA(&original_path[0])) {
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        }
        return (library_handle != NULL);
    }

    void DynamicLibrary::Unload()
    {
        if (library_handle != nullptr) {
            BOOL error_code = FreeLibrary(library_handle);
            if (!error_code)
                GD_LOG_OUTPUT_SYSTEM_ERROR();
            library_handle = nullptr;
        }
    }

    void* DynamicLibrary::GetProcedure(const std::string& name) 
    {
        return GetProcAddress(library_handle, name.c_str());
    }
}
