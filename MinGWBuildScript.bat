@echo off
cd /d %~dp0
if not exist build ( mkdir build )
cd build
rem Generate VS2019 project.
cmake -DCMAKE_TOOLCHAIN_FILE=F:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCFLAGS=-m64 -DCFLAGS=-m64 -G "MinGW Makefiles" .. 
pause
 cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ../.. "-DCMAKE_TOOLCHAIN_FILE=E://vcpkg//scripts//buildsystems//vcpkg.cmake"




