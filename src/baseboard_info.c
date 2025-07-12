#include "baseboard_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>

#ifdef BUILD_PYTHON_MODULE
// Helper function to create Python baseboard object
PyObject* create_baseboard_python_object(const char* manufacturer, const char* product, 
                                        const char* serial_number, const char* version) {
    PyObject* baseboard_dict = PyDict_New();
    if (!baseboard_dict) return NULL;
    
    // Add all fields to the dictionary with proper error checking
    PyObject* temp_obj;
    
    temp_obj = PyUnicode_FromString(manufacturer ? manufacturer : "");
    if (temp_obj) {
        PyDict_SetItemString(baseboard_dict, "manufacturer", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(product ? product : "");
    if (temp_obj) {
        PyDict_SetItemString(baseboard_dict, "product", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(serial_number ? serial_number : "");
    if (temp_obj) {
        PyDict_SetItemString(baseboard_dict, "serial_number", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(version ? version : "");
    if (temp_obj) {
        PyDict_SetItemString(baseboard_dict, "version", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    return baseboard_dict;
}

// Function that returns Python objects directly
PyObject* get_baseboard_info_objects(bool Json) {
    if (Json) {
        // JSON mode - return string representation
        char* json_str = get_baseboard_info(true);
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
        BSTR query = SysAllocString(L"SELECT Manufacturer, Product, SerialNumber, Version FROM Win32_BaseBoard");
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
            char* product = _get_wmi_string_property(pclsObj, L"Product");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* version = _get_wmi_string_property(pclsObj, L"Version");
            
            PyObject* baseboard_obj = create_baseboard_python_object(manufacturer, product, serial_number, version);
            
            free(manufacturer); free(product); free(serial_number); free(version);
            
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return baseboard_obj ? baseboard_obj : Py_None;
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return Py_None;
#else
        // Linux implementation
        char* board_vendor = _read_dmi_attribute_linux("board_vendor");
        char* board_name = _read_dmi_attribute_linux("board_name");
        char* board_serial = _read_dmi_attribute_linux("board_serial");
        char* board_version = _read_dmi_attribute_linux("board_version");

        PyObject* baseboard_obj = create_baseboard_python_object(board_vendor, board_name, board_serial, board_version);

        free(board_vendor); free(board_name); free(board_serial); free(board_version);

        return baseboard_obj ? baseboard_obj : Py_None;
#endif
    }
}
#endif

char* get_baseboard_info(bool Json) {
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
            return _create_json_string("{\"error\": \"Failed to initialize WMI for baseboard info: 0x%lx\"}", hr);
        } else {
            return NULL; // Return NULL for Python objects on error
        }
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Manufacturer, Product, SerialNumber, Version FROM Win32_BaseBoard");
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
            return _create_json_string("{\"error\": \"Failed to execute WMI query for BaseBoard: 0x%lx\"}", hr);
        } else {
            return NULL;
        }
    }

    if (Json) {
        // JSON mode - original implementation
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* product = _get_wmi_string_property(pclsObj, L"Product");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* version = _get_wmi_string_property(pclsObj, L"Version");

            json_result = _create_json_string(
                "{\"manufacturer\": \"%s\", \"product\": \"%s\", \"serial_number\": \"%s\", \"version\": \"%s\"}",
                manufacturer, product, serial_number, version
            );
            free(manufacturer); free(product); free(serial_number); free(version);
        } else {
            json_result = strdup("{\"error\": \"Baseboard information not found in WMI.\"}");
        }

        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return json_result;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* product = _get_wmi_string_property(pclsObj, L"Product");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* version = _get_wmi_string_property(pclsObj, L"Version");
            
            PyObject* baseboard_obj = create_baseboard_python_object(manufacturer, product, serial_number, version);
            
            free(manufacturer); free(product); free(serial_number); free(version);
            
            if (baseboard_obj) {
                PyObject* repr = PyObject_Repr(baseboard_obj);
                if (!repr) {
                    Py_DECREF(baseboard_obj);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{}");
                }
                
                const char* repr_str = PyUnicode_AsUTF8(repr);
                char* result = strdup(repr_str ? repr_str : "{}");
                Py_DECREF(repr);
                Py_DECREF(baseboard_obj);
                
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
        char* board_vendor = _read_dmi_attribute_linux("board_vendor");
        char* board_name = _read_dmi_attribute_linux("board_name");
        char* board_serial = _read_dmi_attribute_linux("board_serial");
        char* board_version = _read_dmi_attribute_linux("board_version");

        char* json_str = _create_json_string(
            "{\"manufacturer\": \"%s\", \"product\": \"%s\", \"serial_number\": \"%s\", \"version\": \"%s\"}",
            board_vendor, board_name, board_serial, board_version
        );

        free(board_vendor); free(board_name); free(board_serial); free(board_version);
        return json_str;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        char* board_vendor = _read_dmi_attribute_linux("board_vendor");
        char* board_name = _read_dmi_attribute_linux("board_name");
        char* board_serial = _read_dmi_attribute_linux("board_serial");
        char* board_version = _read_dmi_attribute_linux("board_version");

        PyObject* baseboard_obj = create_baseboard_python_object(board_vendor, board_name, board_serial, board_version);

        free(board_vendor); free(board_name); free(board_serial); free(board_version);

        if (baseboard_obj) {
            PyObject* repr = PyObject_Repr(baseboard_obj);
            if (!repr) {
                Py_DECREF(baseboard_obj);
                return strdup("{}");
            }
            
            const char* repr_str = PyUnicode_AsUTF8(repr);
            char* result = strdup(repr_str ? repr_str : "{}");
            Py_DECREF(repr);
            Py_DECREF(baseboard_obj);
            return result;
        }
        return strdup("{}");
#else
        return strdup("{}");
#endif
    }
#endif
}
