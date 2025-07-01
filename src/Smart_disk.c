#include "Smart_disk.h"
#include "helpers.h"
#ifdef _WIN32
#include <windows.h>
#include <wbemidl.h>
#include <comdef.h>
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
// Windows-specific helper function to extract uint64 from VARIANT
unsigned long long get_uint64_from_variant(VARIANT* vt) {
    if (vt->vt == VT_I8) return (unsigned long long)vt->llVal;
    if (vt->vt == VT_UI8) return vt->ullVal;
    if (vt->vt == VT_I4) return (unsigned long long)vt->intVal;
    if (vt->vt == VT_UI4) return (unsigned long long)vt->uintVal;
    return 0;
}
#endif

char* get_smart_info_json() {
#ifdef _WIN32
    IWbemLocator *pLoc = NULL;
    IWbemServices *pSvc = NULL;
    IEnumWbemClassObject *pEnumerator = NULL;
    IWbemClassObject *pclsObj = NULL;
    HRESULT hr;
    ULONG uReturn = 0;
    
    // 1. Initialize WMI
    hr = _initialize_wmi(&pLoc, &pSvc);
    if (FAILED(hr)) {
        return strdup("{\"error\": \"Failed to initialize WMI for disk info\"}");
    }
    
    // 2. Execute WMI query to get disk information
    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM Win32_DiskDrive");
    hr = pSvc->lpVtbl->ExecQuery(
        pSvc, query_lang, query,
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL, &pEnumerator
    );
    SysFreeString(query_lang);
    SysFreeString(query);
    if (FAILED(hr)) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("{\"error\": \"Failed to execute WMI query for disk info\"}");
    }
    
    // 3. Iterate through results and extract properties
    size_t bufsize = 8192;
    size_t used = 0;
    char* json = (char*)malloc(bufsize);
    if (!json) {
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("{\"error\": \"Memory allocation failed for disk JSON buffer\"}");
    }
    strcpy(json, "{\"disks\": [");
    used = strlen(json);
    int first = 1;
    
    while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
        // Extract properties
        VARIANT vtProp;
        char model[256] = "", serial[256] = "", interface_type[64] = "", size[64] = "", manufacturer[128] = "", media_type[128] = "", status[64] = "", device_id[256] = "", caption[256] = "", firmware[128] = "", pnp_id[256] = "";
        unsigned long long total_cylinders = 0, total_sectors = 0, total_tracks = 0;
        unsigned int partitions = 0, sectors_per_track = 0, total_heads = 0, tracks_per_cylinder = 0, bytes_per_sector = 0, index = 0, signature = 0;
        
        // Model
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Model", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(model, vtProp.bstrVal, sizeof(model));
        VariantClear(&vtProp);
        
        // SerialNumber
        hr = pclsObj->lpVtbl->Get(pclsObj, L"SerialNumber", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(serial, vtProp.bstrVal, sizeof(serial));
        VariantClear(&vtProp);
        
        // InterfaceType
        hr = pclsObj->lpVtbl->Get(pclsObj, L"InterfaceType", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(interface_type, vtProp.bstrVal, sizeof(interface_type));
        VariantClear(&vtProp);
        
        // Size
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Size", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_BSTR || vtProp.vt == VT_I8 || vtProp.vt == VT_UI8)) {
            if (vtProp.vt == VT_BSTR && vtProp.bstrVal) {
                wcstombs(size, vtProp.bstrVal, sizeof(size));
            } else if (vtProp.vt == VT_I8 || vtProp.vt == VT_UI8) {
                snprintf(size, sizeof(size), "%llu", get_uint64_from_variant(&vtProp));
            }
        }
        VariantClear(&vtProp);
        
        // Partitions
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Partitions", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_UI4)) partitions = vtProp.uintVal;
        VariantClear(&vtProp);
        
        // Manufacturer
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Manufacturer", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(manufacturer, vtProp.bstrVal, sizeof(manufacturer));
        VariantClear(&vtProp);
        
        // MediaType
        hr = pclsObj->lpVtbl->Get(pclsObj, L"MediaType", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(media_type, vtProp.bstrVal, sizeof(media_type));
        VariantClear(&vtProp);
        
        // Status
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Status", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(status, vtProp.bstrVal, sizeof(status));
        VariantClear(&vtProp);
        
        // DeviceID
        hr = pclsObj->lpVtbl->Get(pclsObj, L"DeviceID", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(device_id, vtProp.bstrVal, sizeof(device_id));
        VariantClear(&vtProp);
        
        // Caption
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Caption", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(caption, vtProp.bstrVal, sizeof(caption));
        VariantClear(&vtProp);
        
        // FirmwareRevision
        hr = pclsObj->lpVtbl->Get(pclsObj, L"FirmwareRevision", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(firmware, vtProp.bstrVal, sizeof(firmware));
        VariantClear(&vtProp);
        
        // PNPDeviceID
        hr = pclsObj->lpVtbl->Get(pclsObj, L"PNPDeviceID", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR && vtProp.bstrVal) wcstombs(pnp_id, vtProp.bstrVal, sizeof(pnp_id));
        VariantClear(&vtProp);
        
        // SectorsPerTrack
        hr = pclsObj->lpVtbl->Get(pclsObj, L"SectorsPerTrack", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_UI4)) sectors_per_track = vtProp.uintVal;
        VariantClear(&vtProp);
        
        // TotalCylinders
        hr = pclsObj->lpVtbl->Get(pclsObj, L"TotalCylinders", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I8 || vtProp.vt == VT_UI8)) total_cylinders = get_uint64_from_variant(&vtProp);
        VariantClear(&vtProp);
        
        // TotalHeads
        hr = pclsObj->lpVtbl->Get(pclsObj, L"TotalHeads", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_UI4)) total_heads = vtProp.uintVal;
        VariantClear(&vtProp);
        
        // TotalSectors
        hr = pclsObj->lpVtbl->Get(pclsObj, L"TotalSectors", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I8 || vtProp.vt == VT_UI8)) total_sectors = get_uint64_from_variant(&vtProp);
        VariantClear(&vtProp);
        
        // TotalTracks
        hr = pclsObj->lpVtbl->Get(pclsObj, L"TotalTracks", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I8 || vtProp.vt == VT_UI8)) total_tracks = get_uint64_from_variant(&vtProp);
        VariantClear(&vtProp);
        
        // TracksPerCylinder
        hr = pclsObj->lpVtbl->Get(pclsObj, L"TracksPerCylinder", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_UI4)) tracks_per_cylinder = vtProp.uintVal;
        VariantClear(&vtProp);
        
        // BytesPerSector
        hr = pclsObj->lpVtbl->Get(pclsObj, L"BytesPerSector", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_UI4)) bytes_per_sector = vtProp.uintVal;
        VariantClear(&vtProp);
        
        // Index
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Index", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_UI4)) index = vtProp.uintVal;
        VariantClear(&vtProp);
        
        // Signature
        hr = pclsObj->lpVtbl->Get(pclsObj, L"Signature", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && (vtProp.vt == VT_I4 || vtProp.vt == VT_UI4)) signature = vtProp.uintVal;
        VariantClear(&vtProp);
        
        // Build JSON for this disk
        char* disk_json = _create_json_string(
            "%s{\"model\":\"%s\",\"serial_number\":\"%s\",\"interface_type\":\"%s\",\"size\":\"%s\",\"partitions\":%u,\"manufacturer\":\"%s\",\"media_type\":\"%s\",\"status\":\"%s\",\"device_id\":\"%s\",\"caption\":\"%s\",\"firmware_revision\":\"%s\",\"pnp_device_id\":\"%s\",\"sectors_per_track\":%u,\"total_cylinders\":%llu,\"total_heads\":%u,\"total_sectors\":%llu,\"total_tracks\":%llu,\"tracks_per_cylinder\":%u,\"bytes_per_sector\":%u,\"index\":%u,\"signature\":%u}",
            first ? "" : ",",
            model, serial, interface_type, size, partitions, manufacturer, media_type, status, device_id, caption, firmware, pnp_id, sectors_per_track, total_cylinders, total_heads, total_sectors, total_tracks, tracks_per_cylinder, bytes_per_sector, index, signature
        );
        strcat(json, disk_json);
        used += strlen(disk_json);
        free(disk_json);
        if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
    }
    strcat(json, "]}");
    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return json;
#else
    // Linux implementation - return error message
    return strdup("{\"error\": \"SMART disk info not available on this platform\"}");
#endif
}
