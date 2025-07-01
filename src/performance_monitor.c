#include "performance_monitor.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#include <time.h>
#include <windows.h>
#else
#include "linux_helpers.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* get_cpu_usage_json() {
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
        return _create_json_string("{\"error\": \"Failed to initialize WMI for CPU usage: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT LoadPercentage FROM Win32_Processor");
    if (!query_lang || !query) {
        SysFreeString(query_lang); SysFreeString(query);
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for CPU usage: 0x%lx\"}", hr);
    }

    char* full_json_string = (char*)malloc(2048);
    if (!full_json_string) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("{\"error\": \"Memory allocation failed for CPU usage JSON buffer.\"}");
    }
    strcpy(full_json_string, "{\"cpu_usage\": [");
    size_t current_len = strlen(full_json_string);

    int first_cpu = 1;
    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        ULONG load_percentage = _get_wmi_ulong_property(pclsObj, L"LoadPercentage");

        char* cpu_json_part = _create_json_string(
            "%s{\"load_percentage\": %lu}",
            first_cpu ? "" : ",", load_percentage
        );
        first_cpu = 0;

        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }

        if (cpu_json_part) {
            strcat(full_json_string, cpu_json_part);
            free(cpu_json_part);
        }
    }

    strcat(full_json_string, "]}");
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return full_json_string;

#else
    // Linux implementation - read from /proc/loadavg
    char* loadavg_str = _read_proc_sys_value("/proc/loadavg", "");
    if (strcmp(loadavg_str, "N/A") == 0) {
        free(loadavg_str);
        return strdup("{\"error\": \"Failed to read CPU load average from /proc/loadavg\"}");
    }

    // Parse load average (1, 5, 15 minute averages)
    double load_1min, load_5min, load_15min;
    if (sscanf(loadavg_str, "%lf %lf %lf", &load_1min, &load_5min, &load_15min) == 3) {
        char* json_str = _create_json_string(
            "{\"cpu_usage\": {\"load_1min\": %.2f, \"load_5min\": %.2f, \"load_15min\": %.2f}}",
            load_1min, load_5min, load_15min
        );
        free(loadavg_str);
        return json_str;
    } else {
        free(loadavg_str);
        return strdup("{\"error\": \"Failed to parse CPU load average data\"}");
    }
#endif
}

char* get_ram_usage_json() {
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
        return _create_json_string("{\"error\": \"Failed to initialize WMI for RAM usage: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT TotalVisibleMemorySize, FreePhysicalMemory FROM Win32_OperatingSystem");
    if (!query_lang || !query) {
        SysFreeString(query_lang); SysFreeString(query);
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for RAM usage: 0x%lx\"}", hr);
    }

    if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        LONGLONG total_memory_kb = _get_wmi_longlong_property(pclsObj, L"TotalVisibleMemorySize");
        LONGLONG free_memory_kb = _get_wmi_longlong_property(pclsObj, L"FreePhysicalMemory");

        LONGLONG used_memory_kb = total_memory_kb - free_memory_kb;
        double usage_percent = total_memory_kb > 0 ? (double)used_memory_kb / total_memory_kb * 100.0 : 0.0;

        json_result = _create_json_string(
            "{\"ram_usage\": {\"total_memory_kb\": %lld, \"free_memory_kb\": %lld, \"used_memory_kb\": %lld, \"usage_percent\": %.2f}}",
            total_memory_kb, free_memory_kb, used_memory_kb, usage_percent
        );
    } else {
        json_result = strdup("{\"error\": \"RAM usage information not found in WMI.\"}");
    }

    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return json_result;

#else
    // Linux implementation - read from /proc/meminfo
    char* mem_total_str = _read_proc_sys_value("/proc/meminfo", "MemTotal:");
    char* mem_free_str = _read_proc_sys_value("/proc/meminfo", "MemFree:");
    char* mem_available_str = _read_proc_sys_value("/proc/meminfo", "MemAvailable:");

    long long total_memory_kb = (strcmp(mem_total_str, "N/A") == 0) ? 0 : atoll(mem_total_str);
    long long free_memory_kb = (strcmp(mem_free_str, "N/A") == 0) ? 0 : atoll(mem_free_str);
    long long available_memory_kb = (strcmp(mem_available_str, "N/A") == 0) ? 0 : atoll(mem_available_str);

    long long used_memory_kb = total_memory_kb - available_memory_kb;
    double usage_percent = total_memory_kb > 0 ? (double)used_memory_kb / total_memory_kb * 100.0 : 0.0;

    char* json_str = _create_json_string(
        "{\"ram_usage\": {\"total_memory_kb\": %lld, \"free_memory_kb\": %lld, \"available_memory_kb\": %lld, \"used_memory_kb\": %lld, \"usage_percent\": %.2f}}",
        total_memory_kb, free_memory_kb, available_memory_kb, used_memory_kb, usage_percent
    );

    free(mem_total_str); free(mem_free_str); free(mem_available_str);
    return json_str;
#endif
}

char* get_system_performance_json() {
    char* cpu_usage = get_cpu_usage_json();
    char* ram_usage = get_ram_usage_json();

    // Combine both into a single JSON
    char* combined_json = _create_json_string(
        "{\"system_performance\": {\"cpu\": %s, \"ram\": %s}}",
        cpu_usage, ram_usage
    );

    free(cpu_usage);
    free(ram_usage);

    return combined_json;
}

char* monitor_cpu_usage_duration(int duration_seconds, int interval_ms) {
    if (duration_seconds <= 0 || interval_ms <= 0) {
        return strdup("{\"error\": \"Invalid duration or interval parameters\"}");
    }

    char* full_json_string = (char*)malloc(8192);
    if (!full_json_string) {
        return strdup("{\"error\": \"Memory allocation failed for CPU monitoring JSON buffer.\"}");
    }
    strcpy(full_json_string, "{\"cpu_monitoring\": {\"duration_seconds\": ");
    char temp[64];
    snprintf(temp, sizeof(temp), "%d, \"interval_ms\": %d, \"readings\": [", duration_seconds, interval_ms);
    strcat(full_json_string, temp);

    int readings_count = 0;
    int max_readings = (duration_seconds * 1000) / interval_ms;
    time_t start_time = time(NULL);
    time_t current_time;

    do {
        char* current_reading = get_cpu_usage_json();
        if (current_reading) {
            if (readings_count > 0) {
                strcat(full_json_string, ",");
            }
            strcat(full_json_string, current_reading);
            free(current_reading);
            readings_count++;
        }

        current_time = time(NULL);
        if (current_time - start_time >= duration_seconds) {
            break;
        }

#ifdef _WIN32
        Sleep(interval_ms);
#else
        usleep(interval_ms * 1000);
#endif
    } while (readings_count < max_readings);

    strcat(full_json_string, "]}}");
    return full_json_string;
}

char* monitor_ram_usage_duration(int duration_seconds, int interval_ms) {
    if (duration_seconds <= 0 || interval_ms <= 0) {
        return strdup("{\"error\": \"Invalid duration or interval parameters\"}");
    }

    char* full_json_string = (char*)malloc(8192);
    if (!full_json_string) {
        return strdup("{\"error\": \"Memory allocation failed for RAM monitoring JSON buffer.\"}");
    }
    strcpy(full_json_string, "{\"ram_monitoring\": {\"duration_seconds\": ");
    char temp[64];
    snprintf(temp, sizeof(temp), "%d, \"interval_ms\": %d, \"readings\": [", duration_seconds, interval_ms);
    strcat(full_json_string, temp);

    int readings_count = 0;
    int max_readings = (duration_seconds * 1000) / interval_ms;
    time_t start_time = time(NULL);
    time_t current_time;

    do {
        char* current_reading = get_ram_usage_json();
        if (current_reading) {
            if (readings_count > 0) {
                strcat(full_json_string, ",");
            }
            strcat(full_json_string, current_reading);
            free(current_reading);
            readings_count++;
        }

        current_time = time(NULL);
        if (current_time - start_time >= duration_seconds) {
            break;
        }

#ifdef _WIN32
        Sleep(interval_ms);
#else
        usleep(interval_ms * 1000);
#endif
    } while (readings_count < max_readings);

    strcat(full_json_string, "]}}");
    return full_json_string;
}

char* monitor_system_performance_duration(int duration_seconds, int interval_ms) {
    if (duration_seconds <= 0 || interval_ms <= 0) {
        return strdup("{\"error\": \"Invalid duration or interval parameters\"}");
    }

    char* full_json_string = (char*)malloc(16384);
    if (!full_json_string) {
        return strdup("{\"error\": \"Memory allocation failed for system performance monitoring JSON buffer.\"}");
    }
    strcpy(full_json_string, "{\"system_performance_monitoring\": {\"duration_seconds\": ");
    char temp[64];
    snprintf(temp, sizeof(temp), "%d, \"interval_ms\": %d, \"readings\": [", duration_seconds, interval_ms);
    strcat(full_json_string, temp);

    int readings_count = 0;
    int max_readings = (duration_seconds * 1000) / interval_ms;
    time_t start_time = time(NULL);
    time_t current_time;

    do {
        char* current_reading = get_system_performance_json();
        if (current_reading) {
            if (readings_count > 0) {
                strcat(full_json_string, ",");
            }
            strcat(full_json_string, current_reading);
            free(current_reading);
            readings_count++;
        }

        current_time = time(NULL);
        if (current_time - start_time >= duration_seconds) {
            break;
        }

#ifdef _WIN32
        Sleep(interval_ms);
#else
        usleep(interval_ms * 1000);
#endif
    } while (readings_count < max_readings);

    strcat(full_json_string, "]}}");
    return full_json_string;
}
