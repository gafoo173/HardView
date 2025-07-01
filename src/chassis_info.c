#include "chassis_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>

char* get_chassis_info_json() {
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
        return _create_json_string("{\"error\": \"Failed to initialize WMI for chassis info: 0x%lx\"}", hr);
    }

    // Try Win32_Chassis first
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

    if (SUCCEEDED(hr) && SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
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
        // Fallback to Win32_SystemEnclosure
        if (pEnumerator) { pEnumerator->lpVtbl->Release(pEnumerator); pEnumerator = NULL; }
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }

        BSTR fallback_query = SysAllocString(L"SELECT Manufacturer, ChassisTypes, SerialNumber, Version FROM Win32_SystemEnclosure");
        if (!fallback_query) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return _create_json_string("{\"error\": \"Memory allocation failed for fallback WMI query.\"}");
        }

        hr = pSvc->lpVtbl->ExecQuery(
            pSvc, SysAllocString(L"WQL"), fallback_query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL, &pEnumerator
        );
        SysFreeString(fallback_query);

        if (SUCCEEDED(hr) && SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* chassis_types = _get_wmi_string_property(pclsObj, L"ChassisTypes");
            char* serial_number = _get_wmi_string_property(pclsObj, L"SerialNumber");
            char* version = _get_wmi_string_property(pclsObj, L"Version");

            // Convert chassis type number to description
            char* type_desc = strdup("Unknown");
            if (strcmp(chassis_types, "N/A") != 0) {
                int type_num = atoi(chassis_types);
                switch (type_num) {
                    case 1: free(type_desc); type_desc = strdup("Other"); break;
                    case 2: free(type_desc); type_desc = strdup("Unknown"); break;
                    case 3: free(type_desc); type_desc = strdup("Desktop"); break;
                    case 4: free(type_desc); type_desc = strdup("Low Profile Desktop"); break;
                    case 5: free(type_desc); type_desc = strdup("Pizza Box"); break;
                    case 6: free(type_desc); type_desc = strdup("Mini Tower"); break;
                    case 7: free(type_desc); type_desc = strdup("Tower"); break;
                    case 8: free(type_desc); type_desc = strdup("Portable"); break;
                    case 9: free(type_desc); type_desc = strdup("Laptop"); break;
                    case 10: free(type_desc); type_desc = strdup("Notebook"); break;
                    case 11: free(type_desc); type_desc = strdup("Hand Held"); break;
                    case 12: free(type_desc); type_desc = strdup("Docking Station"); break;
                    case 13: free(type_desc); type_desc = strdup("All in One"); break;
                    case 14: free(type_desc); type_desc = strdup("Sub Notebook"); break;
                    case 15: free(type_desc); type_desc = strdup("Space-Saving"); break;
                    case 16: free(type_desc); type_desc = strdup("Lunch Box"); break;
                    case 17: free(type_desc); type_desc = strdup("Main System Chassis"); break;
                    case 18: free(type_desc); type_desc = strdup("Expansion Chassis"); break;
                    case 19: free(type_desc); type_desc = strdup("SubChassis"); break;
                    case 20: free(type_desc); type_desc = strdup("Bus Expansion Chassis"); break;
                    case 21: free(type_desc); type_desc = strdup("Peripheral Chassis"); break;
                    case 22: free(type_desc); type_desc = strdup("Storage Chassis"); break;
                    case 23: free(type_desc); type_desc = strdup("Rack Mount Chassis"); break;
                    case 24: free(type_desc); type_desc = strdup("Sealed-Case PC"); break;
                    default: free(type_desc); type_desc = strdup("Unknown"); break;
                }
            }

            json_result = _create_json_string(
                "{\"manufacturer\": \"%s\", \"type\": \"%s\", \"serial_number\": \"%s\", \"version\": \"%s\"}",
                manufacturer, type_desc, serial_number, version
            );
            free(manufacturer); free(chassis_types); free(serial_number); free(version); free(type_desc);
        } else {
            json_result = strdup("{\"error\": \"Chassis information not found in WMI (tried Win32_Chassis and Win32_SystemEnclosure).\"}");
        }
    }

    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return json_result;

#else
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
