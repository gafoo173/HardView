#include "system_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>

char* get_system_info_json() {
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
        return _create_json_string("{\"error\": \"Failed to initialize WMI for system info: 0x%lx\"}", hr);
    }

    // Try Win32_ComputerSystemProduct first
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

    if (SUCCEEDED(hr) && SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
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
        // Fallback to Win32_ComputerSystem
        if (pEnumerator) { pEnumerator->lpVtbl->Release(pEnumerator); pEnumerator = NULL; }
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }

        BSTR fallback_query = SysAllocString(L"SELECT Manufacturer, Model, Name FROM Win32_ComputerSystem");
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
            char* model = _get_wmi_string_property(pclsObj, L"Model");
            char* name = _get_wmi_string_property(pclsObj, L"Name");

            json_result = _create_json_string(
                "{\"manufacturer\": \"%s\", \"product_name\": \"%s\", \"uuid\": \"N/A\", \"serial_number\": \"N/A\"}",
                manufacturer, model
            );
            free(manufacturer); free(model); free(name);
        } else {
            json_result = strdup("{\"error\": \"System information not found in WMI (tried Win32_ComputerSystemProduct and Win32_ComputerSystem).\"}");
        }
    }

    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return json_result;

#else
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
