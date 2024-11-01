# Development Guide

This document provides detailed instructions for building and developing the File Organizer Shell Extension.

## Development Requirements

- Visual Studio 2022 (Community Edition or higher)
- Windows 10 SDK (10.0.19041.0 or later)
- C++17 compiler support
- Administrator privileges for testing

## Building the Project

1. Open `ShellExt.sln` in Visual Studio 2022
2. Select the desired configuration:
   - Debug or Release
   - x86 or x64 platform
3. Build the solution (F7 or Build > Build Solution)

The compiled DLL will be placed in:
- `x64\FileOrganizerExt.dll` for 64-bit builds
- `x86\FileOrganizerExt.dll` for 32-bit builds

## Project Structure

```
ShellExt/
├── ShellExt.h           # Main header file with class definitions
├── ShellExt.cpp         # Implementation of shell extension functionality
├── dllmain.cpp          # DLL entry point and registration functions
├── ShellExt.def         # Module definition file for exports
├── resource.h           # Resource definitions
├── ShellExt.rc         # Resource file (strings, version info)
├── register.bat         # Registration script
└── unregister.bat      # Unregistration script
```

## Testing During Development

1. Build the project in Debug configuration
2. Run `register.bat` as administrator
3. Open Windows Explorer to test the context menu
4. Use Debug > Attach to Process to attach to explorer.exe
5. Set breakpoints in Visual Studio for debugging
6. Run `unregister.bat` when finished testing

Note: You must unregister the DLL before rebuilding, as Windows keeps it locked while registered.

## Debugging Tips

1. Use OutputDebugString for logging:
   ```cpp
   OutputDebugString(L"Debug message\n");
   ```

2. View debug output using DebugView from Sysinternals:
   - Download from Microsoft's website
   - Run as administrator
   - Filter for your messages

3. Common issues:
   - DLL not registering: Check administrator privileges
   - Context menu not showing: Verify file type registration
   - Explorer crashes: Check COM reference counting

## Best Practices

1. Always implement proper reference counting
2. Handle out-of-memory conditions
3. Validate input parameters
4. Use secure string functions (StringCch*)
5. Clean up resources in destructors
6. Follow COM threading rules

## Adding New Features

1. Update interface definitions in `ShellExt.h`
2. Implement new functionality in `ShellExt.cpp`
3. Add any new resources to `ShellExt.rc`
4. Update version information
5. Document changes in README.md

## Release Process

1. Update version numbers in:
   - resource.h
   - ShellExt.rc
2. Build both x86 and x64 in Release configuration
3. Test on clean Windows installation
4. Create release package:
   - DLLs for both architectures
   - Registration scripts
   - Documentation
5. Tag the release in source control

## Code Style

- Use consistent indentation (4 spaces)
- Follow Microsoft's COM naming conventions
- Comment complex logic and public interfaces
- Use RAII where possible
- Handle all error conditions

## Resources

- [Windows Shell Extension Documentation](https://docs.microsoft.com/en-us/windows/win32/shell/shell-exts)
- [COM Programming Guidelines](https://docs.microsoft.com/en-us/windows/win32/com/com-programming-guide)
- [Windows SDK Samples](https://github.com/microsoft/Windows-classic-samples)
