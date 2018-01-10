@echo off

rem ****************************************************************
rem ******* get yloader
rem git clone https://github.com/adrianmichel/yloader.git yloader

if not exist "external" mkdir external
cd external

rem ****************************************************************
rem ******* get json library
git clone https://github.com/nlohmann/json.git json

rem ****************************************************************
rem ******* get and build boost

git clone --recursive https://github.com/boostorg/boost.git boost
cd boost

rem ******* build boost
call bootstrap
rem ******* copy header only libraries to the include path
call b2 headers

set BOOST_COMMON_OPTIONS=--toolset=msvc-14.0 --with-thread --with-filesystem --with-date_time --with-regex --with-locale --with-atomic --with-program_options link=static threading=multi --build-type=complete -a define=BOOST_USE_WINAPI_VERSION=0x0501 
call b2 %BOOST_COMMON_OPTIONS% address-model=32 --stagedir=stage\x86
call b2 %BOOST_COMMON_OPTIONS% address-model=64 --stagedir=stage\x64


rem ****************************************************************
rem libcurl

cd ..
call git clone --recursive https://github.com/adrianmichel/build-libcurl-windows.git libcurl
cd libcurl
call build

cd ..



