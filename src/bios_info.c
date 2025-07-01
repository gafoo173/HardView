#include "bios_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* get_bios_info_json() {
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

        char release_date[11];
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

#else
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
