#pragma once 
#if defined(_MSC_VER) && defined(_WIN32)
#include <Windows.h>
#elif defined(__GNUC__) && (defined(__linux__) || defined(__MINGW64__) || defined(__MINGW32__) )
#include <dlfcn.h>
#include <sys/param.h>
#include <unistd.h>
#endif
#include <vector>

namespace GreyDawn
{
class DynamicLibrary
{
public: 
    DynamicLibrary() = default;
    DynamicLibrary(const std::string& path, const std::string& name)
    {
        Load(path, name);
    }
    ~DynamicLibrary()
    {
        Unload();
    }
    DynamicLibrary(const DynamicLibrary& thread_pool) = delete;
    DynamicLibrary(DynamicLibrary&& dynamic_library)
    {
        this->library_handle = dynamic_library.library_handle;
        dynamic_library.library_handle = nullptr;
    }
    DynamicLibrary& operator=(const DynamicLibrary& thread_pool) = delete;
    DynamicLibrary& operator=(DynamicLibrary&& dynamic_library)
    {
        this->library_handle = dynamic_library.library_handle;
        dynamic_library.library_handle = nullptr;
    }

#if defined(_MSC_VER) && defined(_WIN32)
    bool Load(const std::string& path, const std::string& name)
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
    void Unload()
    {
        if (library_handle != nullptr) {
            (void)FreeLibrary(library_handle);
            library_handle = nullptr;
        }
    }
    void* GetProcedure(const std::string& name)
    {
        return GetProcAddress(library_handle, name.c_str());
    }
    HMODULE library_handle = nullptr;
#elif defined(__GNUC__) && (defined(__linux__) || defined(__MINGW64__) || defined(__MINGW32__) )
//    bool Load(const std::string& path, const std::string& name) {
//        Unload();
//        std::vector< char > original_path(MAXPATHLEN);
//        (void)getcwd(&originalPath[0], originalPath.size());
//        (void)chdir(path.c_str());
//        const auto library = fmt::format(
//            "{}/{}.so",
//            path.c_str(),
//            name.c_str(),
//            );
//        library_handle = dlopen(library.c_str(), RTLD_NOW);
//        (void)chdir(&original_path[0]);
//        return (library_handle != NULL);
//}
//
//    void Unload() {
//        if (library_handle != NULL) {
//            (void)dlclose(library_handle);
//            library_handle = NULL;
//        }
//    }
//
//    void* GetProcedure(const std::string& name) {
//        return dlsym(library_handle, name.c_str());
//    }
//    void* library_handle = nullptr;
#else
#   error "Unknown Compiler"
#endif 


};


}


















