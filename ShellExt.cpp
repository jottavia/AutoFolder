/////////////////////////////////////////////////////////////////////////////
// ShellExt.cpp
// Implementation of the Windows Shell Extension
// Copyright (c) 2024. Licensed under MIT License.
/////////////////////////////////////////////////////////////////////////////

#include "ShellExt.h"
#include <strsafe.h>
#include <filesystem>
#include <shlwapi.h>
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")

// External variables from dllmain.cpp
extern HINSTANCE g_hInst;
extern long g_cDllRef;

// Define the GUID for the shell extension
// {B743F9C1-0B24-4C9F-8EE1-C21F3C9A7747}
const CLSID CLSID_FileOrganizerExt = 
    {0xb743f9c1, 0x0b24, 0x4c9f, {0x8e, 0xe1, 0xc2, 0x1f, 0x3c, 0x9a, 0x77, 0x47}};

// Constructor
CShellExt::CShellExt() : m_cRef(1), m_bIsFile(false) {
    InterlockedIncrement(&g_cDllRef);
}

// Destructor
CShellExt::~CShellExt() {
    InterlockedDecrement(&g_cDllRef);
}

#pragma region IUnknown Implementation

// QueryInterface implementation
STDMETHODIMP CShellExt::QueryInterface(REFIID riid, void** ppv) {
    static const QITAB qit[] = {
        QITABENT(CShellExt, IShellExtInit),
        QITABENT(CShellExt, IContextMenu),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

// AddRef implementation
STDMETHODIMP_(ULONG) CShellExt::AddRef() {
    return InterlockedIncrement(&m_cRef);
}

// Release implementation
STDMETHODIMP_(ULONG) CShellExt::Release() {
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef) {
        delete this;
    }
    return cRef;
}

#pragma endregion

#pragma region IShellExtInit Implementation

// Initialize the shell extension
STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID) {
    if (!pdtobj) {
        return E_INVALIDARG;
    }

    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm;

    // Get the selected files
    if (SUCCEEDED(pdtobj->GetData(&fe, &stm))) {
        HDROP hDrop = static_cast<HDROP>(stm.hGlobal);
        UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

        // We only support single file selection
        if (fileCount == 1) {
            wchar_t szFile[MAX_PATH];
            if (DragQueryFile(hDrop, 0, szFile, ARRAYSIZE(szFile))) {
                m_szSelectedFile = szFile;
                m_bIsFile = !std::filesystem::is_directory(m_szSelectedFile);
            }
        }

        ReleaseStgMedium(&stm);
    }

    return S_OK;
}

#pragma endregion

#pragma region IContextMenu Implementation

// Add menu items to the context menu
STDMETHODIMP CShellExt::QueryContextMenu(
    HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
    // If the flags include CMF_DEFAULTONLY then we shouldn't do anything
    if (uFlags & CMF_DEFAULTONLY) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
    }

    // Only show menu for files, not folders
    if (!m_bIsFile) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
    }

    // Create the menu item
    wchar_t szMenuText[256];
    LoadString(g_hInst, IDS_MENUTEXT, szMenuText, ARRAYSIZE(szMenuText));

    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_STRING | MIIM_ID | MIIM_STATE;
    mii.wID = idCmdFirst + IDM_ORGANIZE;
    mii.dwTypeData = szMenuText;
    mii.fState = MFS_ENABLED;

    if (!InsertMenuItem(hmenu, indexMenu, TRUE, &mii)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Return number of menu items added
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(1));
}

// Handle the menu command
STDMETHODIMP CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
    // Check if we're being called by command line
    if (HIWORD(pici->lpVerb)) {
        return E_INVALIDARG;
    }

    // Check if this is our command
    switch (LOWORD(pici->lpVerb)) {
        case IDM_ORGANIZE:
            return CreateFolderAndMoveFile() ? S_OK : E_FAIL;
        default:
            return E_INVALIDARG;
    }
}

// Provide help text for the menu item
STDMETHODIMP CShellExt::GetCommandString(
    UINT_PTR idCmd, UINT uType, UINT* pReserved, LPSTR pszName, UINT cchMax) {
    if (idCmd != IDM_ORGANIZE) {
        return E_INVALIDARG;
    }

    if (uType == GCS_HELPTEXT) {
        wchar_t szHelp[256];
        LoadString(g_hInst, IDS_HELPTEXT, szHelp, ARRAYSIZE(szHelp));

        if (uType == GCS_HELPTEXT) {
            if (uType & GCS_UNICODE) {
                StringCchCopy((LPWSTR)pszName, cchMax, szHelp);
            } else {
                WideCharToMultiByte(CP_ACP, 0, szHelp, -1, pszName, cchMax, NULL, NULL);
            }
            return S_OK;
        }
    }

    return E_INVALIDARG;
}

#pragma endregion

#pragma region Helper Methods

// Create a new folder and move the file into it
bool CShellExt::CreateFolderAndMoveFile() {
    try {
        std::filesystem::path filePath(m_szSelectedFile);
        std::wstring folderName = GetFileNameWithoutExtension(m_szSelectedFile);
        std::filesystem::path parentPath = filePath.parent_path();
        std::filesystem::path newFolderPath = parentPath / folderName;

        // Create the folder
        if (!std::filesystem::create_directory(newFolderPath)) {
            return false;
        }

        // Move the file
        std::filesystem::path newFilePath = newFolderPath / filePath.filename();
        std::filesystem::rename(filePath, newFilePath);

        // Trigger folder rename mode
        return TriggerFolderRename(newFolderPath.wstring());
    }
    catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

// Extract filename without extension
std::wstring CShellExt::GetFileNameWithoutExtension(const std::wstring& filePath) {
    std::filesystem::path path(filePath);
    return path.stem().wstring();
}

// Trigger the rename mode for the folder
bool CShellExt::TriggerFolderRename(const std::wstring& folderPath) {
    PIDLIST_ABSOLUTE pidl;
    if (SUCCEEDED(SHParseDisplayName(folderPath.c_str(), NULL, &pidl, 0, NULL))) {
        HRESULT hr = SHOpenFolderAndSelectItems(pidl, 0, NULL, 0);
        ILFree(pidl);

        if (SUCCEEDED(hr)) {
            // Simulate F2 key press to trigger rename
            INPUT inputs[2] = {};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_F2;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_F2;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
            return true;
        }
    }
    return false;
}

#pragma endregion

#pragma region Class Factory Implementation

CShellExtClassFactory::CShellExtClassFactory() : m_cRef(1) {
    InterlockedIncrement(&g_cDllRef);
}

CShellExtClassFactory::~CShellExtClassFactory() {
    InterlockedDecrement(&g_cDllRef);
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid, void** ppv) {
    static const QITAB qit[] = {
        QITABENT(CShellExtClassFactory, IClassFactory),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef() {
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release() {
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef) {
        delete this;
    }
    return cRef;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(
    IUnknown* pUnkOuter, REFIID riid, void** ppv) {
    if (pUnkOuter != NULL) {
        return CLASS_E_NOAGGREGATION;
    }

    CShellExt* pExt = new (std::nothrow) CShellExt();
    if (pExt == NULL) {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = pExt->QueryInterface(riid, ppv);
    pExt->Release();
    return hr;
}

STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock) {
    if (fLock) {
        InterlockedIncrement(&g_cDllRef);
    } else {
        InterlockedDecrement(&g_cDllRef);
    }
    return S_OK;
}

#pragma endregion
