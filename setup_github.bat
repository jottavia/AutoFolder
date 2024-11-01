@echo off
setlocal enabledelayedexpansion

echo Setting up GitHub repository for Auto Folder Shell Extension...
echo.

:: First, run cleanup to remove unnecessary files
call cleanup.bat

:: Initialize Git repository
echo Initializing Git repository...
git init

:: Add all necessary files
echo Adding files to Git...
git add ^
    .gitignore ^
    .vscode\* ^
    LICENSE ^
    README.md ^
    ShellExt.cpp ^
    ShellExt.def ^
    ShellExt.h ^
    ShellExt.rc ^
    ShellExt.sln ^
    ShellExt.vcxproj ^
    build.cmd ^
    cleanup.bat ^
    dllmain.cpp ^
    register.bat ^
    resource.h ^
    unregister.bat

:: Create initial commit
echo Creating initial commit...
git commit -m "Initial commit: Auto Folder Shell Extension"

:ask_repo
echo.
set /p "REPO_NAME=Enter your GitHub repository name (default: AutoFolderShellExt): "
if "!REPO_NAME!"=="" set "REPO_NAME=AutoFolderShellExt"

:ask_username
echo.
set /p "GITHUB_USERNAME=Enter your GitHub username: "
if "!GITHUB_USERNAME!"=="" (
    echo GitHub username cannot be empty
    goto ask_username
)

:: Create GitHub repository using GitHub CLI if available
where gh >nul 2>&1
if !errorLevel! equ 0 (
    echo.
    echo Creating GitHub repository using GitHub CLI...
    echo Note: Make sure you're logged in to GitHub CLI (run 'gh auth login' if needed)
    
    choice /C YN /M "Create private repository"
    if !errorLevel! equ 1 (
        gh repo create !REPO_NAME! --private --source . --remote origin
    ) else (
        gh repo create !REPO_NAME! --public --source . --remote origin
    )
) else (
    :: Set up remote manually
    echo.
    echo GitHub CLI not found. Setting up remote manually...
    
    choice /C YN /M "Create private repository"
    if !errorLevel! equ 1 (
        echo Please create a private repository named '!REPO_NAME!' on GitHub
    ) else (
        echo Please create a public repository named '!REPO_NAME!' on GitHub
    )
    
    echo.
    echo Go to: https://github.com/new
    echo Repository name: !REPO_NAME!
    echo.
    pause
    
    git remote add origin https://github.com/!GITHUB_USERNAME!/!REPO_NAME!.git
)

:: Push to GitHub
echo.
echo Pushing to GitHub...
git branch -M main
git push -u origin main

echo.
echo Repository setup complete!
echo Your code is now available at: https://github.com/!GITHUB_USERNAME!/!REPO_NAME!
echo.
pause
