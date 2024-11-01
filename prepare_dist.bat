@echo off
setlocal enabledelayedexpansion

echo Preparing distribution package...
echo.

:: Create dist directory
if exist "dist" rd /s /q "dist"
mkdir "dist"

:: Copy the built DLL
if exist "x64\Release\AutoFolderShellExt.dll" (
    copy "x64\Release\AutoFolderShellExt.dll" "dist\AutoFolderShellExt.dll"
) else (
    echo Error: AutoFolderShellExt.dll not found. Please build the project first.
    pause
    exit /b 1
)

:: Copy installation files
copy "dist\install.bat" "dist\install.bat" >nul
copy "dist\uninstall.bat" "dist\uninstall.bat" >nul
copy "dist\README.txt" "dist\README.txt" >nul

echo.
echo Distribution package prepared successfully.
echo Files are in the 'dist' directory.
echo You can now zip the 'dist' directory and distribute it.
echo.
pause
