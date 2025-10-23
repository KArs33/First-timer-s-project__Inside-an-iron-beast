@echo off
REM Build wrapper for IAIB project — uses cmd quoting-safe paths
setlocal
REM Ensure build folder exists
if not exist "%~dp0build" mkdir "%~dp0build"

REM Compile all sources into build\driver.exe
g++ -g "%~dp0driver.cpp" "%~dp0IAIB.cpp" "%~dp0map.cpp" "%~dp0player.cpp" "%~dp0foe.cpp" "%~dp0loc.cpp" -o "%~dp0build\driver.exe"
if errorlevel 1 (
  echo BUILD_FAIL
  endlocal
  exit /b 1
)
echo BUILD_OK
endlocal
exit /b 0
