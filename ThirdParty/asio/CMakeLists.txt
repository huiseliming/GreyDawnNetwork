cmake_minimum_required(VERSION 3.12)

set(This asio)
project(${This})

find_package(OpenSSL REQUIRED)

FILE(GLOB_RECURSE HeaderFiles  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h) #��ȡ��ǰĿ¼ͷ�ļ�
FILE(GLOB_RECURSE SourceFiles  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp) #��ȡ��ǰĿ¼Դ�ļ�

add_library(${This} STATIC ${SourceFiles} ${HeaderFiles})


target_include_directories(${This} PUBLIC include)

target_link_libraries(${This} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)

add_definitions(-DASIO_SEPARATE_COMPILATION)
