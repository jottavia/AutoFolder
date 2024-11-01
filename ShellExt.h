/////////////////////////////////////////////////////////////////////////////
// ShellExt.h
// Windows Shell Extension for File Organization
// Copyright (c) 2024. Licensed under MIT License.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <shlobj.h>
#include <string>

// {B743F9C1-0B24-4C9F-8EE1-C21F3C9A7747}
// Unique GUID for the shell extension
DEFINE_GUID(CLSID_FileOrganizerExt,
    0xb743f9c1, 0x0b24, 0x4c9f, 0x8e, 0xe1, 0xc2, 0x1f, 0x3c, 0x9a, 0x77, 0x47);

class CShellExt : public IShellExtInit, public IContextMenu {
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IShellExtInit methods
    STDMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID);

    // IContextMenu methods
    STDMETHODIMP QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst,
                                 UINT idCmdLast, UINT uFlags);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved,
                                LPSTR pszName, UINT cchMax);

    CShellExt();
    ~CShellExt();

private:
    // Helper methods
    bool CreateFolderAndMoveFile();
    std::wstring GetFileNameWithoutExtension(const std::wstring& filePath);
    bool TriggerFolderRename(const std::wstring& folderPath);

    // Member variables
    long m_cRef;                     // Reference count
    std::wstring m_szSelectedFile;   // Path to the selected file
    bool m_bIsFile;                  // Flag indicating if selection is a file

    // Constants
    static const UINT IDM_ORGANIZE = 0;  // Command ID for the context menu
};

// Class factory for creating shell extension instances
class CShellExtClassFactory : public IClassFactory {
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IClassFactory methods
    STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv);
    STDMETHODIMP LockServer(BOOL fLock);

    CShellExtClassFactory();
    ~CShellExtClassFactory();

private:
    long m_cRef;
};

// Declare external variables
extern HINSTANCE g_hInst;
extern long g_cDllRef;
