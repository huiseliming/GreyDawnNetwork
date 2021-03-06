#include "utility/DynamicLibrary.h"
#include <dlfcn.h>
#include <sys/param.h>
#include <unistd.h>

namespace GreyDawn
{

   bool DynamicLibrary::Load(const std::string& path, const std::string& name) 
   {
        Unload();
        //保存当前路径，设置当前路径到动态库路径，防止动态库加载依赖文件所在路径
        std::string original_path = GetExecuteFileDirectoryAbsolutePath();
        if(chdir(path.c_str()) < 0){
            GD_LOG_OUTPUT_SYSTEM_ERROR();
            return false;
        }
        const auto library_path = fmt::format("{}/lib{}.so", path.c_str(), name.c_str());
        library_handle_ = dlopen(library_path.c_str(), RTLD_NOW);
        if(!library_handle_)
            GD_LOG_ERROR("[dlopen dlerror>{}]", dlerror());
        if(chdir(original_path.c_str()) < 0){
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        }
        return (library_handle_ != NULL);
    }

    void DynamicLibrary::Unload() 
    {
        if (library_handle_ != NULL) {
            if(dlclose(library_handle_))
                GD_LOG_ERROR("[dlclose dlerror>{}]", dlerror());
            library_handle_ = nullptr;
        }
    }

    void* DynamicLibrary::GetProcedure(const std::string& name) 
    {
        return dlsym(library_handle_, name.c_str());
    }
}
























