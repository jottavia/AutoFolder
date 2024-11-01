/////////////////////////////////////////////////////////////////////////////
// dllmain.cpp
// DLL Entry Point and Registration Functions
// Copyright (c) 2024. Licensed under MIT License.
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <guiddef.h>
#include <strsafe.h>
#include "ShellExt.h"
#include "resource.h"

// {B743F9C1-0B24-4C9F-8EE1-C21F3C9A7747}
// This GUID must match the one in ShellExt.h
DEFINE_GUID(CLSID_FileOrganizerExt,
    0xb743f9c1, 0x0b24, 0x4c9f, 0x8e, 0xe1, 0xc2, 0x1f, 0x3c, 0x9a, 0x77, 0x47);

// Global variables
HINSTANCE g_hInst = NULL;
long g_cDllRef = 0;

// Forward declarations
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv);
STDAPI DllCanUnloadNow(void);
STDAPI DllRegisterServer(void);
STDAPI DllUnregisterServer(void);

// Helper functions
HRESULT SetHKCRRegistryKeyAndValue(const wchar_t* pszSubKey, const wchar_t* pszValueName, const wchar_t* pszData);
HRESULT GetHKCRRegistryKeyAndValue(const wchar_t* pszSubKey, const wchar_t* pszValueName, wchar_t* pszData, DWORD cbData);
HRESULT RegisterInprocServer(const wchar_t* pszModule, const GUID& clsid, const wchar_t* pszFriendlyName, const wchar_t* pszThreadModel);
HRESULT RegisterShellExtContextMenuHandler(const wchar_t* pszFileType, const GUID& clsid, const wchar_t* pszFriendlyName);
HRESULT UnregisterInprocServer(const GUID& clsid);
HRESULT UnregisterShellExtContextMenuHandler(const wchar_t* pszFileType, const GUID& clsid);

#pragma region DLL Entry Point

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            g_hInst = hModule;
            DisableThreadLibraryCalls(hModule);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

#pragma endregion

#pragma region COM Registration Functions

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    if (rclsid != CLSID_FileOrganizerExt) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    CShellExtClassFactory* pClassFactory = new (std::nothrow) CShellExtClassFactory();
    if (pClassFactory == NULL) {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = pClassFactory->QueryInterface(riid, ppv);
    pClassFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow(void) {
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer(void) {
    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Register the component
    HRESULT hr = RegisterInprocServer(
        szModule,
        CLSID_FileOrganizerExt,
        L"File Organizer Shell Extension",
        L"Apartment"
    );
    if (SUCCEEDED(hr)) {
        // Register the context menu handler
        hr = RegisterShellExtContextMenuHandler(
            L"*",  // Register for all file types
            CLSID_FileOrganizerExt,
            L"File Organizer"
        );
    }

    return hr;
}

STDAPI DllUnregisterServer(void) {
    // Unregister the context menu handler
    HRESULT hr = UnregisterShellExtContextMenuHandler(
        L"*",
        CLSID_FileOrganizerExt
    );
    if (SUCCEEDED(hr)) {
        // Unregister the component
        hr = UnregisterInprocServer(CLSID_FileOrganizerExt);
    }

    return hr;
}

#pragma endregion

#pragma region Registry Helper Functions

HRESULT SetHKCRRegistryKeyAndValue(const wchar_t* pszSubKey, const wchar_t* pszValueName, const wchar_t* pszData) {
    HRESULT hr = S_OK;
    HKEY hKey = NULL;

    // Create/open the key
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(
        HKEY_CLASSES_ROOT,
        pszSubKey,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL));

    if (SUCCEEDED(hr)) {
        if (pszData != NULL) {
            // Set the value
            DWORD cbData = (lstrlen(pszData) + 1) * sizeof(*pszData);
            hr = HRESULT_FROM_WIN32(RegSetValueEx(
                hKey,
                pszValueName,
                0,
                REG_SZ,
                reinterpret_cast<const BYTE*>(pszData),
                cbData));
        }
        RegCloseKey(hKey);
    }

    return hr;
}

HRESULT GetHKCRRegistryKeyAndValue(const wchar_t* pszSubKey, const wchar_t* pszValueName, wchar_t* pszData, DWORD cbData) {
    HRESULT hr = S_OK;
    HKEY hKey = NULL;

    // Open the key
    hr = HRESULT_FROM_WIN32(RegOpenKeyEx(
        HKEY_CLASSES_ROOT,
        pszSubKey,
        0,
        KEY_READ,
        &hKey));

    if (SUCCEEDED(hr)) {
        // Get the value
        hr = HRESULT_FROM_WIN32(RegQueryValueEx(
            hKey,
            pszValueName,
            NULL,
            NULL,
            reinterpret_cast<LPBYTE>(pszData),
            &cbData));
        RegCloseKey(hKey);
    }

    return hr;
}

HRESULT RegisterInprocServer(const wchar_t* pszModule, const GUID& clsid, const wchar_t* pszFriendlyName, const wchar_t* pszThreadModel) {
    if (pszModule == NULL || pszThreadModel == NULL) {
        return E_INVALIDARG;
    }

    wchar_t szCLSID[MAX_PATH];
    wchar_t szSubkey[MAX_PATH];
    HRESULT hr = S_OK;

    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (FAILED(hr)) return hr;

    // Set the default value of the CLSID key
    hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);
    if (SUCCEEDED(hr)) {
        // Set the InprocServer32 key
        hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s\\InprocServer32", szCLSID);
        if (SUCCEEDED(hr)) {
            hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszModule);
            if (SUCCEEDED(hr)) {
                // Set the threading model
                hr = SetHKCRRegistryKeyAndValue(szSubkey, L"ThreadingModel", pszThreadModel);
            }
        }
    }

    return hr;
}

HRESULT RegisterShellExtContextMenuHandler(const wchar_t* pszFileType, const GUID& clsid, const wchar_t* pszFriendlyName) {
    if (pszFileType == NULL) {
        return E_INVALIDARG;
    }

    wchar_t szCLSID[MAX_PATH];
    wchar_t szSubkey[MAX_PATH];
    HRESULT hr = S_OK;

    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"%s\\shellex\\ContextMenuHandlers\\%s",
        pszFileType, pszFriendlyName);
    if (FAILED(hr)) return hr;

    // Set the default value of the context menu handler key
    return SetHKCRRegistryKeyAndValue(szSubkey, NULL, szCLSID);
}

HRESULT UnregisterInprocServer(const GUID& clsid) {
    wchar_t szCLSID[MAX_PATH];
    wchar_t szSubkey[MAX_PATH];
    HRESULT hr = S_OK;

    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (FAILED(hr)) return hr;

    return HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
}

HRESULT UnregisterShellExtContextMenuHandler(const wchar_t* pszFileType, const GUID& clsid) {
    if (pszFileType == NULL) {
        return E_INVALIDARG;
    }

    wchar_t szCLSID[MAX_PATH];
    wchar_t szSubkey[MAX_PATH];
    HRESULT hr = S_OK;

    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"%s\\shellex\\ContextMenuHandlers\\File Organizer",
        pszFileType);
    if (FAILED(hr)) return hr;

    return HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
}

#pragma endregion
