cmake_minimum_required(VERSION 3.12)
set(This SumDynamicLibrary)

FILE(GLOB_RECURSE Sources  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${This} SHARED ${Sources})
