/*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License.
 See the LICENSE file in the project root for more details.
================================================================================
*/


#include "win_helpers.h"
#ifdef _WIN32
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

char* _get_wmi_string_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    char* result = strdup("N/A");
    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_BSTR:
                if (vtProp.bstrVal) {
                    int len = WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, NULL, 0, NULL, NULL);
                    if (len > 0) {
                        char* temp = (char*)malloc(len);
                        if (temp) {
                            WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, temp, len, NULL, NULL);
                            free(result);
                            result = temp;
                        }
                    }
                }
                break;
            case VT_I1:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%d", vtProp.cVal);
                break;
            case VT_UI1:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%u", vtProp.bVal);
                break;
            case VT_I2:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%d", vtProp.iVal);
                break;
            case VT_UI2:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%u", vtProp.uiVal);
                break;
            case VT_I4:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%ld", vtProp.lVal);
                break;
            case VT_UI4:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%lu", vtProp.ulVal);
                break;
            case VT_I8:
                free(result);
                result = (char*)malloc(64);
                if (result) sprintf(result, "%lld", vtProp.llVal);
                break;
            case VT_UI8:
                free(result);
                result = (char*)malloc(64);
                if (result) sprintf(result, "%llu", vtProp.ullVal);
                break;
            case VT_INT:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%d", vtProp.intVal);
                break;
            case VT_UINT:
                free(result);
                result = (char*)malloc(32);
                if (result) sprintf(result, "%u", vtProp.uintVal);
                break;
            case VT_BOOL:
                free(result);
                result = strdup(vtProp.boolVal == VARIANT_TRUE ? "True" : "False");
                break;
            default:
                // Return empty string for unsupported types
                break;
        }
    }
    
    VariantClear(&vtProp);
    return result;
}

ULONG _get_wmi_ulong_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    ULONG result = 0;
    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_I1:    result = (ULONG)vtProp.cVal; break;
            case VT_UI1:   result = (ULONG)vtProp.bVal; break;
            case VT_I2:    result = (ULONG)vtProp.iVal; break;
            case VT_UI2:   result = (ULONG)vtProp.uiVal; break;
            case VT_I4:    result = (ULONG)vtProp.lVal; break;
            case VT_UI4:   result = vtProp.ulVal; break;
            case VT_I8:    result = (ULONG)vtProp.llVal; break;
            case VT_UI8:   result = (ULONG)vtProp.ullVal; break;
            case VT_INT:   result = (ULONG)vtProp.intVal; break;
            case VT_UINT:  result = (ULONG)vtProp.uintVal; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = (ULONG)_wtoi(vtProp.bstrVal);
                break;
            case VT_BOOL:
                result = (vtProp.boolVal == VARIANT_TRUE) ? 1 : 0;
                break;
            default:
                // Return 0 for unsupported types
                break;
        }
    }
    
    VariantClear(&vtProp);
    return result;
}

LONGLONG _get_wmi_longlong_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    LONGLONG result = 0;
    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);

    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_I1:    result = (LONGLONG)vtProp.cVal; break;
            case VT_UI1:   result = (LONGLONG)vtProp.bVal; break;
            case VT_I2:    result = (LONGLONG)vtProp.iVal; break;
            case VT_UI2:   result = (LONGLONG)vtProp.uiVal; break;
            case VT_I4:    result = (LONGLONG)vtProp.lVal; break;
            case VT_UI4:   result = (LONGLONG)vtProp.ulVal; break;
            case VT_I8:    result = vtProp.llVal; break;
            case VT_UI8:   result = vtProp.ullVal; break;
            case VT_INT:   result = (LONGLONG)vtProp.intVal; break;
            case VT_UINT:  result = (LONGLONG)vtProp.uintVal; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = _wtoi64(vtProp.bstrVal);
                break;
            case VT_BOOL:
                result = (vtProp.boolVal == VARIANT_TRUE) ? 1 : 0;
                break;
            default:
                // Return 0 for unsupported types
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}

int _get_wmi_bool_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    int result = 0;
    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_BOOL:
                result = (vtProp.boolVal == VARIANT_TRUE) ? 1 : 0;
                break;
            case VT_I1:    result = (vtProp.cVal != 0) ? 1 : 0; break;
            case VT_UI1:   result = (vtProp.bVal != 0) ? 1 : 0; break;
            case VT_I2:    result = (vtProp.iVal != 0) ? 1 : 0; break;
            case VT_UI2:   result = (vtProp.uiVal != 0) ? 1 : 0; break;
            case VT_I4:    result = (vtProp.lVal != 0) ? 1 : 0; break;
            case VT_UI4:   result = (vtProp.ulVal != 0) ? 1 : 0; break;
            case VT_I8:    result = (vtProp.llVal != 0) ? 1 : 0; break;
            case VT_UI8:   result = (vtProp.ullVal != 0) ? 1 : 0; break;
            case VT_INT:   result = (vtProp.intVal != 0) ? 1 : 0; break;
            case VT_UINT:  result = (vtProp.uintVal != 0) ? 1 : 0; break;
            case VT_BSTR:
                if (vtProp.bstrVal) {
                    if (wcscmp(vtProp.bstrVal, L"True") == 0 || wcscmp(vtProp.bstrVal, L"true") == 0 || wcscmp(vtProp.bstrVal, L"1") == 0) {
                        result = 1;
                    }
                }
                break;
            default:
                // Return 0 for unsupported types
                break;
        }
    }
    
    VariantClear(&vtProp);
    return result;
}

HRESULT _initialize_wmi(IWbemLocator **pLoc, IWbemServices **pSvc) {
    HRESULT hr;
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        return hr;
    }
    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL
    );
    if (FAILED(hr) && hr != RPC_E_TOO_LATE && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED)) {
        CoUninitialize();
        return hr;
    }
    hr = CoCreateInstance(
        &CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
        &IID_IWbemLocator, (LPVOID *)pLoc
    );
    if (FAILED(hr)) {
        CoUninitialize();
        return hr;
    }
    BSTR resource = SysAllocString(L"ROOT\\CIMV2");
    if (!resource) {
        if (*pLoc) (*pLoc)->lpVtbl->Release(*pLoc);
        CoUninitialize();
        return E_OUTOFMEMORY;
    }
    hr = (*pLoc)->lpVtbl->ConnectServer(
        *pLoc, resource, NULL, NULL, 0, 0, NULL, NULL, pSvc
    );
    SysFreeString(resource);
    if (FAILED(hr)) {
        if (*pLoc) (*pLoc)->lpVtbl->Release(*pLoc);
        CoUninitialize();
        return hr;
    }
    hr = CoSetProxyBlanket(
        (IUnknown*)*pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE
    );
    if (FAILED(hr)) {
        if (*pSvc) (*pSvc)->lpVtbl->Release(*pSvc);
        if (*pLoc) (*pLoc)->lpVtbl->Release(*pLoc);
        CoUninitialize();
    }
    return hr;
}

void _cleanup_wmi(IWbemLocator *pLoc, IWbemServices *pSvc, IEnumWbemClassObject *pEnumerator, IWbemClassObject *pclsObj) {
    if (pclsObj) pclsObj->lpVtbl->Release(pclsObj);
    if (pEnumerator) pEnumerator->lpVtbl->Release(pEnumerator);
    if (pSvc) pSvc->lpVtbl->Release(pSvc);
    if (pLoc) pLoc->lpVtbl->Release(pLoc);
    CoUninitialize();
}

#endif // _WIN32
