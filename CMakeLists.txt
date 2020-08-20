cmake_minimum_required(VERSION 3.12)

set(This GreyDawnNetWork)
project(${This})
message(STATUS "compiler: " ${CMAKE_CXX_COMPILER_ID})
find_package(GTest CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
#find_package(skyr-url CONFIG REQUIRED)
enable_testing()

if (MSVC)
    # MSVC链接静态库(/MT),替换/MD为/MT来进行链接静态库,使用RUNTIME_STATIC_LINK控制链接静态库还是动态库
    if(MSVC AND RUNTIME_STATIC_LINK)
        foreach(flag
            CMAKE_C_FLAGS
            CMAKE_CXX_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
        )
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
    #关闭警告
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS 
        -D_SCL_SECURE_NO_WARNINGS
        -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
        -D_WIN32_WINDOWS
    )
    #使用Windows共享库,添加这个设置不用使用"__declspec(dllimport)"去导出符号
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 

#build ThirdParty
add_subdirectory(ThirdParty)

#build library
FILE(GLOB_RECURSE SourceFiles  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    )
macro(SetPlatformImplSourceFiles SourceFiles Platform)
    foreach(SourceFile ${SourceFiles})
        if( (NOT (${Platform} STREQUAL "Win32")) AND 
          ( ${SourceFile} MATCHES ".*\\/Win32\\/.*Win32\\.cpp"))
            list(REMOVE_ITEM SourceFiles ${SourceFile})
        endif()
        if( (NOT (${Platform} STREQUAL "Linux")) AND 
          ( ${SourceFile} MATCHES ".*\\/Linux\\/.*Linux\\.cpp"))
            list(REMOVE_ITEM SourceFiles ${SourceFile})
        endif()
    endforeach()
endmacro()

if (MSVC)
    SetPlatformImplSourceFiles("${SourceFiles}" "Win32")
endif()

if (UNIX)
    SetPlatformImplSourceFiles("${SourceFiles}" "Linux")
endif()

add_library(${This} STATIC ${SourceFiles} "src/network/Session.cpp")

target_include_directories(${This} PUBLIC include)
target_include_directories(${This} PRIVATE src)

set(DependentLibraries ${DependentLibraries} jsoncpp_lib)
set(DependentLibraries ${DependentLibraries} asio)
set(DependentLibraries ${DependentLibraries} spdlog::spdlog)

if (UNIX)
    set(DependentLibraries ${DependentLibraries} pthread dl)
endif()
#set(DependentLibraries ${DependentLibraries} skyr::skyr-url)
target_link_libraries(${This} PRIVATE ${DependentLibraries})

#build tests
add_subdirectory(tests)



