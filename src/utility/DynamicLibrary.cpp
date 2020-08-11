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
        this->library_handle = dynamic_library.library_handle;
        dynamic_library.library_handle = nullptr;
    }
    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& dynamic_library)
    {
        this->library_handle = dynamic_library.library_handle;
        dynamic_library.library_handle = nullptr;
        return *this;
    }
}






























