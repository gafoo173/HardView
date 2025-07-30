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


#include "ram_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <Python.h>

#define INITIAL_JSON_BUFFER_SIZE 2048
#define MAX_INFO_LEN 1024

char* get_ram_info(bool Json) {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* current_ram_json_part = NULL;
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    long long total_physical_memory_bytes = 0;
    ULONG uReturn = 0;

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for RAM JSON buffer.\"}");
    strcpy(full_json_string, "{\"total_physical_memory_bytes\": 0, \"memory_modules\": [");
    current_len = strlen(full_json_string);

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to initialize WMI for RAM info: 0x%lx\"}", hr);
    }

    BSTR query_total_mem = SysAllocString(L"SELECT TotalPhysicalMemory FROM Win32_ComputerSystem");
    if (!query_total_mem) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        free(full_json_string);
        return _create_json_string("{\"error\": \"Memory allocation failed for WMI query strings.\"}");
    }

    hr = pSvc->lpVtbl->ExecQuery(pSvc, SysAllocString(L"WQL"), query_total_mem,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    SysFreeString(query_total_mem);

    if (SUCCEEDED(hr) && SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        total_physical_memory_bytes = _get_wmi_longlong_property(pclsObj, L"TotalPhysicalMemory");
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
    }
    if (pEnumerator) { pEnumerator->lpVtbl->Release(pEnumerator); pEnumerator = NULL; }

    char temp_header[MAX_INFO_LEN];
    snprintf(temp_header, sizeof(temp_header), "{\"total_physical_memory_bytes\": %lld, \"memory_modules\": [", total_physical_memory_bytes);
    strcpy(full_json_string, temp_header);
    current_len = strlen(full_json_string);

    BSTR query_modules = SysAllocString(L"SELECT Capacity, Speed, Manufacturer, SerialNumber, PartNumber FROM Win32_PhysicalMemory");
    if (!query_modules) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        free(full_json_string);
        return _create_json_string("{\"error\": \"Memory allocation failed for WMI query strings.\"}");
    }

    hr = pSvc->lpVtbl->ExecQuery(pSvc, SysAllocString(L"WQL"), query_modules,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    SysFreeString(query_modules);

    if (FAILED(hr)) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to execute WMI query for PhysicalMemory: 0x%lx\"}", hr);
    }

    int first_module = 1;
    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        LONGLONG capacity = _get_wmi_longlong_property(pclsObj, L"Capacity");
        ULONG speed = _get_wmi_ulong_property(pclsObj, L"Speed");
        char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
        char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
        char* part_number = _get_wmi_string_property(pclsObj, L"PartNumber");

        current_ram_json_part = _create_json_string(
            "%s{\"capacity_bytes\": %lld, \"speed_mhz\": %lu, \"manufacturer\": \"%s\", \"serial_number\": \"%s\", \"part_number\": \"%s\"}",
            first_module ? "" : ",", capacity, speed, manufacturer, serial_number, part_number
        );
        first_module = 0;

        free(manufacturer); free(serial_number); free(part_number);
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }

        if (current_ram_json_part) {
            size_t part_len = strlen(current_ram_json_part);
            if (current_len + part_len + 3 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_ram_json_part);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{\"error\": \"Memory re-allocation failed for RAM JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_ram_json_part);
            current_len += part_len;
            free(current_ram_json_part);
            current_ram_json_part = NULL;
        }
    }

    strcat(full_json_string, "]}");
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return full_json_string;

#else
    char* mem_total_str = _read_proc_sys_value("/proc/meminfo", "MemTotal:");
    char* mem_free_str = _read_proc_sys_value("/proc/meminfo", "MemFree:");
    char* buffers_str = _read_proc_sys_value("/proc/meminfo", "Buffers:");
    char* cached_str = _read_proc_sys_value("/proc/meminfo", "Cached:");

    long long total_mem_kb = (strcmp(mem_total_str, "N/A") == 0) ? 0 : atoll(mem_total_str);
    long long free_mem_kb = (strcmp(mem_free_str, "N/A") == 0) ? 0 : atoll(mem_free_str);
    long long buffers_kb = (strcmp(buffers_str, "N/A") == 0) ? 0 : atoll(buffers_str);
    long long cached_kb = (strcmp(cached_str, "N/A") == 0) ? 0 : atoll(cached_str);

    char* json_str = _create_json_string(
        "{\"total_mem_bytes\": %lld, \"free_mem_bytes\": %lld, \"buffers_bytes\": %lld, \"cached_bytes\": %lld}",
        total_mem_kb * 1024, free_mem_kb * 1024, buffers_kb * 1024, cached_kb * 1024
    );

    free(mem_total_str); free(mem_free_str); free(buffers_str); free(cached_str);
    return json_str;
#endif
}

PyObject* get_ram_info_objects(bool Json) {
    if (Json) {
        char* json_result = get_ram_info(true);
        PyObject* py_result = PyUnicode_FromString(json_result);
        free(json_result);
        return py_result;
    }

#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    long long total_physical_memory_bytes = 0;
    ULONG uReturn = 0;

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        PyObject* error_dict = PyDict_New();
        PyDict_SetItemString(error_dict, "error", PyUnicode_FromFormat("Failed to initialize WMI for RAM info: 0x%lx", hr));
        return error_dict;
    }

    BSTR query_total_mem = SysAllocString(L"SELECT TotalPhysicalMemory FROM Win32_ComputerSystem");
    if (!query_total_mem) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        PyObject* error_dict = PyDict_New();
        PyDict_SetItemString(error_dict, "error", PyUnicode_FromString("Memory allocation failed for WMI query strings."));
        return error_dict;
    }

    hr = pSvc->lpVtbl->ExecQuery(pSvc, SysAllocString(L"WQL"), query_total_mem,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    SysFreeString(query_total_mem);

    if (SUCCEEDED(hr) && SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        total_physical_memory_bytes = _get_wmi_longlong_property(pclsObj, L"TotalPhysicalMemory");
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
    }
    if (pEnumerator) { pEnumerator->lpVtbl->Release(pEnumerator); pEnumerator = NULL; }

    BSTR query_modules = SysAllocString(L"SELECT Capacity, Speed, Manufacturer, SerialNumber, PartNumber FROM Win32_PhysicalMemory");
    if (!query_modules) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        PyObject* error_dict = PyDict_New();
        PyDict_SetItemString(error_dict, "error", PyUnicode_FromString("Memory allocation failed for WMI query strings."));
        return error_dict;
    }

    hr = pSvc->lpVtbl->ExecQuery(pSvc, SysAllocString(L"WQL"), query_modules,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    SysFreeString(query_modules);

    if (FAILED(hr)) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        PyObject* error_dict = PyDict_New();
        PyDict_SetItemString(error_dict, "error", PyUnicode_FromFormat("Failed to execute WMI query for PhysicalMemory: 0x%lx", hr));
        return error_dict;
    }

    PyObject* result_dict = PyDict_New();
    PyObject* modules_list = PyList_New(0);
    
    PyDict_SetItemString(result_dict, "total_physical_memory_bytes", PyLong_FromLongLong(total_physical_memory_bytes));
    PyDict_SetItemString(result_dict, "memory_modules", modules_list);

    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        LONGLONG capacity = _get_wmi_longlong_property(pclsObj, L"Capacity");
        ULONG speed = _get_wmi_ulong_property(pclsObj, L"Speed");
        char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
        char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
        char* part_number = _get_wmi_string_property(pclsObj, L"PartNumber");

        PyObject* module_dict = PyDict_New();
        PyDict_SetItemString(module_dict, "capacity_bytes", PyLong_FromLongLong(capacity));
        PyDict_SetItemString(module_dict, "speed_mhz", PyLong_FromUnsignedLong(speed));
        PyDict_SetItemString(module_dict, "manufacturer", PyUnicode_FromString(manufacturer ? manufacturer : ""));
        PyDict_SetItemString(module_dict, "serial_number", PyUnicode_FromString(serial_number ? serial_number : ""));
        PyDict_SetItemString(module_dict, "part_number", PyUnicode_FromString(part_number ? part_number : ""));

        PyList_Append(modules_list, module_dict);
        Py_DECREF(module_dict);

        free(manufacturer); free(serial_number); free(part_number);
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
    }

    Py_DECREF(modules_list);
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return result_dict;

#else
    char* mem_total_str = _read_proc_sys_value("/proc/meminfo", "MemTotal:");
    char* mem_free_str = _read_proc_sys_value("/proc/meminfo", "MemFree:");
    char* buffers_str = _read_proc_sys_value("/proc/meminfo", "Buffers:");
    char* cached_str = _read_proc_sys_value("/proc/meminfo", "Cached:");

    long long total_mem_kb = (strcmp(mem_total_str, "N/A") == 0) ? 0 : atoll(mem_total_str);
    long long free_mem_kb = (strcmp(mem_free_str, "N/A") == 0) ? 0 : atoll(mem_free_str);
    long long buffers_kb = (strcmp(buffers_str, "N/A") == 0) ? 0 : atoll(buffers_str);
    long long cached_kb = (strcmp(cached_str, "N/A") == 0) ? 0 : atoll(cached_str);

    PyObject* result_dict = PyDict_New();
    PyDict_SetItemString(result_dict, "total_mem_bytes", PyLong_FromLongLong(total_mem_kb * 1024));
    PyDict_SetItemString(result_dict, "free_mem_bytes", PyLong_FromLongLong(free_mem_kb * 1024));
    PyDict_SetItemString(result_dict, "buffers_bytes", PyLong_FromLongLong(buffers_kb * 1024));
    PyDict_SetItemString(result_dict, "cached_bytes", PyLong_FromLongLong(cached_kb * 1024));

    free(mem_total_str); free(mem_free_str); free(buffers_str); free(cached_str);
    return result_dict;
#endif
}
