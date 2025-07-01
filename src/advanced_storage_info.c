#include "advanced_storage_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>

// Function to escape backslashes
char* escape_backslashes(const char* input) {
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

char* get_partitions_info_json() {
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
        return _create_json_string("{\"error\": \"Failed to initialize WMI for partitions info: 0x%lx\"}", hr);
    }
    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT DeviceID, Model, SerialNumber, InterfaceType, Size, MediaType FROM Win32_DiskDrive");
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for partitions: 0x%lx\"}", hr);
    }
    // We will collect the results in a JSON array
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
   // ... existing code ...
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
#else
    char* board_vendor = _read_dmi_attribute_linux("board_vendor");
    char* board_name = _read_dmi_attribute_linux("board_name");
    char* board_serial = _read_dmi_attribute_linux("board_serial");
    char* board_version = _read_dmi_attribute_linux("board_version");
    // In Linux: We will use lsblk to output partition information in JSON format
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
#endif
}
