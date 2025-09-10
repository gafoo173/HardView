#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <Wbemidl.h>
#include <comdef.h>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <vector>

#pragma comment(lib, "wbemuuid.lib")

namespace win {

inline std::string get_string_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    std::string result = "N/A";

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_BSTR:
                if (vtProp.bstrVal != nullptr) {
                    int length = SysStringLen(vtProp.bstrVal);
                    int size_needed = WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, length, NULL, 0, NULL, NULL);
                    result.resize(size_needed);
                    WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, length, &result[0], size_needed, NULL, NULL);
                }
                break;
            case VT_I1: result = std::to_string(vtProp.cVal); break;
            case VT_UI1: result = std::to_string(vtProp.bVal); break;
            case VT_I2: result = std::to_string(vtProp.iVal); break;
            case VT_UI2: result = std::to_string(vtProp.uiVal); break;
            case VT_I4: result = std::to_string(vtProp.lVal); break;
            case VT_UI4: result = std::to_string(vtProp.ulVal); break;
            case VT_I8: result = std::to_string(vtProp.llVal); break;
            case VT_UI8: result = std::to_string(vtProp.ullVal); break;
            case VT_INT: result = std::to_string(vtProp.intVal); break;
            case VT_UINT: result = std::to_string(vtProp.uintVal); break;
            case VT_R4: result = std::to_string(vtProp.fltVal); break;
            case VT_R8: result = std::to_string(vtProp.dblVal); break;
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE) ? "true" : "false"; break;
            case VT_NULL:
            case VT_EMPTY:
                result = "N/A";
                break;
        }
    }
    VariantClear(&vtProp);
    return result;
}

inline std::vector<std::string> get_string_array(IWbemClassObject* obj, const wchar_t* name) {
    std::vector<std::string> result;
    VARIANT vtProp;
    VariantInit(&vtProp);

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        if ((vtProp.vt & VT_ARRAY) && (vtProp.vt & VT_BSTR)) {
            SAFEARRAY* psa = vtProp.parray;
            if (psa != nullptr) {
                LONG lBound = 0, uBound = 0;
                SafeArrayGetLBound(psa, 1, &lBound);
                SafeArrayGetUBound(psa, 1, &uBound);

                for (LONG i = lBound; i <= uBound; i++) {
                    BSTR bstrItem;
                    SafeArrayGetElement(psa, &i, &bstrItem);
                    if (bstrItem != nullptr) {
                        int length = SysStringLen(bstrItem);
                        int size_needed = WideCharToMultiByte(CP_UTF8, 0, bstrItem, length, NULL, 0, NULL, NULL);
                        std::string str(size_needed, 0);
                        WideCharToMultiByte(CP_UTF8, 0, bstrItem, length, &str[0], size_needed, NULL, NULL);
                        result.push_back(str);
                        SysFreeString(bstrItem);
                    }
                }
            }
        }
    }
    VariantClear(&vtProp);
    return result;
}
inline std::vector<uint16_t> get_uint16_array_property(IWbemClassObject* obj, const wchar_t* name) {
    std::vector<uint16_t> result;
    VARIANT vtProp;
    VariantInit(&vtProp);

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        if ((vtProp.vt & VT_ARRAY) && (vtProp.vt & VT_UI2)) { // VT_UI2 = uint16_t
            SAFEARRAY* psa = vtProp.parray;
            if (psa != nullptr) {
                LONG lBound = 0, uBound = 0;
                SafeArrayGetLBound(psa, 1, &lBound);
                SafeArrayGetUBound(psa, 1, &uBound);

                for (LONG i = lBound; i <= uBound; i++) {
                    uint16_t value = 0;
                    SafeArrayGetElement(psa, &i, &value);
                    result.push_back(value);
                }
            }
        }
    }

    VariantClear(&vtProp);
    return result;
}


inline uint16_t get_uint16_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp); 
    uint16_t result = 0;

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) { 
            case VT_I1: result = static_cast<uint16_t>(vtProp.cVal); break;
            case VT_UI1: result = static_cast<uint16_t>(vtProp.bVal); break;
            case VT_I2: result = static_cast<uint16_t>(vtProp.iVal); break;
            case VT_UI2: result = vtProp.uiVal; break;
            case VT_I4: result = static_cast<uint16_t>(vtProp.lVal); break;
            case VT_UI4: result = static_cast<uint16_t>(vtProp.ulVal); break;
            case VT_I8: result = static_cast<uint16_t>(vtProp.llVal); break;
            case VT_UI8: result = static_cast<uint16_t>(vtProp.ullVal); break;
            case VT_INT: result = static_cast<uint16_t>(vtProp.intVal); break;
            case VT_UINT: result = static_cast<uint16_t>(vtProp.uintVal); break;
            case VT_R4: result = static_cast<uint16_t>(vtProp.fltVal); break;
            case VT_R8: result = static_cast<uint16_t>(vtProp.dblVal); break;
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE) ? 1 : 0; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = static_cast<uint16_t>(_wtoi(vtProp.bstrVal));
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}

inline uint32_t get_uint32_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp); 
    uint32_t result = 0;

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) { 
            case VT_I1: result = static_cast<uint32_t>(vtProp.cVal); break;
            case VT_UI1: result = static_cast<uint32_t>(vtProp.bVal); break;
            case VT_I2: result = static_cast<uint32_t>(vtProp.iVal); break;
            case VT_UI2: result = static_cast<uint32_t>(vtProp.uiVal); break;
            case VT_I4: result = static_cast<uint32_t>(vtProp.lVal); break;
            case VT_UI4: result = vtProp.ulVal; break;
            case VT_I8: result = static_cast<uint32_t>(vtProp.llVal); break;
            case VT_UI8: result = static_cast<uint32_t>(vtProp.ullVal); break;
            case VT_INT: result = static_cast<uint32_t>(vtProp.intVal); break;
            case VT_UINT: result = static_cast<uint32_t>(vtProp.uintVal); break;
            case VT_R4: result = static_cast<uint32_t>(vtProp.fltVal); break;
            case VT_R8: result = static_cast<uint32_t>(vtProp.dblVal); break;
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE) ? 1 : 0; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = static_cast<uint32_t>(_wtoi(vtProp.bstrVal));
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}
inline uint64_t get_uint64_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp); 
    uint64_t result = 0;

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) { 
            case VT_I1: result = static_cast<uint64_t>(vtProp.cVal); break;
            case VT_UI1: result = static_cast<uint64_t>(vtProp.bVal); break;
            case VT_I2: result = static_cast<uint64_t>(vtProp.iVal); break;
            case VT_UI2: result = static_cast<uint64_t>(vtProp.uiVal); break;
            case VT_I4: result = static_cast<uint64_t>(vtProp.lVal); break;
            case VT_UI4: result = vtProp.ulVal; break;
            case VT_I8: result = static_cast<uint64_t>(vtProp.llVal); break;
            case VT_UI8: result = static_cast<uint64_t>(vtProp.ullVal); break;
            case VT_INT: result = static_cast<uint64_t>(vtProp.intVal); break;
            case VT_UINT: result = static_cast<uint64_t>(vtProp.uintVal); break;
            case VT_R4: result = static_cast<uint64_t>(vtProp.fltVal); break;
            case VT_R8: result = static_cast<uint64_t>(vtProp.dblVal); break;
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE) ? 1 : 0; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = static_cast<uint64_t>(_wtoi(vtProp.bstrVal));
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}

inline int64_t get_int64_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    int64_t result = 0;

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_I1: result = vtProp.cVal; break;
            case VT_UI1: result = vtProp.bVal; break;
            case VT_I2: result = vtProp.iVal; break;
            case VT_UI2: result = vtProp.uiVal; break;
            case VT_I4: result = vtProp.lVal; break;
            case VT_UI4: result = vtProp.ulVal; break;
            case VT_I8: result = vtProp.llVal; break;
            case VT_UI8: result = static_cast<int64_t>(vtProp.ullVal); break;
            case VT_INT: result = vtProp.intVal; break;
            case VT_UINT: result = vtProp.uintVal; break;
            case VT_R4: result = static_cast<int64_t>(vtProp.fltVal); break;
            case VT_R8: result = static_cast<int64_t>(vtProp.dblVal); break;
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE) ? 1 : 0; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = _wtoi64(vtProp.bstrVal);
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}

inline bool get_bool_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    bool result = false;

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE); break;
            case VT_I1: result = (vtProp.cVal != 0); break;
            case VT_UI1: result = (vtProp.bVal != 0); break;
            case VT_I2: result = (vtProp.iVal != 0); break;
            case VT_UI2: result = (vtProp.uiVal != 0); break;
            case VT_I4: result = (vtProp.lVal != 0); break;
            case VT_UI4: result = (vtProp.ulVal != 0); break;
            case VT_I8: result = (vtProp.llVal != 0); break;
            case VT_UI8: result = (vtProp.ullVal != 0); break;
            case VT_INT: result = (vtProp.intVal != 0); break;
            case VT_UINT: result = (vtProp.uintVal != 0); break;
            case VT_R4: result = (vtProp.fltVal != 0.0f); break;
            case VT_R8: result = (vtProp.dblVal != 0.0); break;
            case VT_BSTR:
                if (vtProp.bstrVal) {
                    std::wstring str(vtProp.bstrVal);
                    std::transform(str.begin(), str.end(), str.begin(), ::towlower);
                    result = (str == L"true" || str == L"1" || str == L"yes" || str == L"on");
                }
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}

inline double get_double_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    double result = 0.0;

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_R8: result = vtProp.dblVal; break;
            case VT_R4: result = static_cast<double>(vtProp.fltVal); break;
            case VT_I1: result = static_cast<double>(vtProp.cVal); break;
            case VT_UI1: result = static_cast<double>(vtProp.bVal); break;
            case VT_I2: result = static_cast<double>(vtProp.iVal); break;
            case VT_UI2: result = static_cast<double>(vtProp.uiVal); break;
            case VT_I4: result = static_cast<double>(vtProp.lVal); break;
            case VT_UI4: result = static_cast<double>(vtProp.ulVal); break;
            case VT_I8: result = static_cast<double>(vtProp.llVal); break;
            case VT_UI8: result = static_cast<double>(vtProp.ullVal); break;
            case VT_INT: result = static_cast<double>(vtProp.intVal); break;
            case VT_UINT: result = static_cast<double>(vtProp.uintVal); break;
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE) ? 1.0 : 0.0; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = _wtof(vtProp.bstrVal);
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}

inline float get_float_property(IWbemClassObject* obj, const wchar_t* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    float result = 0.0f;

    HRESULT hr = obj->Get(name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr)) {
        switch (vtProp.vt) {
            case VT_R4: result = vtProp.fltVal; break;
            case VT_R8: result = static_cast<float>(vtProp.dblVal); break;
            case VT_I1: result = static_cast<float>(vtProp.cVal); break;
            case VT_UI1: result = static_cast<float>(vtProp.bVal); break;
            case VT_I2: result = static_cast<float>(vtProp.iVal); break;
            case VT_UI2: result = static_cast<float>(vtProp.uiVal); break;
            case VT_I4: result = static_cast<float>(vtProp.lVal); break;
            case VT_UI4: result = static_cast<float>(vtProp.ulVal); break;
            case VT_I8: result = static_cast<float>(vtProp.llVal); break;
            case VT_UI8: result = static_cast<float>(vtProp.ullVal); break;
            case VT_INT: result = static_cast<float>(vtProp.intVal); break;
            case VT_UINT: result = static_cast<float>(vtProp.uintVal); break;
            case VT_BOOL: result = (vtProp.boolVal == VARIANT_TRUE) ? 1.0f : 0.0f; break;
            case VT_BSTR:
                if (vtProp.bstrVal)
                    result = static_cast<float>(_wtof(vtProp.bstrVal));
                break;
        }
    }

    VariantClear(&vtProp);
    return result;
}

inline void initialize_wmi(IWbemLocator** pLoc, IWbemServices** pSvc) {
    HRESULT hr;

    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        throw std::runtime_error("CoInitializeEx failed");
    }

    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL
    );
    if (FAILED(hr) && hr != RPC_E_TOO_LATE && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED)) {
        throw std::runtime_error("CoInitializeSecurity failed");
    }

    hr = CoCreateInstance(
        CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)pLoc
    );
    if (FAILED(hr)) {
        throw std::runtime_error("CoCreateInstance failed");
    }

    BSTR resource = SysAllocString(L"ROOT\\CIMV2");
    if (!resource) throw std::bad_alloc();

    hr = (*pLoc)->ConnectServer(resource, NULL, NULL, 0, 0, 0, 0, pSvc);
    SysFreeString(resource);

    if (FAILED(hr)) {
        throw std::runtime_error("ConnectServer failed");
    }

    hr = CoSetProxyBlanket(
        *pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE
    );
    if (FAILED(hr)) {
        throw std::runtime_error("CoSetProxyBlanket failed");
    }
}

inline void cleanup_wmi(IWbemLocator* pLoc, IWbemServices* pSvc,
                       IEnumWbemClassObject* pEnumerator, IWbemClassObject* pclsObj) {
    if (pclsObj) pclsObj->Release();
    if (pEnumerator) pEnumerator->Release();
    if (pSvc) pSvc->Release();
    if (pLoc) pLoc->Release();
    CoUninitialize();
}

} 

#endif
