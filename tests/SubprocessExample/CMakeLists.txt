cmake_minimum_required(VERSION 3.12)
set(This SubprocessExample)

FILE(GLOB_RECURSE ServiceTestSourceFiles  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${This}
    ${ServiceTestSourceFiles} 
    )

#target_include_directories(${This} PRIVATE ..)
target_include_directories(${This} PRIVATE ../../src)

target_link_libraries(${This} PUBLIC
    ${DependentLibraries}
    GreyDawnNetWork
    GTest::gtest_main
)

add_test(
    NAME ${This}
    COMMAND ${This}
)

