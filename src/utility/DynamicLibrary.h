#pragma once 
#ifdef _WIN32
#include <Windows.h>
#endif
#include <vector>
#include "Utility.h"

namespace GreyDawn
{
    class DynamicLibrary
    {
    public: 
    DynamicLibrary() = default;
    DynamicLibrary(const std::string& path, const std::string& name);
    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary(DynamicLibrary&& dynamic_library);
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(DynamicLibrary&& dynamic_library);
    ~DynamicLibrary();

    /**
     * Load dynamic library by path and library name
     *
     * @param[in] path 
     *     library path
     * 
     * @param[in] name
     *     library name
     * 
     * @return 
     *     Return true if the dynamic library is successfully loaded, otherwise false
     */
    bool Load(const std::string& path, const std::string& name);

    /**
     * Unload
     */
    void Unload();

    /**
     * Get procedure addr pointer by procedure name 
     *
     * @param[in] name 
     *     procedure name 
     * 
     * @return 
     *     procedure addr pointer
     */
    void* GetProcedure(const std::string& name);
#ifdef _WIN32
    HMODULE library_handle_ = nullptr;
#elif __linux__
    void* library_handle_ = nullptr;
#else
#error "Unknown System"
#endif 
    };

}


















