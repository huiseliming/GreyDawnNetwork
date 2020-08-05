cmake_minimum_required(VERSION 3.12)
set(This tests)

FILE(GLOB_RECURSE TestSourceFiles  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp) #获取当前目录源文件

add_executable(${This}
    ${TestSourceFiles} 
    )

target_include_directories(${This} PRIVATE ..)
target_include_directories(${This} PRIVATE ../src)

target_link_libraries(${This} PUBLIC
    GTest::gtest_main
)

add_test(
    NAME ${This}
    COMMAND ${This}
)


