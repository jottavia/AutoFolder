@echo off
setlocal enabledelayedexpansion

echo Installing Auto Folder Shell Extension...
echo.

:: Check for administrative privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Error: Administrative privileges required.
    echo Please run this script as Administrator.
    pause
    exit /b 1
)

:: Get the directory of the batch file
set "SCRIPT_DIR=%~dp0"

:: Kill Explorer to ensure clean registration
echo Stopping Windows Explorer...
taskkill /f /im explorer.exe >nul 2>&1
timeout /t 2 >nul

:: Install and register 64-bit DLL
if exist "%SCRIPT_DIR%x64\Release\AutoFolderShellExt.dll" (
    echo Installing 64-bit shell extension...
    
    :: Copy to System32
    echo Copying DLL to System32...
    copy /Y "%SCRIPT_DIR%x64\Release\AutoFolderShellExt.dll" "%SystemRoot%\System32\AutoFolderShellExt.dll"
    if !errorLevel! neq 0 (
        echo [ERROR] Failed to copy DLL to System32
        goto :error
    )
    
    :: Register the DLL
    echo Registering shell extension...
    regsvr32 /s "%SystemRoot%\System32\AutoFolderShellExt.dll"
    if !errorLevel! neq 0 (
        echo [ERROR] Failed to register shell extension
        goto :error
    )
    
    echo [SUCCESS] Installation complete
) else (
    echo [ERROR] DLL not found at: %SCRIPT_DIR%x64\Release\AutoFolderShellExt.dll
    echo Please build the project first.
    goto :error
)

:: Restart Explorer
echo.
echo Starting Windows Explorer...
start explorer.exe

echo.
echo Installation completed successfully.
echo The "Auto Folder" option should now appear when right-clicking files.
echo.
echo If you don't see the option:
echo 1. Try logging out and logging back in
echo 2. Or restart your computer
echo.
pause
exit /b 0

:error
echo.
echo Installation failed. Please check the error messages above.
echo.
pause
exit /b 1
