#include "chassis_info.h"
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
// Helper function to create Python chassis object
PyObject* create_chassis_python_object(const char* manufacturer, const char* model, 
                                      const char* serial_number, const char* chassis_type) {
    PyObject* chassis_dict = PyDict_New();
    if (!chassis_dict) return NULL;
    
    // Add all fields to the dictionary with proper error checking
    PyObject* temp_obj;
    
    temp_obj = PyUnicode_FromString(manufacturer ? manufacturer : "");
    if (temp_obj) {
        PyDict_SetItemString(chassis_dict, "manufacturer", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(model ? model : "");
    if (temp_obj) {
        PyDict_SetItemString(chassis_dict, "model", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(serial_number ? serial_number : "");
    if (temp_obj) {
        PyDict_SetItemString(chassis_dict, "serial_number", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(chassis_type ? chassis_type : "");
    if (temp_obj) {
        PyDict_SetItemString(chassis_dict, "chassis_type", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    return chassis_dict;
}

// Function that returns Python objects directly
PyObject* get_chassis_info_objects(bool Json) {
    if (Json) {
        // JSON mode - return string representation
        char* json_str = get_chassis_info(true);
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
        BSTR query = SysAllocString(L"SELECT Manufacturer, Model, SerialNumber, ChassisTypes FROM Win32_SystemEnclosure");
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
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* chassis_type = _get_wmi_string_property(pclsObj, L"ChassisTypes");
            
            PyObject* chassis_obj = create_chassis_python_object(manufacturer, model, serial_number, chassis_type);
            
            free(manufacturer); free(model); free(serial_number); free(chassis_type);
            
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return chassis_obj ? chassis_obj : Py_None;
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return Py_None;
#else
        // Linux implementation
        char* chassis_vendor = _read_dmi_attribute_linux("chassis_vendor");
        char* chassis_name = _read_dmi_attribute_linux("chassis_name");
        char* chassis_serial = _read_dmi_attribute_linux("chassis_serial");
        char* chassis_type = _read_dmi_attribute_linux("chassis_type");

        PyObject* chassis_obj = create_chassis_python_object(chassis_vendor, chassis_name, chassis_serial, chassis_type);

        free(chassis_vendor); free(chassis_name); free(chassis_serial); free(chassis_type);

        return chassis_obj ? chassis_obj : Py_None;
#endif
    }
}
#endif

char* get_chassis_info(bool Json) {
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
            return _create_json_string("{\"error\": \"Failed to initialize WMI for chassis info: 0x%lx\"}", hr);
        } else {
            return NULL; // Return NULL for Python objects on error
        }
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Manufacturer, Model, SerialNumber, ChassisTypes FROM Win32_SystemEnclosure");
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
            return _create_json_string("{\"error\": \"Failed to execute WMI query for SystemEnclosure: 0x%lx\"}", hr);
        } else {
            return NULL;
        }
    }

    if (Json) {
        // JSON mode - original implementation
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* chassis_type = _get_wmi_string_property(pclsObj, L"ChassisTypes");

            json_result = _create_json_string(
                "{\"manufacturer\": \"%s\", \"model\": \"%s\", \"serial_number\": \"%s\", \"chassis_type\": \"%s\"}",
                manufacturer, model, serial_number, chassis_type
            );
            free(manufacturer); free(model); free(serial_number); free(chassis_type);
        } else {
            json_result = strdup("{\"error\": \"Chassis information not found in WMI.\"}");
        }

        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return json_result;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* chassis_type = _get_wmi_string_property(pclsObj, L"ChassisTypes");
            
            PyObject* chassis_obj = create_chassis_python_object(manufacturer, model, serial_number, chassis_type);
            
            free(manufacturer); free(model); free(serial_number); free(chassis_type);
            
            if (chassis_obj) {
                PyObject* repr = PyObject_Repr(chassis_obj);
                if (!repr) {
                    Py_DECREF(chassis_obj);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{}");
                }
                
                const char* repr_str = PyUnicode_AsUTF8(repr);
                char* result = strdup(repr_str ? repr_str : "{}");
                Py_DECREF(repr);
                Py_DECREF(chassis_obj);
                
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
        char* chassis_vendor = _read_dmi_attribute_linux("chassis_vendor");
        char* chassis_name = _read_dmi_attribute_linux("chassis_name");
        char* chassis_serial = _read_dmi_attribute_linux("chassis_serial");
        char* chassis_type = _read_dmi_attribute_linux("chassis_type");

        char* json_str = _create_json_string(
            "{\"manufacturer\": \"%s\", \"model\": \"%s\", \"serial_number\": \"%s\", \"chassis_type\": \"%s\"}",
            chassis_vendor, chassis_name, chassis_serial, chassis_type
        );

        free(chassis_vendor); free(chassis_name); free(chassis_serial); free(chassis_type);
        return json_str;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        char* chassis_vendor = _read_dmi_attribute_linux("chassis_vendor");
        char* chassis_name = _read_dmi_attribute_linux("chassis_name");
        char* chassis_serial = _read_dmi_attribute_linux("chassis_serial");
        char* chassis_type = _read_dmi_attribute_linux("chassis_type");

        PyObject* chassis_obj = create_chassis_python_object(chassis_vendor, chassis_name, chassis_serial, chassis_type);

        free(chassis_vendor); free(chassis_name); free(chassis_serial); free(chassis_type);

        if (chassis_obj) {
            PyObject* repr = PyObject_Repr(chassis_obj);
            if (!repr) {
                Py_DECREF(chassis_obj);
                return strdup("{}");
            }
            
            const char* repr_str = PyUnicode_AsUTF8(repr);
            char* result = strdup(repr_str ? repr_str : "{}");
            Py_DECREF(repr);
            Py_DECREF(chassis_obj);
            return result;
        }
        return strdup("{}");
#else
        return strdup("{}");
#endif
    }
#endif
}
