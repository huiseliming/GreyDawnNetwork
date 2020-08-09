#pragma once 

#ifdef _WIN32
#include <vector>
#include <Windows.h>
#endif

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

#ifdef _WIN32
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
#elif __linux__
    bool load(const std::string& path, const std::string& name) {
        Unload();
        std::vector< char > originalPath(MAXPATHLEN);
        (void)getcwd(&originalPath[0], originalPath.size());
        (void)chdir(path.c_str());
        const auto library = StringExtensions::sprintf(
            "%s/lib%s.%s",
            path.c_str(),
            name.c_str(),
            DynamicLibraryImpl::GetDynamicLibraryFileExtension().c_str()
            );
        impl_->libraryHandle = dlopen(library.c_str(), RTLD_NOW);
        (void)chdir(&originalPath[0]);
        return (impl_->libraryHandle != NULL);
}

    void unload() {
        if (impl_->libraryHandle != NULL) {
            (void)dlclose(impl_->libraryHandle);
            impl_->libraryHandle = NULL;
        }
    }

    void* getProcedure(const std::string& name) {
        return dlsym(impl_->libraryHandle, name.c_str());
    }
#else
#   error "Unknown compiler"
#endif 


};


}


















