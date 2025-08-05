@echo off

REM Runs CMake to generate build files with MinGW Makefiles
cmake -G "MinGW Makefiles" -B build
IF %ERRORLEVEL% NEQ 0 (
  echo Error: CMake generation failed.
  exit /b %ERRORLEVEL%
)

REM Builds the project from the build folder
cmake --build build
IF %ERRORLEVEL% NEQ 0 (
  echo Error: Project build failed.
  exit /b %ERRORLEVEL%
)

echo Project built successfully.