#include <stdio.h>    // For input/output operations like fopen, fclose, fgets
#include <stdlib.h>   // For memory allocation functions like malloc, free
#include <string.h>   // For string manipulation functions like strdup, snprintf, strcspn
#include <stdarg.h>   // For variable arguments (va_list, va_start, va_end)

#ifdef _WIN32
// Disable warnings for POSIX functions like strdup, strcpy, etc.
#define _CRT_SECURE_NO_WARNINGS

// CINTERFACE must be defined before including COM headers to make them C-compatible
#define CINTERFACE

#include <winsock2.h>    // Must be included before windows.h to avoid conflicts with winsock.h
#include <ws2tcpip.h>    // For INET6_ADDRSTRLEN, IPv6 support, etc.
#include <windows.h>     // Windows API
#include <objbase.h>     // COM
#include <oleauto.h>     // BSTR, VARIANT, etc.
#include <wtypes.h>      // Basic COM types
#include <Wbemcli.h>     // WMI API
#include <Wbemidl.h>     // WMI interfaces

// The following libraries must be linked when compiling on Windows:
// - Wbemuuid.lib
// - Ole32.lib
// - Oleaut32.lib
// Note: These links are now handled in setup.py for better MinGW support
// #pragma comment(lib, "wbemuuid.lib")
// #pragma comment(lib, "ole32.lib")
// #pragma comment(lib, "oleaut32.lib")

#else

#include <unistd.h>         // POSIX functions
#include <sys/types.h>      // size_t, ssize_t
#include <sys/socket.h>     // sockets
#include <netdb.h>          // gethostbyname, getservbyport
#include <ifaddrs.h>        // getifaddrs, freeifaddrs
#include <arpa/inet.h>      // inet_ntop
#include <net/if_arp.h>     // arpreq structure
#include <net/if.h>         // IF_NAMESIZE
#include <linux/if_packet.h>// sockaddr_ll
#include <sys/ioctl.h>      // ioctl
#include <netinet/in.h>     // sockaddr_in, sockaddr_in6
#include <dirent.h>         // opendir, readdir, closedir
#include <ctype.h>          // isdigit

#define DMI_PATH_PREFIX "/sys/class/dmi/id/"

#endif

// Maximum buffer size for reading DMI info and JSON strings
#define MAX_INFO_LEN 1024

// Initial size for dynamic JSON buffer
#define INITIAL_JSON_BUFFER_SIZE 2048

// ==========================================================
// Shared Helper Functions
// ==========================================================

/**
 * @brief Helper function to dynamically create a JSON string.
 * Handles memory allocation and potential re-allocation.
 * @param format JSON format string.
 * @param ... Values to be inserted into the JSON string.
 * @return A (char*) JSON formatted string. The allocated memory for this string
 * must be freed using free() after use.
 * Returns JSON with an "error" key on failure.
 */
static char* _create_json_string(const char* format, ...) {
    va_list args, args_copy;
    char* json_str = NULL;
    int len = 0;

    // First pass to get the required buffer size
    va_start(args, format);
    va_copy(args_copy, args); // Create a copy for the second pass
    len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (len < 0) {
        // Encoding error or other internal vsnprintf error
        return strdup("{\"error\": \"Failed to estimate JSON string size during formatting.\"}");
    }

    json_str = (char*)malloc(len + 1); // +1 for null terminator
    if (json_str == NULL) {
        // Memory allocation failed
        return strdup("{\"error\": \"Memory allocation failed for JSON string.\"}");
    }

    // Second pass to actually format the string
    va_start(args, format);
    vsnprintf(json_str, len + 1, format, args);
    va_end(args);

    return json_str;
}

// ==========================================================
// Linux Implementation Helpers
// ==========================================================
#ifndef _WIN32

/**
 * @brief Internal helper function to read the content of a specific DMI file on Linux.
 * Allocates memory for the returned string.
 * @param attribute_name The name of the file to read within DMI_PATH_PREFIX.
 * @return A (char*) string containing the file content. The allocated memory for this string
 * must be freed using free() after use.
 * Returns "N/A" if the file cannot be opened or read.
 * Returns "MEMORY_ERROR" if memory allocation fails.
 */
static char* _read_dmi_attribute_linux(const char* attribute_name) {
    char path[MAX_INFO_LEN];
    snprintf(path, sizeof(path), "%s%s", DMI_PATH_PREFIX, attribute_name);

    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return strdup("N/A");
    }

    char* buffer = (char*)malloc(MAX_INFO_LEN);
    if (buffer == NULL) {
        fclose(fp);
        return strdup("MEMORY_ERROR");
    }

    if (fgets(buffer, MAX_INFO_LEN, fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
    } else {
        strcpy(buffer, "N/A");
    }

    fclose(fp);
    return buffer;
}

/**
 * @brief Reads a specific value from a /proc or /sys file on Linux.
 * @param path The full path to the file.
 * @param key The key to look for (e.g., "MemTotal:", "model name:").
 * @return The value associated with the key as a dynamically allocated string,
 * or "N/A" if not found/error. Memory must be freed.
 */
static char* _read_proc_sys_value(const char* path, const char* key) {
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return strdup("N/A");
    }

    char line[MAX_INFO_LEN];
    char* value = strdup("N/A"); // Default value

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(key) == 0 || strstr(line, key) == line) { // Check if line starts with key or if key is empty (to read entire content)
            char* start = line;
            if (strlen(key) > 0) {
                start = strchr(line, ':');
                if (!start) continue;
                start += 1; // Skip the colon
            }

            while (*start == ' ' || *start == '\t') start++; // Skip leading spaces/tabs
            size_t end_ptr = strcspn(start, "\n"); // Find newline character
            if (end_ptr > 0) {
                char temp[MAX_INFO_LEN];
                strncpy(temp, start, end_ptr);
                temp[end_ptr] = '\0';
                free(value);
                value = strdup(temp);
                break;
            }
        }
    }
    fclose(fp);
    return value;
}

#endif // _WIN32

// ==========================================================
// Windows Implementation Helpers (WMI)
// ==========================================================
#ifdef _WIN32

/**
 * @brief Internal helper function to get a WMI property value as a string (BSTR).
 * Handles BSTR to UTF-8 char* conversion.
 * @param pclsObj Pointer to the WMI object (IWbemClassObject*).
 * @param name Name of the property to retrieve (WCHAR*).
 * @return A (char*) string containing the property value. Its allocated memory must be freed using free().
 * Returns "N/A" if the property is not found or an error occurs.
 */
static char* _get_wmi_string_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    char* result = strdup("N/A"); // Default value

    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && (vtProp.vt == VT_BSTR)) {
        int len = WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, NULL, 0, NULL, NULL);
        if (len > 0) {
            char* temp = (char*)malloc(len);
            if (temp) {
                WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, temp, len, NULL, NULL);
                free(result);
                result = temp;
            }
        }
    }
    VariantClear(&vtProp);
    return result;
}

/**
 * @brief Internal helper function to get a WMI property value as an unsigned long.
 * @param pclsObj Pointer to the WMI object (IWbemClassObject*).
 * @param name Name of the property to retrieve (WCHAR*).
 * @return The ULONG value, or 0 if not found/error.
 */
static ULONG _get_wmi_ulong_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    ULONG result = 0;

    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && (vtProp.vt == VT_UI4 || vtProp.vt == VT_I4)) {
        result = vtProp.ulVal;
    } else if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
        // Attempt to convert BSTR to ULONG if it's a number in string form
        result = (ULONG)_wtoi(vtProp.bstrVal);
    }
    VariantClear(&vtProp);
    return result;
}

/**
 * @brief Internal helper function to get a WMI property value as a long long.
 * @param pclsObj Pointer to the WMI object (IWbemClassObject*).
 * @param name Name of the property to retrieve (WCHAR*).
 * @return The LONGLONG value, or 0 if not found/error.
 */
static LONGLONG _get_wmi_longlong_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    LONGLONG result = 0;

    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && (vtProp.vt == VT_UI8 || vtProp.vt == VT_BSTR)) {
        if (vtProp.vt == VT_UI8) {
            result = vtProp.ullVal;
        } else if (vtProp.vt == VT_BSTR) {
            // Convert BSTR to LONGLONG
            result = _wtoi64(vtProp.bstrVal);
        }
    }
    VariantClear(&vtProp);
    return result;
}

/**
 * @brief Internal helper function to get a WMI property value as a boolean.
 * @param pclsObj Pointer to the WMI object (IWbemClassObject*).
 * @param name Name of the property to retrieve (WCHAR*).
 * @return 1 if true, 0 if false or not found/error.
 */
static int _get_wmi_bool_property(IWbemClassObject* pclsObj, const WCHAR* name) {
    VARIANT vtProp;
    VariantInit(&vtProp);
    int result = 0;

    HRESULT hr = pclsObj->lpVtbl->Get(pclsObj, name, 0, &vtProp, 0, 0);
    if (SUCCEEDED(hr) && vtProp.vt == VT_BOOL) {
        result = (vtProp.boolVal == VARIANT_TRUE);
    }
    VariantClear(&vtProp);
    return result;
}

/**
 * @brief Initializes COM, connects to WMI, and sets the security blanket.
 * @param pLoc Pointer to IWbemLocator* to be populated.
 * @param pSvc Pointer to IWbemServices* to be populated.
 * @return HRESULT indicating success or failure.
 */
static HRESULT _initialize_wmi(IWbemLocator **pLoc, IWbemServices **pSvc) {
    HRESULT hr;

    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) { // RPC_E_CHANGED_MODE can happen if COM is already initialized in a different apartment model
        return hr;
    }

    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL
    );
    if (FAILED(hr) && hr != RPC_E_TOO_LATE && hr != HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED)) {
        CoUninitialize(); // Only uninitialize if we initialized it and failed here
        return hr;
    }

    hr = CoCreateInstance(
        &CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
        &IID_IWbemLocator, (LPVOID *)pLoc
    );
    if (FAILED(hr)) {
        CoUninitialize();
        return hr;
    }

    BSTR resource = SysAllocString(L"ROOT\\CIMV2");
    if (!resource) {
        if (*pLoc) (*pLoc)->lpVtbl->Release(*pLoc);
        CoUninitialize();
        return E_OUTOFMEMORY;
    }

    hr = (*pLoc)->lpVtbl->ConnectServer(
        *pLoc, resource, NULL, NULL, 0, 0, NULL, NULL, pSvc
    );
    SysFreeString(resource);

    if (FAILED(hr)) {
        if (*pLoc) (*pLoc)->lpVtbl->Release(*pLoc);
        CoUninitialize();
        return hr;
    }

    hr = CoSetProxyBlanket(
        (IUnknown*)*pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE
    );
    if (FAILED(hr)) {
        if (*pSvc) (*pSvc)->lpVtbl->Release(*pSvc);
        if (*pLoc) (*pLoc)->lpVtbl->Release(*pLoc);
        CoUninitialize();
    }
    return hr;
}

/**
 * @brief Cleans up WMI COM objects.
 * @param pLoc Pointer to IWbemLocator*.
 * @param pSvc Pointer to IWbemServices*.
 * @param pEnumerator Pointer to IEnumWbemClassObject*.
 * @param pclsObj Pointer to IWbemClassObject*.
 */
static void _cleanup_wmi(IWbemLocator *pLoc, IWbemServices *pSvc, IEnumWbemClassObject *pEnumerator, IWbemClassObject *pclsObj) {
    if (pclsObj) pclsObj->lpVtbl->Release(pclsObj);
    if (pEnumerator) pEnumerator->lpVtbl->Release(pEnumerator);
    if (pSvc) pSvc->lpVtbl->Release(pSvc);
    if (pLoc) pLoc->lpVtbl->Release(pLoc);
    CoUninitialize();
}

#endif // _WIN32

// ==========================================================
// Public Functions
// ==========================================================

/**
 * @brief Gathers BIOS information (manufacturer, version, release date)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing BIOS information.
 * The allocated memory for this string must be freed using free() after use.
 */
char* get_bios_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* json_result = NULL;
    ULONG uReturn = 0; // uReturn declared here

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        return _create_json_string("{\"error\": \"Failed to initialize WMI for BIOS info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Manufacturer, SMBIOSBIOSVersion, ReleaseDate FROM Win32_BIOS");
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for BIOS: 0x%lx\"}", hr);
    }

    if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        char* vendor = _get_wmi_string_property(pclsObj, L"Manufacturer");
        char* version = _get_wmi_string_property(pclsObj, L"SMBIOSBIOSVersion");
        char* release_date_wmi = _get_wmi_string_property(pclsObj, L"ReleaseDate");

        char release_date[11]; //YYYY-MM-DD\0
        if (strlen(release_date_wmi) >= 8 && strcmp(release_date_wmi, "N/A") != 0) {
            snprintf(release_date, sizeof(release_date), "%c%c%c%c-%c%c-%c%c",
                     release_date_wmi[0], release_date_wmi[1], release_date_wmi[2], release_date_wmi[3],
                     release_date_wmi[4], release_date_wmi[5],
                     release_date_wmi[6], release_date_wmi[7]);
        } else {
            strcpy(release_date, "N/A");
        }

        json_result = _create_json_string(
            "{\"vendor\": \"%s\", \"version\": \"%s\", \"release_date\": \"%s\"}",
            vendor, version, release_date
        );

        free(vendor); free(version); free(release_date_wmi);
    } else {
        json_result = strdup("{\"error\": \"BIOS information not found in WMI.\"}");
    }

    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return json_result;

#else // Linux implementation
    char* vendor = _read_dmi_attribute_linux("bios_vendor");
    char* version = _read_dmi_attribute_linux("bios_version");
    char* release_date = _read_dmi_attribute_linux("bios_date");

    char* json_str = _create_json_string(
        "{\"vendor\": \"%s\", \"version\": \"%s\", \"release_date\": \"%s\"}",
        vendor, version, release_date
    );

    free(vendor); free(version); free(release_date);
    return json_str;
#endif
}

/**
 * @brief Gathers system information (manufacturer, product name, UUID, serial number)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing system information.
 * The allocated memory for this string must be freed using free() after use.
 */
char* get_system_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* json_result = NULL;
    ULONG uReturn = 0; // uReturn declared here

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        return _create_json_string("{\"error\": \"Failed to initialize WMI for system info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Manufacturer, Name, UUID, IdentifyingNumber FROM Win32_ComputerSystemProduct");
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for System: 0x%lx\"}", hr);
    }

    if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
        char* product_name = _get_wmi_string_property(pclsObj, L"Name");
        char* uuid = _get_wmi_string_property(pclsObj, L"UUID");
        char* serial_number = _get_wmi_string_property(pclsObj, L"IdentifyingNumber");

        json_result = _create_json_string(
            "{\"manufacturer\": \"%s\", \"product_name\": \"%s\", \"uuid\": \"%s\", \"serial_number\": \"%s\"}",
            manufacturer, product_name, uuid, serial_number
        );
        free(manufacturer); free(product_name); free(uuid); free(serial_number);
    } else {
        json_result = strdup("{\"error\": \"System information not found in WMI.\"}");
    }

    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return json_result;

#else // Linux implementation
    char* sys_vendor = _read_dmi_attribute_linux("sys_vendor");
    char* product_name = _read_dmi_attribute_linux("product_name");
    char* product_uuid = _read_dmi_attribute_linux("product_uuid");
    char* product_serial = _read_dmi_attribute_linux("product_serial");

    char* json_str = _create_json_string(
        "{\"manufacturer\": \"%s\", \"product_name\": \"%s\", \"uuid\": \"%s\", \"serial_number\": \"%s\"}",
        sys_vendor, product_name, product_uuid, product_serial
    );

    free(sys_vendor); free(product_name); free(product_uuid); free(product_serial);
    return json_str;
#endif
}

/**
 * @brief Gathers baseboard information (manufacturer, product, serial number, version)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing baseboard information.
 * The allocated memory for this string must be freed using free() after use.
 */
char* get_baseboard_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* json_result = NULL;
    ULONG uReturn = 0; // uReturn declared here

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        return _create_json_string("{\"error\": \"Failed to initialize WMI for baseboard info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Manufacturer, Product, SerialNumber, Version FROM Win32_BaseBoard");
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for BaseBoard: 0x%lx\"}", hr);
    }

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

#else // Linux implementation
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
#endif
}

/**
 * @brief Gathers chassis information (manufacturer, type, serial number, version)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing chassis information.
 * The allocated memory for this string must be freed using free() after use.
 */
char* get_chassis_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* json_result = NULL;
    ULONG uReturn = 0; // uReturn declared here

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        return _create_json_string("{\"error\": \"Failed to initialize WMI for chassis info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Manufacturer, Type, SerialNumber, Version FROM Win32_Chassis");
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for Chassis: 0x%lx\"}", hr);
    }

    if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
        char* type = _get_wmi_string_property(pclsObj, L"Type");
        char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
        char* version = _get_wmi_string_property(pclsObj, L"Version");

        json_result = _create_json_string(
            "{\"manufacturer\": \"%s\", \"type\": \"%s\", \"serial_number\": \"%s\", \"version\": \"%s\"}",
            manufacturer, type, serial_number, version
        );
        free(manufacturer); free(type); free(serial_number); free(version);
    } else {
        json_result = strdup("{\"error\": \"Chassis information not found in WMI.\"}");
    }

    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return json_result;

#else // Linux implementation
    char* chassis_vendor = _read_dmi_attribute_linux("chassis_vendor");
    char* chassis_type = _read_dmi_attribute_linux("chassis_type");
    char* chassis_serial = _read_dmi_attribute_linux("chassis_serial");
    char* chassis_version = _read_dmi_attribute_linux("chassis_version");

    char* json_str = _create_json_string(
        "{\"manufacturer\": \"%s\", \"type\": \"%s\", \"serial_number\": \"%s\", \"version\": \"%s\"}",
        chassis_vendor, chassis_type, chassis_serial, chassis_version
    );

    free(chassis_vendor); free(chassis_type); free(chassis_serial); free(chassis_version);
    return json_str;
#endif
}

/**
 * @brief Gathers CPU information (name, cores, threads, speed, etc.)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing CPU information.
 * The allocated memory for this string must be freed using free() after use.
 */
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
    ULONG uReturn = 0; // uReturn declared here

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
        ULONG max_clock_speed = _get_wmi_ulong_property(pclsObj, L"MaxClockSpeed"); // In MHz
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
            if (current_len + part_len + 1 >= current_buffer_size) { // +1 for closing bracket and null terminator
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE; // Grow by what's needed + a default chunk
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

#else // Linux implementation
    char* model_name = _read_proc_sys_value("/proc/cpuinfo", "model name");
    char* vendor_id = _read_proc_sys_value("/proc/cpuinfo", "vendor_id");
    char* cpu_cores_str = _read_proc_sys_value("/proc/cpuinfo", "cpu cores");
    char* siblings_str = _read_proc_sys_value("/proc/cpuinfo", "siblings");
    char* cpu_mhz_str = _read_proc_sys_value("/proc/cpuinfo", "cpu MHz");

    // Convert string values to numbers
    int cores = (strcmp(cpu_cores_str, "N/A") == 0) ? 0 : atoi(cpu_cores_str);
    int logical_processors = (strcmp(siblings_str, "N/A") == 0) ? 0 : atoi(siblings_str);
    double cpu_mhz = (strcmp(cpu_mhz_str, "N/A") == 0) ? 0.0 : atof(cpu_mhz_str);

    char* json_str = _create_json_string(
        "{\"cpus\": [{\"name\": \"%s\", \"manufacturer\": \"%s\", \"cores\": %d, \"logical_processors\": %d, \"max_clock_speed_mhz\": %.2f, \"processor_id\": \"N/A\"}]}", // ProcessorId not directly available like WMI
        model_name, vendor_id, cores, logical_processors, cpu_mhz
    );

    free(model_name); free(vendor_id); free(cpu_cores_str); free(siblings_str); free(cpu_mhz_str);
    return json_str;
#endif
}

/**
 * @brief Gathers Random Access Memory (RAM) information (total, free, etc., and individual stick info)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing RAM information.
 * The allocated memory for this string must be freed using free() after use.
 */
char* get_ram_info_json() {
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
    long long total_physical_memory_bytes = 0; // From Win32_ComputerSystem
    ULONG uReturn = 0; // uReturn declared here

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for RAM JSON buffer.\"}");
    strcpy(full_json_string, "{\"total_physical_memory_bytes\": 0, \"memory_modules\": [");
    current_len = strlen(full_json_string);


    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to initialize WMI for RAM info: 0x%lx\"}", hr);
    }

    // Get total physical memory first from Win32_ComputerSystem
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

    // Update total_physical_memory_bytes in the initial JSON string
    char temp_header[MAX_INFO_LEN];
    snprintf(temp_header, sizeof(temp_header), "{\"total_physical_memory_bytes\": %lld, \"memory_modules\": [", total_physical_memory_bytes);
    strcpy(full_json_string, temp_header);
    current_len = strlen(full_json_string);


    // Now get individual memory modules
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
            if (current_len + part_len + 3 >= current_buffer_size) { // +3 for "]}" and null terminator
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

#else // Linux implementation
    char* mem_total_str = _read_proc_sys_value("/proc/meminfo", "MemTotal:");
    char* mem_free_str = _read_proc_sys_value("/proc/meminfo", "MemFree:");
    char* buffers_str = _read_proc_sys_value("/proc/meminfo", "Buffers:");
    char* cached_str = _read_proc_sys_value("/proc/meminfo", "Cached:");

    // Convert values from KB to bytes (approximate, removing " kB")
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

/**
 * @brief Gathers disk drive information (model, serial number, size, etc.)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing disk information.
 * The allocated memory for this string must be freed using free() after use.
 */
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
    ULONG uReturn = 0; // uReturn declared here

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
            if (current_len + part_len + 3 >= current_buffer_size) { // +3 for "]}" and null terminator
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

#else // Linux implementation
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    char* current_disk_json_part = NULL;

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for Disk JSON buffer.\"}");
    strcpy(full_json_string, "{\"disk_drives\": [");
    current_len = strlen(full_json_string);

    // Detect disk devices from /sys/block/
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
        if (entry->d_name[0] == '.') continue; // Skip . and .. and hidden files

        // Simple check: if the name contains a digit and is longer than 3 chars (like nvme0n1p1)
        // or ends with a digit. This isn't perfect but covers common cases.
        if (strlen(entry->d_name) > 3 && (isdigit(entry->d_name[strlen(entry->d_name)-1]) || strchr(entry->d_name, 'p'))) {
            continue;
        }


        char* model = strdup("N/A");
        char* serial = strdup("N/A");
        long long size_bytes = 0;
        char* media_type = strdup("HDD/SSD"); // Linux doesn't easily distinguish
        char* interface_type = strdup("Unknown"); // Requires more complex parsing for this

        // Read model
        snprintf(path, sizeof(path), "/sys/block/%s/device/model", entry->d_name);
        char* temp_model = _read_proc_sys_value(path, "");
        if (strcmp(temp_model, "N/A") != 0) { free(model); model = temp_model; } else { free(temp_model); }

        // Read serial number
        snprintf(path, sizeof(path), "/sys/block/%s/device/serial", entry->d_name);
        char* temp_serial = _read_proc_sys_value(path, "");
        if (strcmp(temp_serial, "N/A") != 0) { free(serial); serial = temp_serial; } else { free(temp_serial); }

        // Read size (in 512-byte sectors)
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

/**
 * @brief Gathers network adapter information (name, MAC, IP addresses, etc.)
 * and returns it as a JSON formatted string.
 * @return A (char*) JSON formatted string containing network adapter information.
 * The allocated memory for this string must be freed using free() after use.
 */
char* get_network_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    char* current_net_json_part = NULL;
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    ULONG uReturn = 0; // uReturn declared here

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for Network JSON buffer.\"}");
    strcpy(full_json_string, "{\"network_adapters\": [");
    current_len = strlen(full_json_string);

    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        free(full_json_string);
        return _create_json_string("{\"error\": \"Failed to initialize WMI for network info: 0x%lx\"}", hr);
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Description, MACAddress, IPAddress, DNSHostName, IPEnabled FROM Win32_NetworkAdapterConfiguration WHERE IPEnabled = TRUE");
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
        return _create_json_string("{\"error\": \"Failed to execute WMI query for NetworkAdapter: 0x%lx\"}", hr);
    }

    int first_adapter = 1;
    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        char* description = _get_wmi_string_property(pclsObj, L"Description");
        char* mac_address = _get_wmi_string_property(pclsObj, L"MACAddress");
        char* dns_host_name = _get_wmi_string_property(pclsObj, L"DNSHostName");

        // Handle IPAddress array (VT_ARRAY | VT_BSTR)
        VARIANT vtIpAddresses;
        VariantInit(&vtIpAddresses);
        BSTR ip_addresses_bstr_array_json = SysAllocString(L"[]"); // Default empty array string
        HRESULT hr_ip = pclsObj->lpVtbl->Get(pclsObj, L"IPAddress", 0, &vtIpAddresses, 0, 0);

        if (SUCCEEDED(hr_ip) && (vtIpAddresses.vt == (VT_ARRAY | VT_BSTR)) && vtIpAddresses.parray) {
            SAFEARRAY *sa = vtIpAddresses.parray;
            BSTR *bstr_array;
            hr_ip = SafeArrayAccessData(sa, (void HUGEP* FAR*)&bstr_array);
            if (SUCCEEDED(hr_ip)) {
                long lbound, ubound;
                SafeArrayGetLBound(sa, 1, &lbound);
                SafeArrayGetUBound(sa, 1, &ubound);

                // Estimate max size for internal IP string
                size_t est_ip_list_len = (ubound - lbound + 1) * (INET6_ADDRSTRLEN + 4) + 2; // (IP length + quotes and commas) + brackets
                char* ip_list_buffer = (char*)malloc(est_ip_list_len);
                if (ip_list_buffer) {
                    strcpy(ip_list_buffer, "[");
                    size_t current_ip_list_len = strlen(ip_list_buffer);

                    for (long i = lbound; i <= ubound; i++) {
                        if (i > lbound) {
                            strcat(ip_list_buffer, ",");
                            current_ip_list_len += 1;
                        }
                        char* current_ip_str = NULL;
                        // Convert BSTR to char* (UTF-8)
                        int current_ip_len = WideCharToMultiByte(CP_UTF8, 0, bstr_array[i], -1, NULL, 0, NULL, NULL);
                        if (current_ip_len > 0) {
                            current_ip_str = (char*)malloc(current_ip_len);
                            if (current_ip_str) {
                                WideCharToMultiByte(CP_UTF8, 0, bstr_array[i], -1, current_ip_str, current_ip_len, NULL, NULL);

                                if (current_ip_list_len + strlen(current_ip_str) + 3 < est_ip_list_len) { // +3 for two quotes and null terminator
                                    strcat(ip_list_buffer, "\"");
                                    strcat(ip_list_buffer, current_ip_str);
                                    strcat(ip_list_buffer, "\"");
                                    current_ip_list_len += strlen(current_ip_str) + 2;
                                }
                                free(current_ip_str);
                            }
                        }
                    }
                    strcat(ip_list_buffer, "]");
                    // Convert char* (UTF-8) to WCHAR* (UTF-16) for SysAllocString
                    int wide_len = MultiByteToWideChar(CP_UTF8, 0, ip_list_buffer, -1, NULL, 0);
                    if (wide_len > 0) {
                        WCHAR* wide_ip_list = (WCHAR*)malloc(wide_len * sizeof(WCHAR));
                        if (wide_ip_list) {
                            MultiByteToWideChar(CP_UTF8, 0, ip_list_buffer, -1, wide_ip_list, wide_len);
                            SysFreeString(ip_addresses_bstr_array_json); // Free default
                            ip_addresses_bstr_array_json = SysAllocString(wide_ip_list); // Reassign BSTR
                            free(wide_ip_list);
                        }
                    }
                    free(ip_list_buffer);
                }
                SafeArrayUnaccessData(sa);
            }
        }
        VariantClear(&vtIpAddresses);


        current_net_json_part = _create_json_string(
            "%s{\"description\": \"%s\", \"mac_address\": \"%s\", \"ip_addresses\": %S, \"dns_host_name\": \"%s\"}",
            first_adapter ? "" : ",", description, mac_address, ip_addresses_bstr_array_json, dns_host_name
        );
        first_adapter = 0;

        free(description); free(mac_address); free(dns_host_name);
        SysFreeString(ip_addresses_bstr_array_json); // Free BSTR used for IP addresses
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }


        if (current_net_json_part) {
            size_t part_len = strlen(current_net_json_part);
            if (current_len + part_len + 3 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_net_json_part);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{\"error\": \"Memory re-allocation failed for Network JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_net_json_part);
            current_len += part_len;
            free(current_net_json_part);
            current_net_json_part = NULL;
        }
    }

    strcat(full_json_string, "]}");
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return full_json_string;

#else // Linux implementation
    struct ifaddrs *ifaddr, *ifa;
    char* full_json_string = (char*)malloc(INITIAL_JSON_BUFFER_SIZE);
    size_t current_buffer_size = INITIAL_JSON_BUFFER_SIZE;
    size_t current_len = 0;
    char* current_net_json_part = NULL;

    if (!full_json_string) return strdup("{\"error\": \"Memory allocation failed for Network JSON buffer.\"}");
    strcpy(full_json_string, "{\"network_adapters\": [");
    current_len = strlen(full_json_string);

    if (getifaddrs(&ifaddr) == -1) {
        free(full_json_string);
        return strdup("{\"error\": \"Failed to get network interface addresses (getifaddrs error).\"}");
    }

    int first_adapter = 1;
    char name[IF_NAMESIZE];
    char mac_addr_str[18]; // XX:XX:XX:XX:XX:XX + null
    char ip_v4_list_str[MAX_INFO_LEN];
    char ip_v6_list_str[MAX_INFO_LEN];

    // Use a temporary structure to collect all IP addresses for each interface
    typedef struct {
        char name[IF_NAMESIZE];
        char mac_address[18];
        char ipv4_addresses[MAX_INFO_LEN];
        char ipv6_addresses[MAX_INFO_LEN];
        int processed;
    } NetAdapterInfo;

    // Since we're not using dynamic data structures (like linked lists),
    // we'll use a static array and keep track of the index.
    // This isn't the most robust for a production application but works for the example.
    NetAdapterInfo adapters[32]; // Support for up to 32 adapters
    int num_adapters = 0;

    // Initialize adapters array
    for (int i = 0; i < 32; i++) {
        adapters[i].processed = 0;
        strcpy(adapters[i].mac_address, "N/A");
        strcpy(adapters[i].ipv4_addresses, "[");
        strcpy(adapters[i].ipv6_addresses, "[");
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        int found_adapter_idx = -1;
        for (int i = 0; i < num_adapters; i++) {
            if (strcmp(adapters[i].name, ifa->ifa_name) == 0) {
                found_adapter_idx = i;
                break;
            }
        }

        if (found_adapter_idx == -1 && num_adapters < 32) {
            // New interface
            found_adapter_idx = num_adapters++;
            strncpy(adapters[found_adapter_idx].name, ifa->ifa_name, IF_NAMESIZE - 1);
            adapters[found_adapter_idx].name[IF_NAMESIZE - 1] = '\0';
        } else if (found_adapter_idx == -1) {
            // Adapter buffer overflow
            continue;
        }

        if (ifa->ifa_addr->sa_family == AF_PACKET) {
            struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
            if (s->sll_halen == 6) { // MAC address length
                snprintf(adapters[found_adapter_idx].mac_address, sizeof(adapters[found_adapter_idx].mac_address),
                         "%02x:%02x:%02x:%02x:%02x:%02x",
                         s->sll_addr[0], s->sll_addr[1], s->sll_addr[2],
                         s->sll_addr[3], s->sll_addr[4], s->sll_addr[5]);
            }
        } else if (ifa->ifa_addr->sa_family == AF_INET) {
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr, ip_str, sizeof(ip_str));
            if (strcmp(adapters[found_adapter_idx].ipv4_addresses, "[") != 0) { // Add comma if there are already IPs
                strcat(adapters[found_adapter_idx].ipv4_addresses, ",");
            }
            strcat(adapters[found_adapter_idx].ipv4_addresses, "\"");
            strcat(adapters[found_adapter_idx].ipv4_addresses, ip_str);
            strcat(adapters[found_adapter_idx].ipv4_addresses, "\"");
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            char ip_str[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr, ip_str, sizeof(ip_str));
            if (strcmp(adapters[found_adapter_idx].ipv6_addresses, "[") != 0) {
                strcat(adapters[found_adapter_idx].ipv6_addresses, ",");
            }
            strcat(adapters[found_adapter_idx].ipv6_addresses, "\"");
            strcat(adapters[found_adapter_idx].ipv6_addresses, ip_str);
            strcat(adapters[found_adapter_idx].ipv6_addresses, "\"");
        }
    }
    freeifaddrs(ifaddr);

    // Now construct the final JSON string from the collected information
    for (int i = 0; i < num_adapters; i++) {
        // Close IP address brackets
        strcat(adapters[i].ipv4_addresses, "]");
        strcat(adapters[i].ipv6_addresses, "]");

        current_net_json_part = _create_json_string(
            "%s{\"name\": \"%s\", \"mac_address\": \"%s\", \"ipv4_addresses\": %s, \"ipv6_addresses\": %s}",
            first_adapter ? "" : ",",
            adapters[i].name,
            adapters[i].mac_address,
            adapters[i].ipv4_addresses,
            adapters[i].ipv6_addresses
        );
        first_adapter = 0;

        if (current_net_json_part) {
            size_t part_len = strlen(current_net_json_part);
            if (current_len + part_len + 3 >= current_buffer_size) {
                current_buffer_size += part_len + INITIAL_JSON_BUFFER_SIZE;
                char* temp = (char*)realloc(full_json_string, current_buffer_size);
                if (!temp) {
                    free(full_json_string); free(current_net_json_part);
                    return strdup("{\"error\": \"Memory re-allocation failed for Network JSON buffer.\"}");
                }
                full_json_string = temp;
            }
            strcat(full_json_string, current_net_json_part);
            current_len += part_len;
            free(current_net_json_part);
            current_net_json_part = NULL;
        }
    }

    strcat(full_json_string, "]}");
    return full_json_string;
#endif
}

// Python C API integration
#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject* py_get_bios_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_bios_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_system_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_system_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_baseboard_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_baseboard_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_chassis_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_chassis_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_cpu_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_cpu_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_ram_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_ram_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_disk_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_disk_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_network_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_network_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

// Method definition table
static PyMethodDef HardViewMethods[] = {
    {"get_bios_info", py_get_bios_info_json, METH_NOARGS, "Get BIOS information as a JSON string."},
    {"get_system_info", py_get_system_info_json, METH_NOARGS, "Get system information as a JSON string."},
    {"get_baseboard_info", py_get_baseboard_info_json, METH_NOARGS, "Get baseboard information as a JSON string."},
    {"get_chassis_info", py_get_chassis_info_json, METH_NOARGS, "Get chassis information as a JSON string."},
    {"get_cpu_info", py_get_cpu_info_json, METH_NOARGS, "Get CPU information as a JSON string."},
    {"get_ram_info", py_get_ram_info_json, METH_NOARGS, "Get RAM information as a JSON string."},
    {"get_disk_info", py_get_disk_info_json, METH_NOARGS, "Get disk information as a JSON string."},
    {"get_network_info", py_get_network_info_json, METH_NOARGS, "Get network adapter information as a JSON string."},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef hardviewmodule = {
    PyModuleDef_HEAD_INIT,
    "HardView",   // Module name
    "A module to get hardware information.", // Module documentation, may be NULL
    -1,       // Size of per-interpreter module state, or -1 if module keeps state in global variables.
    HardViewMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_HardView(void) {
    return PyModule_Create(&hardviewmodule);
}

#endif // BUILD_PYTHON_MODULE
