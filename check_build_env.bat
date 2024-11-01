@echo off
setlocal enabledelayedexpansion

echo Checking build environment...
echo.

:: Check for Visual Studio installation
set "VS2022PATH=C:\Program Files\Microsoft Visual Studio\2022\Community"
if exist "%VS2022PATH%" (
    echo [✓] Visual Studio 2022 found
) else (
    echo [×] Visual Studio 2022 not found at expected location
    echo     Please install Visual Studio 2022 with C++ desktop development workload
)

:: Check for MSBuild
set "MSBUILD_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
if exist "%MSBUILD_PATH%" (
    echo [✓] MSBuild found
) else (
    echo [×] MSBuild not found at expected location
    echo     Please ensure Visual Studio installation is complete
)

:: Check for Windows SDK
set "SDK_PATH=C:\Program Files (x86)\Windows Kits\10"
if exist "%SDK_PATH%" (
    echo [✓] Windows SDK found
) else (
    echo [×] Windows SDK not found
    echo     Please install Windows SDK through Visual Studio Installer
)

echo.
echo If any components are missing, please:
echo 1. Open Visual Studio Installer
echo 2. Modify Visual Studio 2022
echo 3. Ensure "Desktop development with C++" is selected
echo 4. Under Individual Components, ensure "Windows 10 SDK" is selected
echo 5. Click Modify to install missing components

pause
