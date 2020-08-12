#include "utility/DynamicLibrary.h"
#include <dlfcn.h>
#include <sys/param.h>
#include <unistd.h>

namespace GreyDawn
{

   bool DynamicLibrary::Load(const std::string& path, const std::string& name) 
   {
        Unload();
        // std::string original_path = GetExecuteFileDirectoryAbsolutePath();
        // if(chdir(path.c_str()) < 0){
        //     GD_LOG_ERROR("[errno:{:d},strerror:{}]", errno, strerror(errno));
        //     return false;
        // }
        const auto library_path = fmt::format("{}/lib{}.so", path.c_str(), name.c_str());
        library_handle = dlopen(library_path.c_str(), RTLD_NOW);
        if(!library_handle)
            GD_LOG_ERROR("[dlopen dlerror>{}]", dlerror());
        // if(chdir(original_path.c_str()) < 0){
        //     GD_LOG_ERROR("[errno:{:d},strerror:{}]", errno, strerror(errno));
        //     return false;
        // }
        return (library_handle != NULL);
    }

    void DynamicLibrary::Unload() 
    {
        if (library_handle != NULL) {
            if(dlclose(library_handle))
                GD_LOG_ERROR("[dlclose dlerror>{}]", dlerror());
            library_handle = nullptr;
        }
    }

    void* DynamicLibrary::GetProcedure(const std::string& name) 
    {
        return dlsym(library_handle, name.c_str());
    }
}
























