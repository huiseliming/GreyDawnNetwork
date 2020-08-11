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
        DynamicLibrary(const DynamicLibrary& thread_pool) = delete;
        DynamicLibrary(DynamicLibrary&& dynamic_library);
        DynamicLibrary& operator=(const DynamicLibrary& thread_pool) = delete;
        DynamicLibrary& operator=(DynamicLibrary&& dynamic_library);
        ~DynamicLibrary();

        /**
		 * @brief Load dynamic library by path and library name
		 *
         * @param path path
         * 
         * @param name library name
         * 
		 * @return Return true if the dynamic library is successfully loaded, otherwise false
		 *
		 * @note
		 */
        bool Load(const std::string& path, const std::string& name);

         /**
		 * @brief Unload
         * 
		 * @note
		 */
        void Unload();

        /**
		 * @brief Get procedure addr pointer by procedure name 
		 *
         * @param name procedure name 
         * 
		 * @return procedure addr pointer
		 *
		 * @note
		 */
        void* GetProcedure(const std::string& name);
        
#ifdef _WIN32
        HMODULE library_handle = nullptr;
#elif __linux__
        void* library_handle = nullptr;
#else
#error "Unknown System"
#endif 
    };

}


















