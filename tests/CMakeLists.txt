cmake_minimum_required(VERSION 3.12)
set(This tests)

FILE(GLOB_RECURSE TestSourceFiles  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp) #��ȡ��ǰĿ¼Դ�ļ�

add_executable(${This}
    ${TestSourceFiles} 
    )

#target_include_directories(${This} PRIVATE ..)
target_include_directories(${This} PRIVATE ../src)

target_link_libraries(${This} PUBLIC
    ${DependentLibraries}
    GreyDawnNetWork
    GTest::gtest_main
)

add_subdirectory(Service)
add_subdirectory(SumDynamicLibrary)

add_test(
    NAME ${This}
    COMMAND ${This}
)


