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


#include "disk_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#include <dirent.h>
#include <ctype.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_JSON_BUFFER_SIZE 2048
#define MAX_INFO_LEN 1024

#ifdef BUILD_PYTHON_MODULE
// Helper function to create Python disk object
PyObject* create_disk_python_object(const char* model, const char* manufacturer, 
                                   const char* interface_type, long long size,
                                   const char* serial_number, const char* media_type) {
    PyObject* disk_dict = PyDict_New();
    if (!disk_dict) return NULL;
    
    // Add all fields to the dictionary with proper error checking
    PyObject* temp_obj;
    
    temp_obj = PyUnicode_FromString(model ? model : "");
    if (temp_obj) {
        PyDict_SetItemString(disk_dict, "model", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(manufacturer ? manufacturer : "");
    if (temp_obj) {
        PyDict_SetItemString(disk_dict, "manufacturer", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(interface_type ? interface_type : "");
    if (temp_obj) {
        PyDict_SetItemString(disk_dict, "interface_type", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyLong_FromLongLong(size);
    if (temp_obj) {
        PyDict_SetItemString(disk_dict, "size", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(serial_number ? serial_number : "");
    if (temp_obj) {
        PyDict_SetItemString(disk_dict, "serial_number", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(media_type ? media_type : "");
    if (temp_obj) {
        PyDict_SetItemString(disk_dict, "media_type", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    return disk_dict;
}

// Function that returns Python objects directly
PyObject* get_disk_info_objects(bool Json) {
    if (Json) {
        // JSON mode - return string representation
        char* json_str = get_disk_info(true);
        if (!json_str) return Py_None;
        
        PyObject* py_str = PyUnicode_FromString(json_str);
        free(json_str);
        return py_str ? py_str : Py_None;
    } else {
        // Python objects mode - return objects directly
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
        BSTR query = SysAllocString(L"SELECT Model, Manufacturer, InterfaceType, Size, SerialNumber, MediaType FROM Win32_DiskDrive");
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
        
        PyObject* disks_list = PyList_New(0);
        if (!disks_list) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return Py_None;
        }
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* interface_type = _get_wmi_string_property(pclsObj, L"InterfaceType");
            LONGLONG size = _get_wmi_longlong_property(pclsObj, L"Size");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* media_type = _get_wmi_string_property(pclsObj, L"MediaType");
            
            PyObject* disk_obj = create_disk_python_object(model, manufacturer, interface_type, 
                                                         size, serial_number, media_type);
            
            if (disk_obj) {
                int append_result = PyList_Append(disks_list, disk_obj);
                if (append_result != 0) {
                    // Error occurred during append
                    Py_DECREF(disk_obj);
                    Py_DECREF(disks_list);
                    free(model); free(manufacturer); free(interface_type); 
                    free(serial_number); free(media_type);
                    if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); }
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return Py_None;
                }
                Py_DECREF(disk_obj);
            }
            
            free(model); free(manufacturer); free(interface_type); 
            free(serial_number); free(media_type);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return disks_list;
#else
        // Linux implementation
        PyObject* disks_list = PyList_New(0);
        if (!disks_list) {
            return Py_None;
        }
        
        // For Linux, we'll create a simple implementation
        // In a real implementation, you'd parse lsblk output and create objects
        // For now, return empty list
        return disks_list;
#endif
    }
}
#endif

char* get_disk_info(bool Json) {
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
            return _create_json_string("{\"error\": \"Failed to initialize WMI for disk info: 0x%lx\"}", hr);
        } else {
            return NULL; // Return NULL for Python objects on error
        }
    }
    
    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Model, Manufacturer, InterfaceType, Size, SerialNumber, MediaType FROM Win32_DiskDrive");
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
            return _create_json_string("{\"error\": \"Failed to execute WMI query for disk: 0x%lx\"}", hr);
        } else {
            return NULL;
        }
    }
    
    if (Json) {
        // JSON mode - original implementation
        size_t bufsize = 4096;
        size_t used = 0;
        char* json = (char*)malloc(bufsize);
        if (!json) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return strdup("{\"error\": \"Memory allocation failed for disk JSON buffer.\"}");
        }
        strcpy(json, "{\"disks\": [");
        used = strlen(json);
        int first = 1;
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* interface_type = _get_wmi_string_property(pclsObj, L"InterfaceType");
            LONGLONG size = _get_wmi_longlong_property(pclsObj, L"Size");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* media_type = _get_wmi_string_property(pclsObj, L"MediaType");
            
            char* json_part = _create_json_string(
                "%s{\"model\":\"%s\",\"manufacturer\":\"%s\",\"interface_type\":\"%s\",\"size\":%lld,\"serial_number\":\"%s\",\"media_type\":\"%s\"}",
                first ? "" : ",",
                model ? model : "",
                manufacturer ? manufacturer : "",
                interface_type ? interface_type : "",
                size,
                serial_number ? serial_number : "",
                media_type ? media_type : ""
            );
            first = 0;
            size_t part_len = strlen(json_part);
            if (used + part_len + 3 >= bufsize) {
                bufsize += part_len + 4096;
                char* temp = (char*)realloc(json, bufsize);
                if (!temp) {
                    free(json); free(json_part);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{\"error\": \"Memory re-allocation failed for disk JSON buffer.\"}");
                }
                json = temp;
            }
            strcat(json, json_part);
            used += part_len;
            free(json_part);
            free(model); free(manufacturer); free(interface_type); 
            free(serial_number); free(media_type);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        if (first) {
            // No disks found
            strcpy(json, "{\"disks\": []}");
        } else {
            strcat(json, "]}");
        }
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return json;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        PyObject* disks_list = PyList_New(0);
        if (!disks_list) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return NULL;
        }
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* interface_type = _get_wmi_string_property(pclsObj, L"InterfaceType");
            LONGLONG size = _get_wmi_longlong_property(pclsObj, L"Size");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* media_type = _get_wmi_string_property(pclsObj, L"MediaType");
            
            PyObject* disk_obj = create_disk_python_object(model, manufacturer, interface_type, 
                                                         size, serial_number, media_type);
            
            if (disk_obj) {
                int append_result = PyList_Append(disks_list, disk_obj);
                if (append_result != 0) {
                    // Error occurred during append
                    Py_DECREF(disk_obj);
                    Py_DECREF(disks_list);
                    free(model); free(manufacturer); free(interface_type); 
                    free(serial_number); free(media_type);
                    if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); }
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("[]");
                }
                Py_DECREF(disk_obj);
            }
            
            free(model); free(manufacturer); free(interface_type); 
            free(serial_number); free(media_type);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        if (PyList_Size(disks_list) > 0) {
            PyObject* repr = PyObject_Repr(disks_list);
            if (!repr) {
                Py_DECREF(disks_list);
                _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                return strdup("[]");
            }
            
            const char* repr_str = PyUnicode_AsUTF8(repr);
            char* result = strdup(repr_str ? repr_str : "[]");
            Py_DECREF(repr);
            Py_DECREF(disks_list);
            
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return result;
        } else {
            Py_DECREF(disks_list);
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return strdup("[]");
        }
#else
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("[]");
#endif
    }

#else
    // Linux implementation
    if (Json) {
        // JSON mode - original implementation
        char* disk_model = _read_dmi_attribute_linux("disk_model");
        char* disk_vendor = _read_dmi_attribute_linux("disk_vendor");
        char* disk_interface = _read_dmi_attribute_linux("disk_interface");
        char* disk_size = _read_dmi_attribute_linux("disk_size");
        char* disk_serial = _read_dmi_attribute_linux("disk_serial");
        char* disk_media = _read_dmi_attribute_linux("disk_media");

        char* json_str = _create_json_string(
            "{\"disks\": [{\"model\": \"%s\", \"manufacturer\": \"%s\", \"interface_type\": \"%s\", \"size\": %s, \"serial_number\": \"%s\", \"media_type\": \"%s\"}]}",
            disk_model, disk_vendor, disk_interface, disk_size, disk_serial, disk_media
        );

        free(disk_model); free(disk_vendor); free(disk_interface); 
        free(disk_size); free(disk_serial); free(disk_media);
        return json_str;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        char* disk_model = _read_dmi_attribute_linux("disk_model");
        char* disk_vendor = _read_dmi_attribute_linux("disk_vendor");
        char* disk_interface = _read_dmi_attribute_linux("disk_interface");
        char* disk_size = _read_dmi_attribute_linux("disk_size");
        char* disk_serial = _read_dmi_attribute_linux("disk_serial");
        char* disk_media = _read_dmi_attribute_linux("disk_media");

        // Convert string values to appropriate types
        long long size = disk_size ? atoll(disk_size) : 0;

        PyObject* disk_obj = create_disk_python_object(disk_model, disk_vendor, disk_interface,
                                                      size, disk_serial, disk_media);

        free(disk_model); free(disk_vendor); free(disk_interface); 
        free(disk_size); free(disk_serial); free(disk_media);

        if (disk_obj) {
            PyObject* disk_list = PyList_New(1);
            if (disk_list) {
                PyList_SetItem(disk_list, 0, disk_obj);
                PyObject* repr = PyObject_Repr(disk_list);
                if (!repr) {
                    Py_DECREF(disk_list);
                    return strdup("[]");
                }
                
                const char* repr_str = PyUnicode_AsUTF8(repr);
                char* result = strdup(repr_str ? repr_str : "[]");
                Py_DECREF(repr);
                Py_DECREF(disk_list);
                return result;
            } else {
                Py_DECREF(disk_obj);
                return strdup("[]");
            }
        }
        return strdup("[]");
#else
        return strdup("[]");
#endif
    }
#endif
}
