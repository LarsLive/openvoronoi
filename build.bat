@Echo off

:: create build directory
del .\build\win\* /s /q /f
rmdir /Q /S .\build
mkdir build
cd build
mkdir win
cd win

:: please add here your boost installation path
set BOOST_INCLUDE_DIR=/boost_1_80_0

:: please add here your path to Visual Studio Installation
call "c:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"

::Cmake creates us an visualstudio project
:: under build\win you can find this project for build and developing.
cmake.exe ^
-G "Visual Studio 15 2017" ^
-DBoost_INCLUDE_DIR=%BOOST_INCLUDE_DIR% ^
..\..

::Compile project with MSBuild 
MSBuild.exe OpenVoronoi.sln /property:Configuration=Release
 
cd ..\..   



