#include "gpu_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef __linux__
#include <dirent.h>
#endif
#include <ctype.h>

#ifdef BUILD_PYTHON_MODULE
// Helper function to create Python GPU object
PyObject* create_gpu_python_object(const char* name, const char* manufacturer, 
                                  const char* driver_version, long long memory_size,
                                  const char* video_processor, const char* video_mode_description) {
    PyObject* gpu_dict = PyDict_New();
    if (!gpu_dict) return NULL;
    
    // Add all fields to the dictionary with proper error checking
    PyObject* temp_obj;
    
    temp_obj = PyUnicode_FromString(name ? name : "");
    if (temp_obj) {
        PyDict_SetItemString(gpu_dict, "name", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(manufacturer ? manufacturer : "");
    if (temp_obj) {
        PyDict_SetItemString(gpu_dict, "manufacturer", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(driver_version ? driver_version : "");
    if (temp_obj) {
        PyDict_SetItemString(gpu_dict, "driver_version", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyLong_FromLongLong(memory_size);
    if (temp_obj) {
        PyDict_SetItemString(gpu_dict, "memory_size", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(video_processor ? video_processor : "");
    if (temp_obj) {
        PyDict_SetItemString(gpu_dict, "video_processor", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(video_mode_description ? video_mode_description : "");
    if (temp_obj) {
        PyDict_SetItemString(gpu_dict, "video_mode_description", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    return gpu_dict;
}

// Function that returns Python objects directly
PyObject* get_gpu_info_objects(bool Json) {
    if (Json) {
        // JSON mode - return string representation
        char* json_str = get_gpu_info(true);
        if (!json_str) return Py_None;
        
        PyObject* py_str = PyUnicode_FromString(json_str);
        free(json_str);
        return py_str ? py_str : Py_None;
    } else {
        // Python objects mode - return objects directly
#ifdef _WIN32
        IWbemLocator *pLoc = NULL;
        IWbemServices *pSvc = NULL;
        IEnumWbemClassObject *pEnumerator = NULL;
        IWbemClassObject *pclsObj = NULL;
        HRESULT hr;
        ULONG uReturn = 0;
        
        hr = _initialize_wmi(&pLoc, &pSvc);
        if (FAILED(hr)) {
            return Py_None;
        }
        
        BSTR query_lang = SysAllocString(L"WQL");
        BSTR query = SysAllocString(L"SELECT Name, AdapterCompatibility, DriverVersion, AdapterRAM, VideoProcessor, VideoModeDescription FROM Win32_VideoController");
        if (!query_lang || !query) {
            SysFreeString(query_lang); SysFreeString(query);
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return Py_None;
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
            return Py_None;
        }
        
        PyObject* gpus_list = PyList_New(0);
        if (!gpus_list) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return Py_None;
        }
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* name = _get_wmi_string_property(pclsObj, L"Name");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"AdapterCompatibility");
            char* driver_version = _get_wmi_string_property(pclsObj, L"DriverVersion");
            LONGLONG memory_size = _get_wmi_longlong_property(pclsObj, L"AdapterRAM");
            char* video_processor = _get_wmi_string_property(pclsObj, L"VideoProcessor");
            char* video_mode = _get_wmi_string_property(pclsObj, L"VideoModeDescription");
            
            PyObject* gpu_obj = create_gpu_python_object(name, manufacturer, driver_version, 
                                                        memory_size, video_processor, video_mode);
            
            if (gpu_obj) {
                int append_result = PyList_Append(gpus_list, gpu_obj);
                if (append_result != 0) {
                    // Error occurred during append
                    Py_DECREF(gpu_obj);
                    Py_DECREF(gpus_list);
                    free(name); free(manufacturer); free(driver_version); 
                    free(video_processor); free(video_mode);
                    if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); }
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return Py_None;
                }
                Py_DECREF(gpu_obj);
            }
            
            free(name); free(manufacturer); free(driver_version); 
            free(video_processor); free(video_mode);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return gpus_list;
#else
        // Linux implementation
        PyObject* gpus_list = PyList_New(0);
        if (!gpus_list) {
            return Py_None;
        }
        
        // Read GPU information from /proc/gpuinfo or lspci
        char* gpu_name = _read_dmi_attribute_linux("gpu_name");
        char* gpu_vendor = _read_dmi_attribute_linux("gpu_vendor");
        char* gpu_driver = _read_dmi_attribute_linux("gpu_driver");
        char* gpu_memory = _read_dmi_attribute_linux("gpu_memory");
        char* gpu_processor = _read_dmi_attribute_linux("gpu_processor");
        char* gpu_mode = _read_dmi_attribute_linux("gpu_mode");
        
        // Convert string values to appropriate types
        long long memory_size = gpu_memory ? atoll(gpu_memory) : 0;
        
        PyObject* gpu_obj = create_gpu_python_object(gpu_name, gpu_vendor, gpu_driver,
                                                    memory_size, gpu_processor, gpu_mode);
        
        if (gpu_obj) {
            int append_result = PyList_Append(gpus_list, gpu_obj);
            if (append_result != 0) {
                Py_DECREF(gpu_obj);
                Py_DECREF(gpus_list);
                free(gpu_name); free(gpu_vendor); free(gpu_driver); 
                free(gpu_memory); free(gpu_processor); free(gpu_mode);
                return Py_None;
            }
            Py_DECREF(gpu_obj);
        }
        
        free(gpu_name); free(gpu_vendor); free(gpu_driver); 
        free(gpu_memory); free(gpu_processor); free(gpu_mode);
        
        return gpus_list;
#endif
    }
}
#endif

char* get_gpu_info(bool Json) {
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
        if (Json) {
            return _create_json_string("{\"error\": \"Failed to initialize WMI for GPU info: 0x%lx\"}", hr);
        } else {
            return NULL; // Return NULL for Python objects on error
        }
    }
    
    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Name, AdapterCompatibility, DriverVersion, AdapterRAM, VideoProcessor, VideoModeDescription FROM Win32_VideoController");
    if (!query_lang || !query) {
        SysFreeString(query_lang); SysFreeString(query);
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        if (Json) {
            return _create_json_string("{\"error\": \"Memory allocation failed for WMI query strings.\"}");
        } else {
            return NULL;
        }
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
        if (Json) {
            return _create_json_string("{\"error\": \"Failed to execute WMI query for GPU: 0x%lx\"}", hr);
        } else {
            return NULL;
        }
    }
    
    if (Json) {
        // JSON mode - original implementation
        size_t bufsize = 4096;
        size_t used = 0;
        char* json = (char*)malloc(bufsize);
        if (!json) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return strdup("{\"error\": \"Memory allocation failed for GPU JSON buffer.\"}");
        }
        strcpy(json, "{\"gpus\": [");
        used = strlen(json);
        int first = 1;
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* name = _get_wmi_string_property(pclsObj, L"Name");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"AdapterCompatibility");
            char* driver_version = _get_wmi_string_property(pclsObj, L"DriverVersion");
            LONGLONG memory_size = _get_wmi_longlong_property(pclsObj, L"AdapterRAM");
            char* video_processor = _get_wmi_string_property(pclsObj, L"VideoProcessor");
            char* video_mode = _get_wmi_string_property(pclsObj, L"VideoModeDescription");
            
            char* json_part = _create_json_string(
                "%s{\"name\":\"%s\",\"manufacturer\":\"%s\",\"driver_version\":\"%s\",\"memory_size\":%lld,\"video_processor\":\"%s\",\"video_mode_description\":\"%s\"}",
                first ? "" : ",",
                name ? name : "",
                manufacturer ? manufacturer : "",
                driver_version ? driver_version : "",
                memory_size,
                video_processor ? video_processor : "",
                video_mode ? video_mode : ""
            );
            first = 0;
            size_t part_len = strlen(json_part);
            if (used + part_len + 3 >= bufsize) {
                bufsize += part_len + 4096;
                char* temp = (char*)realloc(json, bufsize);
                if (!temp) {
                    free(json); free(json_part);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{\"error\": \"Memory re-allocation failed for GPU JSON buffer.\"}");
                }
                json = temp;
            }
            strcat(json, json_part);
            used += part_len;
            free(json_part);
            free(name); free(manufacturer); free(driver_version); 
            free(video_processor); free(video_mode);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        if (first) {
            // No GPUs found
            strcpy(json, "{\"gpus\": []}");
        } else {
            strcat(json, "]}");
        }
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return json;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        PyObject* gpus_list = PyList_New(0);
        if (!gpus_list) {
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return NULL;
        }
        
        while (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* name = _get_wmi_string_property(pclsObj, L"Name");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"AdapterCompatibility");
            char* driver_version = _get_wmi_string_property(pclsObj, L"DriverVersion");
            LONGLONG memory_size = _get_wmi_longlong_property(pclsObj, L"AdapterRAM");
            char* video_processor = _get_wmi_string_property(pclsObj, L"VideoProcessor");
            char* video_mode = _get_wmi_string_property(pclsObj, L"VideoModeDescription");
            
            PyObject* gpu_obj = create_gpu_python_object(name, manufacturer, driver_version, 
                                                        memory_size, video_processor, video_mode);
            
            if (gpu_obj) {
                int append_result = PyList_Append(gpus_list, gpu_obj);
                if (append_result != 0) {
                    // Error occurred during append
                    Py_DECREF(gpu_obj);
                    Py_DECREF(gpus_list);
                    free(name); free(manufacturer); free(driver_version); 
                    free(video_processor); free(video_mode);
                    if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); }
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("[]");
                }
                Py_DECREF(gpu_obj);
            }
            
            free(name); free(manufacturer); free(driver_version); 
            free(video_processor); free(video_mode);
            if (pclsObj) { pclsObj->lpVtbl->Release(pclsObj); pclsObj = NULL; }
        }
        
        if (PyList_Size(gpus_list) > 0) {
            PyObject* repr = PyObject_Repr(gpus_list);
            if (!repr) {
                Py_DECREF(gpus_list);
                _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                return strdup("[]");
            }
            
            const char* repr_str = PyUnicode_AsUTF8(repr);
            char* result = strdup(repr_str ? repr_str : "[]");
            Py_DECREF(repr);
            Py_DECREF(gpus_list);
            
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return result;
        } else {
            Py_DECREF(gpus_list);
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return strdup("[]");
        }
#else
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("[]");
#endif
    }

#else
    // Linux implementation
    if (Json) {
        // JSON mode - original implementation
char* gpu_name        = _read_proc_sys_value("/sys/class/drm/card0/device/device", "");        
char* gpu_vendor      = _read_proc_sys_value("/sys/class/drm/card0/device/vendor", "");        
char* gpu_driver      = _read_proc_sys_value("/sys/class/drm/card0/device/uevent", "DRIVER");  
char* gpu_memory      = _read_proc_sys_value("/sys/class/drm/card0/device/mem_info_vram_total", "");
char* gpu_processor   = _read_proc_sys_value("/sys/class/drm/card0/device/uevent", "PCI_ID");  
char* gpu_mode        = _read_proc_sys_value("/sys/class/drm/card0/modes", "");                


        char* json_str = _create_json_string(
            "{\"gpus\": [{\"name\": \"%s\", \"manufacturer\": \"%s\", \"driver_version\": \"%s\", \"memory_size\": %s, \"video_processor\": \"%s\", \"video_mode_description\": \"%s\"}]}",
            gpu_name, gpu_vendor, gpu_driver, gpu_memory, gpu_processor, gpu_mode
        );

        free(gpu_name); free(gpu_vendor); free(gpu_driver); 
        free(gpu_memory); free(gpu_processor); free(gpu_mode);
        return json_str;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
char* gpu_name        = _read_proc_sys_value("/sys/class/drm/card0/device/device", "");        
char* gpu_vendor      = _read_proc_sys_value("/sys/class/drm/card0/device/vendor", "");        
char* gpu_driver      = _read_proc_sys_value("/sys/class/drm/card0/device/uevent", "DRIVER");  
char* gpu_memory      = _read_proc_sys_value("/sys/class/drm/card0/device/mem_info_vram_total", "");
char* gpu_processor   = _read_proc_sys_value("/sys/class/drm/card0/device/uevent", "PCI_ID");
char* gpu_mode        = _read_proc_sys_value("/sys/class/drm/card0/modes", "");                
        // Convert string values to appropriate types
        long long memory_size = gpu_memory ? atoll(gpu_memory) : 0;

        PyObject* gpu_obj = create_gpu_python_object(gpu_name, gpu_vendor, gpu_driver,
                                                    memory_size, gpu_processor, gpu_mode);

        free(gpu_name); free(gpu_vendor); free(gpu_driver); 
        free(gpu_memory); free(gpu_processor); free(gpu_mode);

        if (gpu_obj) {
            PyObject* gpu_list = PyList_New(1);
            if (gpu_list) {
                PyList_SetItem(gpu_list, 0, gpu_obj);
                PyObject* repr = PyObject_Repr(gpu_list);
                if (!repr) {
                    Py_DECREF(gpu_list);
                    return strdup("[]");
                }
                
                const char* repr_str = PyUnicode_AsUTF8(repr);
                char* result = strdup(repr_str ? repr_str : "[]");
                Py_DECREF(repr);
                Py_DECREF(gpu_list);
                return result;
            } else {
                Py_DECREF(gpu_obj);
                return strdup("[]");
            }
        }
        return strdup("[]");
#else
        return strdup("[]");
#endif
    }
#endif
}
