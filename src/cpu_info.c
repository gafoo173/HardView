#include "cpu_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>

#define INITIAL_JSON_BUFFER_SIZE 2048

char* get_cpu_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* current_cpu_json_part = NULL;
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    ULONG uReturn = 0;

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for CPU JSON buffer.\"}");
    strcpy(full_json_string, "{\"cpus\": [");
    current_len = strlen(full_json_string);

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to initialize WMI for CPU info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Name, Manufacturer, NumberOfCores, NumberOfLogicalProcessors, MaxClockSpeed, ProcessorId FROM Win32_Processor");
    if (!query_lang || !query) {
        SysFreeString(query_lang); SysFreeString(query);
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        free(full_json_string);
        return _create_json_string("{\"error\": \"Memory allocation failed for WMI query strings.\"}");
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
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to execute WMI query for CPU: 0x%lx\"}", hr);
    }

    int first_cpu = 1;
    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        char* name = _get_wmi_string_property(pclsObj, L"Name");
        char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
        ULONG cores = _get_wmi_ulong_property(pclsObj, L"NumberOfCores");
        ULONG logical_processors = _get_wmi_ulong_property(pclsObj, L"NumberOfLogicalProcessors");
        ULONG max_clock_speed = _get_wmi_ulong_property(pclsObj, L"MaxClockSpeed");
        char* processor_id = _get_wmi_string_property(pclsObj, L"ProcessorId");

        current_cpu_json_part = _create_json_string(
            "%s{\"name\": \"%s\", \"manufacturer\": \"%s\", \"cores\": %lu, \"logical_processors\": %lu, \"max_clock_speed_mhz\": %lu, \"processor_id\": \"%s\"}",
            first_cpu ? "" : ",", name, manufacturer, cores, logical_processors, max_clock_speed, processor_id
        );
        first_cpu = 0;

        free(name); free(manufacturer); free(processor_id);
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }

        if (current_cpu_json_part) {
            size_t part_len = strlen(current_cpu_json_part);
            if (current_len + part_len + 1 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_cpu_json_part);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{\"error\": \"Memory re-allocation failed for CPU JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_cpu_json_part);
            current_len += part_len;
            free(current_cpu_json_part);
            current_cpu_json_part = NULL;
        }
    }

    strcat(full_json_string, "]}");
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return full_json_string;

#else
    char* model_name = _read_proc_sys_value("/proc/cpuinfo", "model name");
    char* vendor_id = _read_proc_sys_value("/proc/cpuinfo", "vendor_id");
    char* cpu_cores_str = _read_proc_sys_value("/proc/cpuinfo", "cpu cores");
    char* siblings_str = _read_proc_sys_value("/proc/cpuinfo", "siblings");
    char* cpu_mhz_str = _read_proc_sys_value("/proc/cpuinfo", "cpu MHz");

    int cores = (strcmp(cpu_cores_str, "N/A") == 0) ? 0 : atoi(cpu_cores_str);
    int logical_processors = (strcmp(siblings_str, "N/A") == 0) ? 0 : atoi(siblings_str);
    double cpu_mhz = (strcmp(cpu_mhz_str, "N/A") == 0) ? 0.0 : atof(cpu_mhz_str);

    char* json_str = _create_json_string(
        "{\"cpus\": [{\"name\": \"%s\", \"manufacturer\": \"%s\", \"cores\": %d, \"logical_processors\": %d, \"max_clock_speed_mhz\": %.2f, \"processor_id\": \"N/A\"}]}",
        model_name, vendor_id, cores, logical_processors, cpu_mhz
    );

    free(model_name); free(vendor_id); free(cpu_cores_str); free(siblings_str); free(cpu_mhz_str);
    return json_str;
#endif
}
