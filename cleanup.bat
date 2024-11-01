@echo off
echo Cleaning up project directory...
echo.

:: Remove build artifacts and temporary files
if exist ".vs" rd /s /q ".vs"
if exist "x64" rd /s /q "x64"
if exist "x86" rd /s /q "x86"
if exist "Debug" rd /s /q "Debug"
if exist "Release" rd /s /q "Release"
if exist "*.aps" del /f /q "*.aps"
if exist "*.user" del /f /q "*.user"
if exist "*.suo" del /f /q "*.suo"
if exist "*.sdf" del /f /q "*.sdf"
if exist "*.opensdf" del /f /q "*.opensdf"
if exist "*.db" del /f /q "*.db"
if exist "*.opendb" del /f /q "*.opendb"
if exist "*.ipch" del /f /q "*.ipch"

:: Remove unnecessary scripts and directories
if exist "setup_github.bat" del /f /q "setup_github.bat"
if exist "dist" rd /s /q "dist"
if exist "build.bat" del /f /q "build.bat"
if exist "build.ps1" del /f /q "build.ps1"
if exist "prepare_dist.bat" del /f /q "prepare_dist.bat"

echo.
echo Directory cleaned. Files remaining:
echo.
echo Source files:
echo - ShellExt.h
echo - ShellExt.cpp
echo - dllmain.cpp
echo.
echo Project files:
echo - ShellExt.sln
echo - ShellExt.vcxproj
echo - ShellExt.def
echo.
echo Resource files:
echo - resource.h
echo - ShellExt.rc
echo.
echo Scripts:
echo - build.cmd
echo - register.bat
echo - unregister.bat
echo.
echo Documentation:
echo - README.md
echo - LICENSE
echo.
echo Configuration:
echo - .gitignore
echo - .vscode/tasks.json
echo - .vscode/launch.json
echo - .vscode/c_cpp_properties.json
echo.
pause
