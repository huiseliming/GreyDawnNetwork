#include "DynamicLibrary.h"


namespace GreyDawn
{

    DynamicLibrary::DynamicLibrary(const std::string& path, const std::string& name)
    {
        Load(path, name);
    }
    DynamicLibrary::~DynamicLibrary()
    {
        Unload();
    }
    DynamicLibrary::DynamicLibrary(DynamicLibrary&& dynamic_library)
    {
        this->library_handle_ = dynamic_library.library_handle_;
        dynamic_library.library_handle_ = nullptr;
    }
    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& dynamic_library)
    {
        if (this != std::addressof(dynamic_library))
        {
            this->library_handle_ = dynamic_library.library_handle_;
            dynamic_library.library_handle_ = nullptr;
        }
        return *this;
    }
}






























