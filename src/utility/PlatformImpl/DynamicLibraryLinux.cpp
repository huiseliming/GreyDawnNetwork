#include "utility/DynamicLibrary.h"
#include <dlfcn.h>
#include <sys/param.h>
#include <unistd.h>

namespace GreyDawn
{

   bool DynamicLibrary::Load(const std::string& path, const std::string& name) 
   {
       Unload();
       std::string original_path = GetExecuteFileDirectoryAbsolutePath();
       (void)chdir(path.c_str());
       const auto library_path = fmt::format("{}/lib{}.so", path.c_str(), name.c_str());
       library_handle = dlopen(library_path.c_str(), RTLD_NOW);
       (void)chdir(original_path.c_str());
       return (library_handle != NULL);
    }

    void DynamicLibrary::Unload() 
    {
        if (library_handle != NULL) {
            (void)dlclose(library_handle);
            library_handle = NULL;
        }
    }

    void* DynamicLibrary::GetProcedure(const std::string& name) 
    {
        return dlsym(library_handle, name.c_str());
    }


}
























