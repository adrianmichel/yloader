@echo off

git clone https://github.com/Microsoft/vcpkg.git external\vcpkg\
cd external\vcpkg
call .\bootstrap-vcpkg.bat
call .\vcpkg integrate install
call .\vcpkg install curl[openssl] --triplet x86-windows
call .\vcpkg install curl[openssl] --triplet x64-windows
call .\vcpkg install nlohmann-json --triplet x86-windows
call .\vcpkg install nlohmann-json --triplet x64-windows
cd ..\..
