rd /s /q build-ax
mkdir build-ax
cd build-ax

call cmake "C:\Qt\6.9.0\Src\qtactiveqt"
cmake --build . --config=RelWithDebInfo
REM cmake --build .

REM cd ../
