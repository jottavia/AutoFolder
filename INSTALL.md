# Installation Guide

This guide provides detailed instructions for installing and using the File Organizer Shell Extension.

## System Requirements

- Windows 10 or later
- Administrator privileges for installation
- Approximately 1MB of disk space

## Installation Steps

### Automated Installation

1. Download the latest release package
2. Extract the ZIP file to a permanent location
   - Recommended: `C:\Program Files\File Organizer`
3. Right-click `register.bat`
4. Select "Run as administrator"
5. Wait for the confirmation message
6. Restart Windows Explorer:
   - Press Ctrl+Shift+Esc to open Task Manager
   - Find "Windows Explorer" under Processes
   - Right-click and select "Restart"

### Manual Installation

For advanced users who prefer manual registration:

1. Open Command Prompt as administrator
2. Navigate to the installation directory
3. Register the appropriate DLL:
   ```batch
   # For 64-bit Windows
   regsvr32 x64\FileOrganizerExt.dll

   # For 32-bit Windows
   regsvr32 x86\FileOrganizerExt.dll
   ```

## Verifying Installation

1. Open Windows Explorer
2. Right-click any file
3. Look for "Organize in New Folder" in the context menu
4. If the option is missing:
   - Verify you ran the registration as administrator
   - Ensure you restarted Windows Explorer
   - Check Event Viewer for any error messages

## Usage Instructions

1. Right-click any file in Windows Explorer
2. Select "Organize in New Folder"
3. The extension will:
   - Create a new folder named after the file
   - Move the file into that folder
   - Leave the folder name selected for renaming
4. Type a new name if desired and press Enter

## Troubleshooting

### Common Issues

1. "Run as administrator" option missing
   - Solution: Hold Shift while right-clicking the batch file

2. "Access denied" during registration
   - Solution: Ensure you're running as administrator
   - Solution: Close any programs using the DLL

3. Context menu option not appearing
   - Solution: Restart Windows Explorer
   - Solution: Re-run registration as administrator

4. Error 0x80004005 during registration
   - Solution: Unregister first, then register again
   - Solution: Check system permissions

### Getting Help

If you encounter issues:

1. Check the [Issues](../../issues) page
2. Create a new issue with:
   - Windows version
   - System architecture (32/64-bit)
   - Error messages
   - Steps to reproduce

## Uninstallation

### Automated Uninstallation

1. Navigate to the installation directory
2. Right-click `unregister.bat`
3. Select "Run as administrator"
4. Wait for the confirmation message
5. Restart Windows Explorer
6. Delete the installation directory

### Manual Uninstallation

1. Open Command Prompt as administrator
2. Navigate to the installation directory
3. Unregister the DLL:
   ```batch
   # For 64-bit Windows
   regsvr32 /u x64\FileOrganizerExt.dll

   # For 32-bit Windows
   regsvr32 /u x86\FileOrganizerExt.dll
   ```
4. Delete the installation directory

## Updating

1. Uninstall the current version
2. Download the new version
3. Follow the installation steps above

## Silent Installation

For system administrators:

```batch
# Silent registration
regsvr32 /s x64\FileOrganizerExt.dll

# Silent unregistration
regsvr32 /s /u x64\FileOrganizerExt.dll
```

## Post-Installation Notes

- The extension integrates with Windows Explorer
- No additional configuration is needed
- No system restart is required
- Changes take effect after Explorer restart
