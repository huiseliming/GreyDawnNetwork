#include "../DynamicLibrary.h"

namespace GreyDawn
{

    bool DynamicLibrary::Load(const std::string& path, const std::string& name)
    {
        Unload();
        std::vector< char > original_path(MAX_PATH);
        (void)GetCurrentDirectoryA((DWORD)original_path.size(), &original_path[0]);
        (void)SetCurrentDirectoryA(path.c_str());
        const auto library = fmt::format("{}/{}.dll", path.c_str(), name.c_str());
        library_handle = LoadLibraryA(library.c_str());
        (void)SetCurrentDirectoryA(&original_path[0]);
        return (library_handle != NULL);
    }

    void DynamicLibrary::Unload()
    {
        if (library_handle != nullptr) {
            (void)FreeLibrary(library_handle);
            library_handle = nullptr;
        }
    }

    void* DynamicLibrary::GetProcedure(const std::string& name) 
    {
        return GetProcAddress(library_handle, name.c_str());
    }
}























