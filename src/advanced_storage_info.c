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

#include <stdio.h>
#include "advanced_storage_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
// Function to escape backslashes - moved to top to avoid implicit declaration
char* escape_backslashes(const char* input) {
    if (!input) return NULL;
    
    size_t len = strlen(input);
    // Worst case: every character is a backslash => double + 1 for null terminator
    char* output = malloc(len * 2 + 1);
    if (!output) return NULL;
    
    char* out = output;
    for (const char* in = input; *in; ++in) {
        if (*in == '\\') *out++ = '\\';
        *out++ = *in;
    }
    *out = 0;
    return output;
}

#ifdef BUILD_PYTHON_MODULE
// Helper function to create Python partition object
PyObject* create_partition_python_object(const char* disk_model, const char* disk_serial, 
                                       const char* disk_interface, long long disk_size,
                                       const char* disk_media, const char* partition_device_id,
                                       const char* partition_type, long long partition_size,
                                       unsigned long partition_index) {
    PyObject* partition_dict = PyDict_New();
    if (!partition_dict) return NULL;
    
    // Add all fields to the dictionary with proper error checking
    PyObject* temp_obj;
    
    temp_obj = PyUnicode_FromString(disk_model ? disk_model : "");
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "disk_model", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(disk_serial ? disk_serial : "");
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "disk_serial", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(disk_interface ? disk_interface : "");
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "disk_interface", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyLong_FromLongLong(disk_size);
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "disk_size", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(disk_media ? disk_media : "");
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "disk_media", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(partition_device_id ? partition_device_id : "");
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "partition_device_id", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(partition_type ? partition_type : "");
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "partition_type", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyLong_FromLongLong(partition_size);
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "partition_size", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyLong_FromUnsignedLong(partition_index);
    if (temp_obj) {
        PyDict_SetItemString(partition_dict, "partition_index", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    return partition_dict;
}

// Function that returns Python objects directly
PyObject* get_partitions_info_objects(bool Json) {
    if (Json) {
        // JSON mode - return string representation
        char* json_str = get_partitions_info(true);
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
        BSTR query = SysAllocString(L"SELECT DeviceID, Model, SerialNumber, InterfaceType, Size, MediaType FROM Win32_DiskDrive");
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
        
        PyObject* partitions_list = PyList_New(0);
        if (!partitions_list) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return Py_None;
        }
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* device_id = _get_wmi_string_property(pclsObj, L"DeviceID");
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* serial = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* interface_type = _get_wmi_string_property(pclsObj, L"InterfaceType");
            LONGLONG size = _get_wmi_longlong_property(pclsObj, L"Size");
            char* media_type = _get_wmi_string_property(pclsObj, L"MediaType");
            
            // Query partitions associated with the disk
            IEnumWbemClassObject* pPartEnum = NULL;
            IWbemClassObject* pPartObj = NULL;
            ULONG uPartRet = 0;
            char assoc_query[512];
            char* device_id_escaped = escape_backslashes(device_id ? device_id : "");
            snprintf(assoc_query, sizeof(assoc_query),
                "ASSOCIATORS OF {Win32_DiskDrive.DeviceID=\"%s\"} WHERE AssocClass=Win32_DiskDriveToDiskPartition",
                device_id_escaped
            );
            free(device_id_escaped);
            BSTR wql_bstr = SysAllocString(L"WQL");
            BSTR assoc_query_bstr = SysAllocStringLen(NULL, strlen(assoc_query));
            mbstowcs(assoc_query_bstr, assoc_query, strlen(assoc_query));
            assoc_query_bstr[strlen(assoc_query)] = 0;
            hr = pSvc->lpVtbl->ExecQuery(
                pSvc, wql_bstr, assoc_query_bstr,
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL, &pPartEnum
            );
            SysFreeString(wql_bstr);
            SysFreeString(assoc_query_bstr);
            
            if (SUCCEEDED(hr)) {
                while (SUCCEEDED(pPartEnum->lpVtbl->Next(pPartEnum, WBEM_INFINITE, 1, &pPartObj, &uPartRet)) && uPartRet == 1) {
                    char* part_device_id = _get_wmi_string_property(pPartObj, L"DeviceID");
                    char* part_type = _get_wmi_string_property(pPartObj, L"Type");
                    LONGLONG part_size = _get_wmi_longlong_property(pPartObj, L"Size");
                    ULONG part_index = _get_wmi_ulong_property(pPartObj, L"Index");
                    
                    PyObject* partition_obj = create_partition_python_object(
                        model, serial, interface_type, size, media_type,
                        part_device_id, part_type, part_size, part_index
                    );
                    
                    if (partition_obj) {
                        int append_result = PyList_Append(partitions_list, partition_obj);
                        if (append_result != 0) {
                            // Error occurred during append
                            Py_DECREF(partition_obj);
                            Py_DECREF(partitions_list);
                            free(part_device_id); free(part_type);
                            if (pPartObj) { pPartObj->lpVtbl->Release(pPartObj); }
                            if (pPartEnum) pPartEnum->lpVtbl->Release(pPartEnum);
                            free(device_id); free(model); free(serial); free(interface_type); free(media_type);
                            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); }
                            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                            return Py_None;
                        }
                        Py_DECREF(partition_obj);
                    }
                    
                    free(part_device_id); free(part_type);
                    if (pPartObj) { pPartObj->lpVtbl->Release(pPartObj); pPartObj = NULL; }
                }
                if (pPartEnum) pPartEnum->lpVtbl->Release(pPartEnum);
            }
            free(device_id); free(model); free(serial); free(interface_type); free(media_type);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return partitions_list;
#else
        // Linux implementation
        PyObject* partitions_list = PyList_New(0);
        if (!partitions_list) {
            return Py_None;
        }
        
        // For Linux, we'll create a simple implementation
        // In a real implementation, you'd parse lsblk output and create objects
        // For now, return empty list
        return partitions_list;
#endif
    }
}
#endif

char* get_partitions_info(bool Json) {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    ULONG uReturn = 0;
    
    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        if (Json) {
            return _create_json_string("{\"error\": \"Failed to initialize WMI for partitions info: 0x%lx\"}", hr);
        } else {
            return NULL; // Return NULL for Python objects on error
        }
    }
    
    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT DeviceID, Model, SerialNumber, InterfaceType, Size, MediaType FROM Win32_DiskDrive");
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
            return _create_json_string("{\"error\": \"Failed to execute WMI query for partitions: 0x%lx\"}", hr);
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
            return strdup("{\"error\": \"Memory allocation failed for partitions JSON buffer.\"}");
        }
        strcpy(json, "{\"partitions\": [");
        used = strlen(json);
        int first = 1;
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* device_id = _get_wmi_string_property(pclsObj, L"DeviceID");
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* serial = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* interface_type = _get_wmi_string_property(pclsObj, L"InterfaceType");
            LONGLONG size = _get_wmi_longlong_property(pclsObj, L"Size");
            char* media_type = _get_wmi_string_property(pclsObj, L"MediaType");
            
            // Query partitions associated with the disk
            IEnumWbemClassObject* pPartEnum = NULL;
            IWbemClassObject* pPartObj = NULL;
            ULONG uPartRet = 0;
            char assoc_query[512];
            char* device_id_escaped = escape_backslashes(device_id ? device_id : "");
            snprintf(assoc_query, sizeof(assoc_query),
                "ASSOCIATORS OF {Win32_DiskDrive.DeviceID=\"%s\"} WHERE AssocClass=Win32_DiskDriveToDiskPartition",
                device_id_escaped
            );
            free(device_id_escaped);
            BSTR wql_bstr = SysAllocString(L"WQL");
            BSTR assoc_query_bstr = SysAllocStringLen(NULL, strlen(assoc_query));
            mbstowcs(assoc_query_bstr, assoc_query, strlen(assoc_query));
            assoc_query_bstr[strlen(assoc_query)] = 0;
            hr = pSvc->lpVtbl->ExecQuery(
                pSvc, wql_bstr, assoc_query_bstr,
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL, &pPartEnum
            );
            SysFreeString(wql_bstr);
            SysFreeString(assoc_query_bstr);
            
            if (SUCCEEDED(hr)) {
                while (SUCCEEDED(pPartEnum->lpVtbl->Next(pPartEnum, WBEM_INFINITE, 1, &pPartObj, &uPartRet)) && uPartRet == 1) {
                    char* part_device_id = _get_wmi_string_property(pPartObj, L"DeviceID");
                    char* part_type = _get_wmi_string_property(pPartObj, L"Type");
                    LONGLONG part_size = _get_wmi_longlong_property(pPartObj, L"Size");
                    ULONG part_index = _get_wmi_ulong_property(pPartObj, L"Index");
                    
                    char* json_part = _create_json_string(
                        "%s{\"disk_model\":\"%s\",\"disk_serial\":\"%s\",\"disk_interface\":\"%s\",\"disk_size\":%lld,\"disk_media\":\"%s\",\"partition_device_id\":\"%s\",\"partition_type\":\"%s\",\"partition_size\":%lld,\"partition_index\":%lu}",
                        first ? "" : ",",
                        model ? model : "",
                        serial ? serial : "",
                        interface_type ? interface_type : "",
                        size,
                        media_type ? media_type : "",
                        part_device_id ? part_device_id : "",
                        part_type ? part_type : "",
                        part_size,
                        part_index
                    );
                    first = 0;
                    size_t part_len = strlen(json_part);
                    if (used + part_len + 3 >= bufsize) {
                        bufsize += part_len + 4096;
                        char* temp = (char*)realloc(json, bufsize);
                        if (!temp) {
                            free(json); free(json_part);
                            if (pPartEnum) pPartEnum->lpVtbl->Release(pPartEnum);
                            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                            return strdup("{\"error\": \"Memory re-allocation failed for partitions JSON buffer.\"}");
                        }
                        json = temp;
                    }
                    strcat(json, json_part);
                    used += part_len;
                    free(json_part);
                    free(part_device_id); free(part_type);
                    if (pPartObj) { pPartObj->lpVtbl->Release(pPartObj); pPartObj = NULL; }
                }
                if (pPartEnum) pPartEnum->lpVtbl->Release(pPartEnum);
            }
            free(device_id); free(model); free(serial); free(interface_type); free(media_type);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        if (first) {
            // No partitions found
            strcpy(json, "{\"partitions\": []}");
        } else {
            strcat(json, "]}");
        }
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return json;
    } else {
        // Python objects mode
#ifdef BUILD_PYTHON_MODULE
        PyObject* partitions_list = PyList_New(0);
        if (!partitions_list) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return NULL;
        }
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* device_id = _get_wmi_string_property(pclsObj, L"DeviceID");
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* serial = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* interface_type = _get_wmi_string_property(pclsObj, L"InterfaceType");
            LONGLONG size = _get_wmi_longlong_property(pclsObj, L"Size");
            char* media_type = _get_wmi_string_property(pclsObj, L"MediaType");
            
            // Query partitions associated with the disk
            IEnumWbemClassObject* pPartEnum = NULL;
            IWbemClassObject* pPartObj = NULL;
            ULONG uPartRet = 0;
            char assoc_query[512];
            char* device_id_escaped = escape_backslashes(device_id ? device_id : "");
            snprintf(assoc_query, sizeof(assoc_query),
                "ASSOCIATORS OF {Win32_DiskDrive.DeviceID=\"%s\"} WHERE AssocClass=Win32_DiskDriveToDiskPartition",
                device_id_escaped
            );
            free(device_id_escaped);
            BSTR wql_bstr = SysAllocString(L"WQL");
            BSTR assoc_query_bstr = SysAllocStringLen(NULL, strlen(assoc_query));
            mbstowcs(assoc_query_bstr, assoc_query, strlen(assoc_query));
            assoc_query_bstr[strlen(assoc_query)] = 0;
            hr = pSvc->lpVtbl->ExecQuery(
                pSvc, wql_bstr, assoc_query_bstr,
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                NULL, &pPartEnum
            );
            SysFreeString(wql_bstr);
            SysFreeString(assoc_query_bstr);
            
            if (SUCCEEDED(hr)) {
                while (SUCCEEDED(pPartEnum->lpVtbl->Next(pPartEnum, WBEM_INFINITE, 1, &pPartObj, &uPartRet)) && uPartRet == 1) {
                    char* part_device_id = _get_wmi_string_property(pPartObj, L"DeviceID");
                    char* part_type = _get_wmi_string_property(pPartObj, L"Type");
                    LONGLONG part_size = _get_wmi_longlong_property(pPartObj, L"Size");
                    ULONG part_index = _get_wmi_ulong_property(pPartObj, L"Index");
                    
                    PyObject* partition_obj = create_partition_python_object(
                        model, serial, interface_type, size, media_type,
                        part_device_id, part_type, part_size, part_index
                    );
                    
                    if (partition_obj) {
                        int append_result = PyList_Append(partitions_list, partition_obj);
                        if (append_result != 0) {
                            // Error occurred during append
                            Py_DECREF(partition_obj);
                            Py_DECREF(partitions_list);
                            free(part_device_id); free(part_type);
                            if (pPartObj) { pPartObj->lpVtbl->Release(pPartObj); }
                            if (pPartEnum) pPartEnum->lpVtbl->Release(pPartEnum);
                            free(device_id); free(model); free(serial); free(interface_type); free(media_type);
                            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); }
                            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                            return NULL;
                        }
                        Py_DECREF(partition_obj);
                    }
                    
                    free(part_device_id); free(part_type);
                    if (pPartObj) { pPartObj->lpVtbl->Release(pPartObj); pPartObj = NULL; }
                }
                if (pPartEnum) pPartEnum->lpVtbl->Release(pPartEnum);
            }
            free(device_id); free(model); free(serial); free(interface_type); free(media_type);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        
        // Convert Python list to string representation for return
        PyObject* repr = PyObject_Repr(partitions_list);
        if (!repr) {
            Py_DECREF(partitions_list);
            return strdup("[]");
        }
        
        const char* repr_str = PyUnicode_AsUTF8(repr);
        char* result = strdup(repr_str ? repr_str : "[]");
        Py_DECREF(repr);
        Py_DECREF(partitions_list);
        return result;
#else
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("[]"); // Return empty list if not building Python module
#endif
    }
#else
    // Linux implementation - similar logic for JSON vs Python objects
    if (Json) {
        // Original Linux JSON implementation
        char* board_vendor = _read_dmi_attribute_linux("board_vendor");
        char* board_name = _read_dmi_attribute_linux("board_name");
        char* board_serial = _read_dmi_attribute_linux("board_serial");
        char* board_version = _read_dmi_attribute_linux("board_version");
        
        FILE* fp = popen("lsblk -J -o NAME,SIZE,MODEL,SERIAL,TYPE,VENDOR", "r");
        if (!fp) {
            free(board_vendor); free(board_name); free(board_serial); free(board_version);
            return _create_json_string("{\"error\": \"Failed to run lsblk for partitions info.\"}");
        }
        
        size_t size = 4096;
        size_t used = 0;
        char* json = malloc(size);
        if (!json) {
            free(board_vendor); free(board_name); free(board_serial); free(board_version);
            pclose(fp);
            return _create_json_string("{\"error\": \"Memory allocation failed for partitions JSON buffer.\"}");
        }
        
        int c;
        while ((c = fgetc(fp)) != EOF) {
            if (used + 1 >= size) {
                size *= 2;
                char* new_json = realloc(json, size);
                if (!new_json) {
                    free(json);
                    free(board_vendor); free(board_name); free(board_serial); free(board_version);
                    pclose(fp);
                    return _create_json_string("{\"error\": \"Memory allocation failed during lsblk output read.\"}");
                }
                json = new_json;
            }
            json[used++] = (char)c;
        }
        json[used] = '\0';
        pclose(fp);
        free(board_vendor); free(board_name); free(board_serial); free(board_version);
        return json;
    } else {
        // Linux Python objects implementation
#ifdef BUILD_PYTHON_MODULE
        PyObject* partitions_list = PyList_New(0);
        if (!partitions_list) {
            return strdup("[]");
        }
        
        // For Linux, we'll create a simple implementation
        // In a real implementation, you'd parse lsblk output and create objects
        Py_DECREF(partitions_list);
        return strdup("[]");
#else
        return strdup("[]");
#endif
    }
#endif
}


