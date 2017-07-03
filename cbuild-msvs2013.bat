
setlocal

rmdir /s /q build.paho
mkdir build.paho

cd build.paho

REM change VS version to your own
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

REM set Debug or Release
set BUILD_TYPE=Debug

REM if DPAHO_WITH_SSL then set OPENSSL_SEARCH_PATH or look at OPENSSL_SEARCH_PATH 
REM in src/CMakeLists.txt [test/CMakeLists.txt] to see default path for SSL libraries
cmake -G "Visual Studio 12 2013" -DPAHO_WITH_SSL=FALSE -DOPENSSL_SEARCH_PATH="C:/OpenSSL-Win64" -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_CONFIGURATION_TYPES="%BUILD_TYPE%" -DCMAKE_VERBOSE_MAKEFILE=TRUE .. 

REM registry key path to MSBuild.exe is here SOFTWARE\Microsoft\MSBuild\ToolsVersions\12.0\MSBuildToolsPath path 
REM it depends of Visual Studio version

call "C:\Program Files (x86)\MSBuild\12.0\bin\amd64\MSBuild.exe" "paho.sln" "/p:Configuration=%BUILD_TYPE%" "/p:Platform=Win32"

ctest -T test -VV -C "%BUILD_TYPE%"

cd ..

endlocal
