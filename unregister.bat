@echo off
setlocal enabledelayedexpansion

echo Uninstalling Auto Folder Shell Extension...
echo.

:: Check for administrative privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Error: Administrative privileges required.
    echo Please run this script as Administrator.
    pause
    exit /b 1
)

:: Kill Explorer to ensure clean unregistration
echo Stopping Windows Explorer...
taskkill /f /im explorer.exe >nul 2>&1
timeout /t 2 >nul

:: Unregister and remove DLL
if exist "%SystemRoot%\System32\AutoFolderShellExt.dll" (
    echo Unregistering shell extension...
    regsvr32 /u /s "%SystemRoot%\System32\AutoFolderShellExt.dll"
    if !errorLevel! neq 0 (
        echo [WARNING] Failed to unregister shell extension
    )
    
    echo Removing DLL from System32...
    del /F "%SystemRoot%\System32\AutoFolderShellExt.dll"
    if !errorLevel! neq 0 (
        echo [WARNING] Failed to remove DLL
    )
)

:: Clean up registry (just in case)
echo Cleaning registry entries...
reg delete "HKCR\*\shellex\ContextMenuHandlers\File Organizer" /f >nul 2>&1
reg delete "HKCR\CLSID\{B743F9C1-0B24-4C9F-8EE1-C21F3C9A7747}" /f >nul 2>&1

:: Restart Explorer
echo.
echo Starting Windows Explorer...
start explorer.exe

echo.
echo Uninstallation completed.
echo The "Auto Folder" option should no longer appear in the context menu.
echo.
pause
