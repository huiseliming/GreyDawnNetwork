cmake_minimum_required(VERSION 3.12)

set(This GreyDawnGlobal)
project(${This})

FILE(GLOB_RECURSE HeaderFiles  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
FILE(GLOB_RECURSE SourceFiles  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${This} SHARED ${SourceFiles} ${HeaderFiles}) 

target_include_directories(${This} PUBLIC include)

set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)