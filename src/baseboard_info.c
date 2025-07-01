#include "baseboard_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>

char* get_baseboard_info_json() {
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

#else
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
