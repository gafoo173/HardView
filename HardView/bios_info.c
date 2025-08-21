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


#include "bios_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef BUILD_PYTHON_MODULE
// Helper function to create Python BIOS object
PyObject* create_bios_python_object(const char* manufacturer, const char* version, 
                                   const char* release_date) {
    PyObject* bios_dict = PyDict_New();
    if (!bios_dict) return NULL;
    
    // Add all fields to the dictionary with proper error checking
    PyObject* temp_obj;
    
    temp_obj = PyUnicode_FromString(manufacturer ? manufacturer : "");
    if (temp_obj) {
        PyDict_SetItemString(bios_dict, "manufacturer", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(version ? version : "");
    if (temp_obj) {
        PyDict_SetItemString(bios_dict, "version", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(release_date ? release_date : "");
    if (temp_obj) {
        PyDict_SetItemString(bios_dict, "release_date", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    return bios_dict;
}

// Function that returns Python objects directly
PyObject* get_bios_info_objects(bool Json) {
    if (Json) {
        // JSON mode - return string representation
        char* json_str = get_bios_info(true);
        if (!json_str) return Py_None;
        
        PyObject* py_str = PyUnicode_FromString(json_str);
        free(json_str);
        return py_str ? py_str : Py_None;
    } else {
        // Python objects mode - return object directly
#ifdef _WIN32
        IWbemLocator *pLoc = NULL;
        IWbemServices *pSvc = NULL;
        IEnumWbemClassObject *pEnumerator = NULL;
        IWbemClassObject *pclsObj = NULL;
        HRESULT hr;
        ULONG uReturn = 0;

        hr = _initialize_wmi(&pLoc, &pSvc);
        if (FAILED(hr)) {
            return Py_None;
        }

        BSTR query_lang = SysAllocString(L"WQL");
        BSTR query = SysAllocString(L"SELECT Manufacturer, SMBIOSBIOSVersion, ReleaseDate FROM Win32_BIOS");
        if (!query_lang || !query) {
            SysFreeString(query_lang); SysFreeString(query);
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return Py_None;
        }

        hr = pSvc->lpVtbl->ExecQuery(
            pSvc, query_lang, query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL, &pEnumerator
        );
        SysFreeString(query_lang);
        SysFreeString(query);

        if (FAILED(hr)) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return Py_None;
        }

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* version = _get_wmi_string_property(pclsObj, L"SMBIOSBIOSVersion");
            char* release_date = _get_wmi_string_property(pclsObj, L"ReleaseDate");
            
            PyObject* bios_obj = create_bios_python_object(manufacturer, version, release_date);
            
            free(manufacturer); free(version); free(release_date);
            
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return bios_obj ? bios_obj : Py_None;
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return Py_None;
#else
        // Linux implementation
        char* bios_vendor = _read_dmi_attribute_linux("bios_vendor");
        char* bios_version = _read_dmi_attribute_linux("bios_version");
        char* bios_date = _read_dmi_attribute_linux("bios_date");

        PyObject* bios_obj = create_bios_python_object(bios_vendor, bios_version, bios_date);

        free(bios_vendor); free(bios_version); free(bios_date);

        return bios_obj ? bios_obj : Py_None;
#endif
    }
}
#endif

char* get_bios_info(bool Json) {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* json_result = NULL;
    ULONG uReturn = 0;

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        if (Json) {
            return _create_json_string("{\"error\": \"Failed to initialize WMI for BIOS info: 0x%lx\"}", hr);
        } else {
            return NULL; // Return NULL for Python objects on error
        }
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Manufacturer, SMBIOSBIOSVersion, ReleaseDate FROM Win32_BIOS");
    if (!query_lang || !query) {
        SysFreeString(query_lang); SysFreeString(query);
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        if (Json) {
            return _create_json_string("{\"error\": \"Memory allocation failed for WMI query strings.\"}");
        } else {
            return NULL;
        }
    }

    hr = pSvc->lpVtbl->ExecQuery(
        pSvc, query_lang, query,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator
    );
    SysFreeString(query_lang);
    SysFreeString(query);

    if (FAILED(hr)) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        if (Json) {
            return _create_json_string("{\"error\": \"Failed to execute WMI query for BIOS: 0x%lx\"}", hr);
        } else {
            return NULL;
        }
    }

    if (Json) {
        // JSON mode - original implementation
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* version = _get_wmi_string_property(pclsObj, L"SMBIOSBIOSVersion");
            char* release_date = _get_wmi_string_property(pclsObj, L"ReleaseDate");

            json_result = _create_json_string(
                "{\"manufacturer\": \"%s\", \"version\": \"%s\", \"release_date\": \"%s\"}",
                manufacturer, version, release_date
            );
            free(manufacturer); free(version); free(release_date);
        } else {
            json_result = strdup("{\"error\": \"BIOS information not found in WMI.\"}");
        }

        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return json_result;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* version = _get_wmi_string_property(pclsObj, L"SMBIOSBIOSVersion");
            char* release_date = _get_wmi_string_property(pclsObj, L"ReleaseDate");
            
            PyObject* bios_obj = create_bios_python_object(manufacturer, version, release_date);
            
            free(manufacturer); free(version); free(release_date);
            
            if (bios_obj) {
                PyObject* repr = PyObject_Repr(bios_obj);
                if (!repr) {
                    Py_DECREF(bios_obj);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{}");
                }
                
                const char* repr_str = PyUnicode_AsUTF8(repr);
                char* result = strdup(repr_str ? repr_str : "{}");
                Py_DECREF(repr);
                Py_DECREF(bios_obj);
                
                _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                return result;
            }
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("{}");
#else
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("{}");
#endif
    }

#else
    // Linux implementation
    if (Json) {
        // JSON mode - original implementation
        char* bios_vendor = _read_dmi_attribute_linux("bios_vendor");
        char* bios_version = _read_dmi_attribute_linux("bios_version");
        char* bios_date = _read_dmi_attribute_linux("bios_date");

        char* json_str = _create_json_string(
            "{\"manufacturer\": \"%s\", \"version\": \"%s\", \"release_date\": \"%s\"}",
            bios_vendor, bios_version, bios_date
        );

        free(bios_vendor); free(bios_version); free(bios_date);
        return json_str;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        char* bios_vendor = _read_dmi_attribute_linux("bios_vendor");
        char* bios_version = _read_dmi_attribute_linux("bios_version");
        char* bios_date = _read_dmi_attribute_linux("bios_date");

        PyObject* bios_obj = create_bios_python_object(bios_vendor, bios_version, bios_date);

        free(bios_vendor); free(bios_version); free(bios_date);

        if (bios_obj) {
            PyObject* repr = PyObject_Repr(bios_obj);
            if (!repr) {
                Py_DECREF(bios_obj);
                return strdup("{}");
            }
            
            const char* repr_str = PyUnicode_AsUTF8(repr);
            char* result = strdup(repr_str ? repr_str : "{}");
            Py_DECREF(repr);
            Py_DECREF(bios_obj);
            return result;
        }
        return strdup("{}");
#else
        return strdup("{}");
#endif
    }
#endif
}
