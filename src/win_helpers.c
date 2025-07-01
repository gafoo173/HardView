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
    if (SUCCEEDED(hr) && (vtProp.vt == VT_BSTR)) {
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
    VariantClear(&vtProp);
    return result;
}

ULONG _get_wmi_ulong_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    ULONG result = 0;
    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
        result = vtProp.ulVal;
    } else if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
        result = (ULONG)_wtoi(vtProp.bstrVal);
    }
    VariantClear(&vtProp);
    return result;
}

LONGLONG _get_wmi_longlong_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    LONGLONG result = 0;
    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && (vtProp.vt == VT_UI8 || vtProp.vt == VT_BSTR)) {
        if (vtProp.vt == VT_UI8) {
            result = vtProp.ullVal;
        } else if (vtProp.vt == VT_BSTR) {
            result = _wtoi64(vtProp.bstrVal);
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
    if (SUCCEEDED(hr) && vtProp.vt == VT_BOOL) {
        result = (vtProp.boolVal == VARIANT_TRUE);
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
