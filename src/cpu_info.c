#include "cpu_info.h"
#include "helpers.h"
#ifdef _WIN32
#include "win_helpers.h"
#else
#include "linux_helpers.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef BUILD_PYTHON_MODULE
// Helper function to create Python CPU object
PyObject* create_cpu_python_object(const char* name, const char* manufacturer, 
                                  const char* architecture, int cores, int threads,
                                  double max_clock_speed, const char* socket_designation) {
    PyObject* cpu_dict = PyDict_New();
    if (!cpu_dict) return NULL;
    
    // Add all fields to the dictionary with proper error checking
    PyObject* temp_obj;
    
    temp_obj = PyUnicode_FromString(name ? name : "");
    if (temp_obj) {
        PyDict_SetItemString(cpu_dict, "name", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(manufacturer ? manufacturer : "");
    if (temp_obj) {
        PyDict_SetItemString(cpu_dict, "manufacturer", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(architecture ? architecture : "");
    if (temp_obj) {
        PyDict_SetItemString(cpu_dict, "architecture", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyLong_FromLong(cores);
    if (temp_obj) {
        PyDict_SetItemString(cpu_dict, "cores", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyLong_FromLong(threads);
    if (temp_obj) {
        PyDict_SetItemString(cpu_dict, "threads", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyFloat_FromDouble(max_clock_speed);
    if (temp_obj) {
        PyDict_SetItemString(cpu_dict, "max_clock_speed", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    temp_obj = PyUnicode_FromString(socket_designation ? socket_designation : "");
    if (temp_obj) {
        PyDict_SetItemString(cpu_dict, "socket_designation", temp_obj);
        Py_DECREF(temp_obj);
    }
    
    return cpu_dict;
}

// Function that returns Python objects directly
PyObject* get_cpu_info_objects(bool Json) {
    if (Json) {
        // JSON mode - return string representation
        char* json_str = get_cpu_info(true);
        if (!json_str) return Py_None;
        
        PyObject* py_str = PyUnicode_FromString(json_str);
        free(json_str);
        return py_str ? py_str : Py_None;
    } else {
        // Python objects mode - return object directly
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
        BSTR query = SysAllocString(L"SELECT Name, Manufacturer, Architecture, NumberOfCores, NumberOfLogicalProcessors, MaxClockSpeed, SocketDesignation FROM Win32_Processor");
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

        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* name = _get_wmi_string_property(pclsObj, L"Name");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* architecture = _get_wmi_string_property(pclsObj, L"Architecture");
            ULONG cores = _get_wmi_ulong_property(pclsObj, L"NumberOfCores");
            ULONG threads = _get_wmi_ulong_property(pclsObj, L"NumberOfLogicalProcessors");
            ULONG max_clock = _get_wmi_ulong_property(pclsObj, L"MaxClockSpeed");
            char* socket_designation = _get_wmi_string_property(pclsObj, L"SocketDesignation");
            
            PyObject* cpu_obj = create_cpu_python_object(name, manufacturer, architecture, 
                                                        cores, threads, max_clock, socket_designation);
            
            free(name); free(manufacturer); free(architecture); free(socket_designation);
            
            _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            return cpu_obj ? cpu_obj : Py_None;
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return Py_None;
#else
        // Linux implementation
        char* cpu_model = _read_dmi_attribute_linux("cpu_model");
        char* cpu_vendor = _read_dmi_attribute_linux("cpu_vendor");
        char* cpu_family = _read_dmi_attribute_linux("cpu_family");
        char* cpu_cores = _read_dmi_attribute_linux("cpu_cores");
        char* cpu_threads = _read_dmi_attribute_linux("cpu_threads");
        char* cpu_max_speed = _read_dmi_attribute_linux("cpu_max_speed");
        char* cpu_socket = _read_dmi_attribute_linux("cpu_socket");

        // Convert string values to appropriate types
        int cores = cpu_cores ? atoi(cpu_cores) : 0;
        int threads = cpu_threads ? atoi(cpu_threads) : 0;
        double max_speed = cpu_max_speed ? atof(cpu_max_speed) : 0.0;

        PyObject* cpu_obj = create_cpu_python_object(cpu_model, cpu_vendor, cpu_family,
                                                    cores, threads, max_speed, cpu_socket);

        free(cpu_model); free(cpu_vendor); free(cpu_family); 
        free(cpu_cores); free(cpu_threads); free(cpu_max_speed); free(cpu_socket);

        return cpu_obj ? cpu_obj : Py_None;
#endif
    }
}
#endif

char* get_cpu_info(bool Json) {
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
            return _create_json_string("{\"error\": \"Failed to initialize WMI for CPU info: 0x%lx\"}", hr);
        } else {
            return NULL; // Return NULL for Python objects on error
        }
    }

    BSTR query_lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Name, Manufacturer, Architecture, NumberOfCores, NumberOfLogicalProcessors, MaxClockSpeed, SocketDesignation FROM Win32_Processor");
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
            return _create_json_string("{\"error\": \"Failed to execute WMI query for CPU: 0x%lx\"}", hr);
        } else {
            return NULL;
        }
    }

    if (Json) {
        // JSON mode - original implementation
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* name = _get_wmi_string_property(pclsObj, L"Name");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* architecture = _get_wmi_string_property(pclsObj, L"Architecture");
            ULONG cores = _get_wmi_ulong_property(pclsObj, L"NumberOfCores");
            ULONG threads = _get_wmi_ulong_property(pclsObj, L"NumberOfLogicalProcessors");
            ULONG max_clock = _get_wmi_ulong_property(pclsObj, L"MaxClockSpeed");
            char* socket_designation = _get_wmi_string_property(pclsObj, L"SocketDesignation");

            json_result = _create_json_string(
                "{\"name\": \"%s\", \"manufacturer\": \"%s\", \"architecture\": \"%s\", \"cores\": %lu, \"threads\": %lu, \"max_clock_speed\": %lu, \"socket_designation\": \"%s\"}",
                name, manufacturer, architecture, cores, threads, max_clock, socket_designation
            );
            free(name); free(manufacturer); free(architecture); free(socket_designation);
        } else {
            json_result = strdup("{\"error\": \"CPU information not found in WMI.\"}");
        }

        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return json_result;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        if (SUCCEEDED(pEnumerator->lpVtbl->Next(pEnumerator, WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn == 1) {
            char* name = _get_wmi_string_property(pclsObj, L"Name");
            char* manufacturer = _get_wmi_string_property(pclsObj, L"Manufacturer");
            char* architecture = _get_wmi_string_property(pclsObj, L"Architecture");
            ULONG cores = _get_wmi_ulong_property(pclsObj, L"NumberOfCores");
            ULONG threads = _get_wmi_ulong_property(pclsObj, L"NumberOfLogicalProcessors");
            ULONG max_clock = _get_wmi_ulong_property(pclsObj, L"MaxClockSpeed");
            char* socket_designation = _get_wmi_string_property(pclsObj, L"SocketDesignation");
            
            PyObject* cpu_obj = create_cpu_python_object(name, manufacturer, architecture, 
                                                        cores, threads, max_clock, socket_designation);
            
            free(name); free(manufacturer); free(architecture); free(socket_designation);
            
            if (cpu_obj) {
                PyObject* repr = PyObject_Repr(cpu_obj);
                if (!repr) {
                    Py_DECREF(cpu_obj);
                    _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                    return strdup("{}");
                }
                
                const char* repr_str = PyUnicode_AsUTF8(repr);
                char* result = strdup(repr_str ? repr_str : "{}");
                Py_DECREF(repr);
                Py_DECREF(cpu_obj);
                
                _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
                return result;
            }
        }
        
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("{}");
#else
        _cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return strdup("{}");
#endif
    }

#else
    // Linux implementation
    if (Json) {
        // JSON mode - original implementation
        char* cpu_model = _read_dmi_attribute_linux("cpu_model");
        char* cpu_vendor = _read_dmi_attribute_linux("cpu_vendor");
        char* cpu_family = _read_dmi_attribute_linux("cpu_family");
        char* cpu_cores = _read_dmi_attribute_linux("cpu_cores");
        char* cpu_threads = _read_dmi_attribute_linux("cpu_threads");
        char* cpu_max_speed = _read_dmi_attribute_linux("cpu_max_speed");
        char* cpu_socket = _read_dmi_attribute_linux("cpu_socket");

        char* json_str = _create_json_string(
            "{\"name\": \"%s\", \"manufacturer\": \"%s\", \"architecture\": \"%s\", \"cores\": %s, \"threads\": %s, \"max_clock_speed\": %s, \"socket_designation\": \"%s\"}",
            cpu_model, cpu_vendor, cpu_family, cpu_cores, cpu_threads, cpu_max_speed, cpu_socket
        );

        free(cpu_model); free(cpu_vendor); free(cpu_family); 
        free(cpu_cores); free(cpu_threads); free(cpu_max_speed); free(cpu_socket);
        return json_str;
    } else {
        // Python objects mode - return string representation of Python object
#ifdef BUILD_PYTHON_MODULE
        char* cpu_model = _read_dmi_attribute_linux("cpu_model");
        char* cpu_vendor = _read_dmi_attribute_linux("cpu_vendor");
        char* cpu_family = _read_dmi_attribute_linux("cpu_family");
        char* cpu_cores = _read_dmi_attribute_linux("cpu_cores");
        char* cpu_threads = _read_dmi_attribute_linux("cpu_threads");
        char* cpu_max_speed = _read_dmi_attribute_linux("cpu_max_speed");
        char* cpu_socket = _read_dmi_attribute_linux("cpu_socket");

        // Convert string values to appropriate types
        int cores = cpu_cores ? atoi(cpu_cores) : 0;
        int threads = cpu_threads ? atoi(cpu_threads) : 0;
        double max_speed = cpu_max_speed ? atof(cpu_max_speed) : 0.0;

        PyObject* cpu_obj = create_cpu_python_object(cpu_model, cpu_vendor, cpu_family,
                                                    cores, threads, max_speed, cpu_socket);

        free(cpu_model); free(cpu_vendor); free(cpu_family); 
        free(cpu_cores); free(cpu_threads); free(cpu_max_speed); free(cpu_socket);

        if (cpu_obj) {
            PyObject* repr = PyObject_Repr(cpu_obj);
            if (!repr) {
                Py_DECREF(cpu_obj);
                return strdup("{}");
            }
            
            const char* repr_str = PyUnicode_AsUTF8(repr);
            char* result = strdup(repr_str ? repr_str : "{}");
            Py_DECREF(repr);
            Py_DECREF(cpu_obj);
            return result;
        }
        return strdup("{}");
#else
        return strdup("{}");
#endif
    }
#endif
}
