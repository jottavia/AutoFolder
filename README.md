# Auto Folder Shell Extension

A Windows Shell Extension that adds an "Auto Folder" context menu option when right-clicking files in Windows Explorer. When clicked, it creates a new folder named after the file and moves the file into it.

## Features

- Adds "Auto Folder" to Windows Explorer context menu
- Creates a folder named after the selected file
- Automatically moves the file into the new folder
- Leaves the folder name in edit mode for immediate renaming
- Supports Windows 10 and later (64-bit)

## Building

### Requirements

- Visual Studio 2022 with:
  - Desktop development with C++ workload
  - Windows 10 SDK

### Build Steps

1. Open `ShellExt.sln` in Visual Studio 2022
2. Select Release configuration and x64 platform
3. Build the solution
4. The DLL will be created in `x64/Release/AutoFolderShellExt.dll`

### Manual Installation

1. Copy `AutoFolderShellExt.dll` to `%SystemRoot%\System32`
2. Register the DLL:
   ```cmd
   regsvr32 "%SystemRoot%\System32\AutoFolderShellExt.dll"
   ```
3. Restart Windows Explorer or log out and back in

### Manual Uninstallation

1. Unregister the DLL:
   ```cmd
   regsvr32 /u "%SystemRoot%\System32\AutoFolderShellExt.dll"
   ```
2. Delete the DLL from System32
3. Restart Windows Explorer

## Development

The shell extension is implemented as a COM server that provides:
- `IShellExtInit` interface for initialization
- `IContextMenu` interface for the context menu handler

Key files:
- `ShellExt.h/cpp`: Main shell extension implementation
- `dllmain.cpp`: DLL entry point and registration
- `resource.h/rc`: String resources and version info

## Distribution

To create a distribution package:

1. Build the project in Release/x64 configuration
2. Run `prepare_dist.bat` to create the distribution files
3. The `dist` directory will contain:
   - AutoFolderShellExt.dll
   - install.bat
   - uninstall.bat
   - README.txt

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
