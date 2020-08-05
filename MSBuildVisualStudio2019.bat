@echo off
cd /d %~dp0
if not exist build ( mkdir build )
cd build
rem Generate VS2019 project.
cmake -G "Visual Studio 16 2019" -A "x64" -B build "-DCMAKE_TOOLCHAIN_FILE=F:/vcpkg/scripts/buildsystems/vcpkg.cmake" ..
pause
