#include "disk_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>

#define INITIAL_JSON_BUFFER_SIZE 2048
#define MAX_INFO_LEN 1024

char* get_disk_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* current_disk_json_part = NULL;
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    ULONG uReturn = 0;

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for Disk JSON buffer.\"}");
    strcpy(full_json_string, "{\"disk_drives\": [");
    current_len = strlen(full_json_string);

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to initialize WMI for disk info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Model, SerialNumber, Size, MediaType, InterfaceType FROM Win32_DiskDrive");
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for DiskDrive: 0x%lx\"}", hr);
    }

    int first_disk = 1;
    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        char* model = _get_wmi_string_property(pclsObj, L"Model");
        char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
        LONGLONG size_bytes = _get_wmi_longlong_property(pclsObj, L"Size");
        char* media_type = _get_wmi_string_property(pclsObj, L"MediaType");
        char* interface_type = _get_wmi_string_property(pclsObj, L"InterfaceType");

        current_disk_json_part = _create_json_string(
            "%s{\"model\": \"%s\", \"serial_number\": \"%s\", \"size_bytes\": %lld, \"media_type\": \"%s\", \"interface_type\": \"%s\"}",
            first_disk ? "" : ",", model, serial_number, size_bytes, media_type, interface_type
        );
        first_disk = 0;

        free(model); free(serial_number); free(media_type); free(interface_type);
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }

        if (current_disk_json_part) {
            size_t part_len = strlen(current_disk_json_part);
            if (current_len + part_len + 3 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_disk_json_part);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{\"error\": \"Memory re-allocation failed for Disk JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_disk_json_part);
            current_len += part_len;
            free(current_disk_json_part);
            current_disk_json_part = NULL;
        }
    }

    strcat(full_json_string, "]}");
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return full_json_string;

#else
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    char* current_disk_json_part = NULL;

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for Disk JSON buffer.\"}");
    strcpy(full_json_string, "{\"disk_drives\": [");
    current_len = strlen(full_json_string);

    DIR *dir;
    struct dirent *entry;
    char path[MAX_INFO_LEN];
    int first_disk = 1;

    dir = opendir("/sys/block/");
    if (dir == NULL) {
        free(full_json_string);
        return strdup("{\"error\": \"Failed to open /sys/block/ directory.\"}");
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        if (strlen(entry->d_name) > 3 && (isdigit(entry->d_name[strlen(entry->d_name)-1]) || strchr(entry->d_name, 'p'))) {
            continue;
        }
        char* model = strdup("N/A");
        char* serial = strdup("N/A");
        long long size_bytes = 0;
        char* media_type = strdup("HDD/SSD");
        char* interface_type = strdup("Unknown");
        snprintf(path, sizeof(path), "/sys/block/%s/device/model", entry->d_name);
        char* temp_model = _read_proc_sys_value(path, "");
        if (strcmp(temp_model, "N/A") != 0) { free(model); model = temp_model; } else { free(temp_model); }
        snprintf(path, sizeof(path), "/sys/block/%s/device/serial", entry->d_name);
        char* temp_serial = _read_proc_sys_value(path, "");
        if (strcmp(temp_serial, "N/A") != 0) { free(serial); serial = temp_serial; } else { free(temp_serial); }
        snprintf(path, sizeof(path), "/sys/block/%s/size", entry->d_name);
        char* temp_size_sectors_str = _read_proc_sys_value(path, "");
        if (strcmp(temp_size_sectors_str, "N/A") != 0) {
            size_bytes = atoll(temp_size_sectors_str) * 512;
        }
        free(temp_size_sectors_str);
        current_disk_json_part = _create_json_string(
            "%s{\"name\": \"%s\", \"model\": \"%s\", \"serial_number\": \"%s\", \"size_bytes\": %lld, \"media_type\": \"%s\", \"interface_type\": \"%s\"}",
            first_disk ? "" : ",", entry->d_name, model, serial, size_bytes, media_type, interface_type
        );
        first_disk = 0;
        free(model); free(serial); free(media_type); free(interface_type);
        if (current_disk_json_part) {
            size_t part_len = strlen(current_disk_json_part);
            if (current_len + part_len + 3 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_disk_json_part);
                    closedir(dir);
                    return strdup("{\"error\": \"Memory re-allocation failed for Disk JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_disk_json_part);
            current_len += part_len;
            free(current_disk_json_part);
            current_disk_json_part = NULL;
        }
    }
    closedir(dir);
    strcat(full_json_string, "]}");
    return full_json_string;
#endif
}
