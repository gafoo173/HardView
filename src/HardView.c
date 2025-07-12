#include "HardView.h"
#include "bios_info.h"
#include "system_info.h"
#include "baseboard_info.h"
#include "chassis_info.h"
#include "cpu_info.h"
#include "gpu_info.h"
#include "ram_info.h"
#include "disk_info.h"
#include "network_info.h"
#include "performance_monitor.h"
#include "advanced_storage_info.h"
#include "Smart_disk.h"

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject* py_get_bios_info(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_bios_info() takes 0 or 1 arguments");
        return NULL;
    }
    
    char* result = get_bios_info(json_mode);
    if (!result) Py_RETURN_NONE;
    
    if (json_mode) {
        // JSON mode - return as string
        PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
        free(result);
        if (!py_str) {
            PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
            return NULL;
        }
        return py_str;
    } else {
        // Python objects mode - result is a string representation of Python objects
        // We need to evaluate it to get the actual Python objects
        PyObject* main_module = PyImport_ImportModule("__main__");
        if (!main_module) {
            free(result);
            PyErr_SetString(PyExc_ImportError, "Failed to import __main__ module");
            return NULL;
        }
        
        PyObject* globals = PyModule_GetDict(main_module);
        Py_DECREF(main_module);
        
        PyObject* py_objects = PyRun_String(result, Py_eval_input, globals, globals);
        free(result);
        
        if (!py_objects) {
            PyErr_SetString(PyExc_ValueError, "Failed to evaluate Python objects string");
            return NULL;
        }
        
        return py_objects;
    }
}

static PyObject* py_get_bios_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_bios_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_bios_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_system_info(PyObject* self, PyObject* args) {
    char* result = get_system_info(true);
    if (!result) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
    free(result);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_system_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_system_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_system_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_baseboard_info(PyObject* self, PyObject* args) {
    char* result = get_baseboard_info(true);
    if (!result) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
    free(result);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_chassis_info(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_chassis_info() takes 0 or 1 arguments");
        return NULL;
    }
    
    char* result = get_chassis_info(json_mode);
    if (!result) Py_RETURN_NONE;
    
    if (json_mode) {
        // JSON mode - return as string
        PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
        free(result);
        if (!py_str) {
            PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
            return NULL;
        }
        return py_str;
    } else {
        // Python objects mode - result is a string representation of Python objects
        // We need to evaluate it to get the actual Python objects
        PyObject* main_module = PyImport_ImportModule("__main__");
        if (!main_module) {
            free(result);
            PyErr_SetString(PyExc_ImportError, "Failed to import __main__ module");
            return NULL;
        }
        
        PyObject* globals = PyModule_GetDict(main_module);
        Py_DECREF(main_module);
        
        PyObject* py_objects = PyRun_String(result, Py_eval_input, globals, globals);
        free(result);
        
        if (!py_objects) {
            PyErr_SetString(PyExc_ValueError, "Failed to evaluate Python objects string");
            return NULL;
        }
        
        return py_objects;
    }
}

static PyObject* py_get_chassis_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_chassis_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_chassis_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_cpu_info(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_cpu_info() takes 0 or 1 arguments");
        return NULL;
    }
    
    char* result = get_cpu_info(json_mode);
    if (!result) Py_RETURN_NONE;
    
    if (json_mode) {
        // JSON mode - return as string
        PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
        free(result);
        if (!py_str) {
            PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
            return NULL;
        }
        return py_str;
    } else {
        // Python objects mode - result is a string representation of Python objects
        // We need to evaluate it to get the actual Python objects
        PyObject* main_module = PyImport_ImportModule("__main__");
        if (!main_module) {
            free(result);
            PyErr_SetString(PyExc_ImportError, "Failed to import __main__ module");
            return NULL;
        }
        
        PyObject* globals = PyModule_GetDict(main_module);
        Py_DECREF(main_module);
        
        PyObject* py_objects = PyRun_String(result, Py_eval_input, globals, globals);
        free(result);
        
        if (!py_objects) {
            PyErr_SetString(PyExc_ValueError, "Failed to evaluate Python objects string");
            return NULL;
        }
        
        return py_objects;
    }
}

static PyObject* py_get_cpu_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_cpu_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_cpu_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_ram_info(PyObject* self, PyObject* args) {
    char* result = get_ram_info(true);
    if (!result) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
    free(result);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_ram_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_ram_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_ram_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_disk_info(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_disk_info() takes 0 or 1 arguments");
        return NULL;
    }
    
    char* result = get_disk_info(json_mode);
    if (!result) Py_RETURN_NONE;
    
    if (json_mode) {
        // JSON mode - return as string
        PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
        free(result);
        if (!py_str) {
            PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
            return NULL;
        }
        return py_str;
    } else {
        // Python objects mode - result is a string representation of Python objects
        // We need to evaluate it to get the actual Python objects
        PyObject* main_module = PyImport_ImportModule("__main__");
        if (!main_module) {
            free(result);
            PyErr_SetString(PyExc_ImportError, "Failed to import __main__ module");
            return NULL;
        }
        
        PyObject* globals = PyModule_GetDict(main_module);
        Py_DECREF(main_module);
        
        PyObject* py_objects = PyRun_String(result, Py_eval_input, globals, globals);
        free(result);
        
        if (!py_objects) {
            PyErr_SetString(PyExc_ValueError, "Failed to evaluate Python objects string");
            return NULL;
        }
        
        return py_objects;
    }
}

static PyObject* py_get_disk_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_disk_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_disk_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_network_info(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_network_info() takes 0 or 1 arguments");
        return NULL;
    }
    
    char* result = get_network_info(json_mode);
    if (!result) Py_RETURN_NONE;
    
    if (json_mode) {
        // JSON mode - return as string
        PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
        free(result);
        if (!py_str) {
            PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
            return NULL;
        }
        return py_str;
    } else {
        // Python objects mode - result is a string representation of Python objects
        // We need to evaluate it to get the actual Python objects
        PyObject* main_module = PyImport_ImportModule("__main__");
        if (!main_module) {
            free(result);
            PyErr_SetString(PyExc_ImportError, "Failed to import __main__ module");
            return NULL;
        }
        
        PyObject* globals = PyModule_GetDict(main_module);
        Py_DECREF(main_module);
        
        PyObject* py_objects = PyRun_String(result, Py_eval_input, globals, globals);
        free(result);
        
        if (!py_objects) {
            PyErr_SetString(PyExc_ValueError, "Failed to evaluate Python objects string");
            return NULL;
        }
        
        return py_objects;
    }
}

static PyObject* py_get_network_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_network_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_network_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_cpu_usage(PyObject* self, PyObject* args) {
    char* result = get_cpu_usage(true);
    if (!result) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
    free(result);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_ram_usage(PyObject* self, PyObject* args) {
    char* result = get_ram_usage(true);
    if (!result) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
    free(result);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_system_performance(PyObject* self, PyObject* args) {
    char* result = get_system_performance(true);
    if (!result) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
    free(result);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_monitor_cpu_usage(PyObject* self, PyObject* args) {
    int duration_seconds, interval_ms;
    if (!PyArg_ParseTuple(args, "ii", &duration_seconds, &interval_ms)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments. Expected (duration_seconds, interval_ms)");
        return NULL;
    }

    char* json_str = monitor_cpu_usage_duration(duration_seconds, interval_ms);
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_monitor_ram_usage(PyObject* self, PyObject* args) {
    int duration_seconds, interval_ms;
    if (!PyArg_ParseTuple(args, "ii", &duration_seconds, &interval_ms)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments. Expected (duration_seconds, interval_ms)");
        return NULL;
    }

    char* json_str = monitor_ram_usage_duration(duration_seconds, interval_ms);
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_monitor_system_performance(PyObject* self, PyObject* args) {
    int duration_seconds, interval_ms;
    if (!PyArg_ParseTuple(args, "ii", &duration_seconds, &interval_ms)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments. Expected (duration_seconds, interval_ms)");
        return NULL;
    }

    char* json_str = monitor_system_performance_duration(duration_seconds, interval_ms);
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_partitions_info(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_partitions_info() takes 0 or 1 arguments");
        return NULL;
    }
    
    char* result = get_partitions_info(json_mode);
    if (!result) Py_RETURN_NONE;
    
    if (json_mode) {
        // JSON mode - return as string
        PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
        free(result);
        if (!py_str) {
            PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
            return NULL;
        }
        return py_str;
    } else {
        // Python objects mode - result is a string representation of Python objects
        // We need to evaluate it to get the actual Python objects
        PyObject* main_module = PyImport_ImportModule("__main__");
        if (!main_module) {
            free(result);
            PyErr_SetString(PyExc_ImportError, "Failed to import __main__ module");
            return NULL;
        }
        
        PyObject* globals = PyModule_GetDict(main_module);
        Py_DECREF(main_module);
        
        PyObject* py_objects = PyRun_String(result, Py_eval_input, globals, globals);
        free(result);
        
        if (!py_objects) {
            PyErr_SetString(PyExc_ValueError, "Failed to evaluate Python objects string");
            return NULL;
        }
        
        return py_objects;
    }
}

static PyObject* py_get_partitions_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_partitions_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_partitions_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_smart_info(PyObject* self, PyObject* args) {
    char* result = get_smart_info(true);
    if (!result) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
    free(result);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_smart_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_smart_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_smart_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_baseboard_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_baseboard_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_baseboard_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_gpu_info(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_gpu_info() takes 0 or 1 arguments");
        return NULL;
    }
    
    char* result = get_gpu_info(json_mode);
    if (!result) Py_RETURN_NONE;
    
    if (json_mode) {
        // JSON mode - return as string
        PyObject* py_str = PyUnicode_DecodeUTF8(result, strlen(result), "strict");
        free(result);
        if (!py_str) {
            PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
            return NULL;
        }
        return py_str;
    } else {
        // Python objects mode - result is a string representation of Python objects
        // We need to evaluate it to get the actual Python objects
        PyObject* main_module = PyImport_ImportModule("__main__");
        if (!main_module) {
            free(result);
            PyErr_SetString(PyExc_ImportError, "Failed to import __main__ module");
            return NULL;
        }
        
        PyObject* globals = PyModule_GetDict(main_module);
        Py_DECREF(main_module);
        
        PyObject* py_objects = PyRun_String(result, Py_eval_input, globals, globals);
        free(result);
        
        if (!py_objects) {
            PyErr_SetString(PyExc_ValueError, "Failed to evaluate Python objects string");
            return NULL;
        }
        
        return py_objects;
    }
}

static PyObject* py_get_gpu_info_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_gpu_info_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_gpu_info_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_cpu_usage_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_cpu_usage_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_cpu_usage_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_ram_usage_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_ram_usage_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_ram_usage_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_get_system_performance_objects(PyObject* self, PyObject* args) {
    int json_mode = 1; // Default to JSON mode for backward compatibility
    if (!PyArg_ParseTuple(args, "|i", &json_mode)) {
        PyErr_SetString(PyExc_TypeError, "get_system_performance_objects() takes 0 or 1 arguments");
        return NULL;
    }
    
    PyObject* result = get_system_performance_objects(json_mode);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_monitor_cpu_usage_objects(PyObject* self, PyObject* args) {
    int duration_seconds, interval_ms;
    if (!PyArg_ParseTuple(args, "ii", &duration_seconds, &interval_ms)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments. Expected (duration_seconds, interval_ms)");
        return NULL;
    }

    PyObject* result = monitor_cpu_usage_duration_objects(duration_seconds, interval_ms);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_monitor_ram_usage_objects(PyObject* self, PyObject* args) {
    int duration_seconds, interval_ms;
    if (!PyArg_ParseTuple(args, "ii", &duration_seconds, &interval_ms)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments. Expected (duration_seconds, interval_ms)");
        return NULL;
    }

    PyObject* result = monitor_ram_usage_duration_objects(duration_seconds, interval_ms);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyObject* py_monitor_system_performance_objects(PyObject* self, PyObject* args) {
    int duration_seconds, interval_ms;
    if (!PyArg_ParseTuple(args, "ii", &duration_seconds, &interval_ms)) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments. Expected (duration_seconds, interval_ms)");
        return NULL;
    }

    PyObject* result = monitor_system_performance_duration_objects(duration_seconds, interval_ms);
    if (!result) Py_RETURN_NONE;
    
    return result; // Return the object directly
}

static PyMethodDef HardViewMethods[] = {
    {"get_bios_info", py_get_bios_info, METH_VARARGS, "Get BIOS information as a JSON string or Python objects."},
    {"get_bios_info_objects", py_get_bios_info_objects, METH_VARARGS, "Get BIOS information as Python objects directly."},
    {"get_system_info", py_get_system_info, METH_NOARGS, "Get system information as a JSON string."},
    {"get_system_info_objects", py_get_system_info_objects, METH_VARARGS, "Get system information as Python objects."},
    {"get_baseboard_info", py_get_baseboard_info, METH_NOARGS, "Get baseboard information as a JSON string."},
    {"get_chassis_info", py_get_chassis_info, METH_VARARGS, "Get chassis information as a JSON string or Python objects."},
    {"get_chassis_info_objects", py_get_chassis_info_objects, METH_VARARGS, "Get chassis information as Python objects directly."},
    {"get_cpu_info", py_get_cpu_info, METH_VARARGS, "Get CPU information as a JSON string or Python objects."},
    {"get_cpu_info_objects", py_get_cpu_info_objects, METH_VARARGS, "Get CPU information as Python objects directly."},
    {"get_gpu_info", py_get_gpu_info, METH_VARARGS, "Get GPU information as a JSON string or Python objects."},
    {"get_gpu_info_objects", py_get_gpu_info_objects, METH_VARARGS, "Get GPU information as Python objects directly."},
    {"get_ram_info", py_get_ram_info, METH_NOARGS, "Get RAM information as a JSON string."},
    {"get_ram_info_objects", py_get_ram_info_objects, METH_VARARGS, "Get RAM information as Python objects."},
    {"get_disk_info", py_get_disk_info, METH_VARARGS, "Get disk information as a JSON string or Python objects."},
    {"get_disk_info_objects", py_get_disk_info_objects, METH_VARARGS, "Get disk information as Python objects directly."},
    {"get_network_info", py_get_network_info, METH_VARARGS, "Get network adapter information as a JSON string or Python objects."},
    {"get_network_info_objects", py_get_network_info_objects, METH_VARARGS, "Get network adapter information as Python objects directly."},
    {"get_cpu_usage", py_get_cpu_usage, METH_NOARGS, "Get current CPU usage as a JSON string."},
    {"get_cpu_usage_objects", py_get_cpu_usage_objects, METH_VARARGS, "Get current CPU usage as Python objects."},
    {"get_ram_usage", py_get_ram_usage, METH_NOARGS, "Get current RAM usage as a JSON string."},
    {"get_ram_usage_objects", py_get_ram_usage_objects, METH_VARARGS, "Get current RAM usage as Python objects."},
    {"get_system_performance", py_get_system_performance, METH_NOARGS, "Get system performance (CPU and RAM usage) as a JSON string."},
    {"get_system_performance_objects", py_get_system_performance_objects, METH_VARARGS, "Get system performance (CPU and RAM usage) as Python objects."},

    // === التغيير هنا: توحيد أسماء دوال المراقبة ===
    {"monitor_cpu_usage_duration", py_monitor_cpu_usage, METH_VARARGS, "Monitor CPU usage for a specified duration. Args: (duration_seconds, interval_ms)"},
    {"monitor_cpu_usage_duration_objects", py_monitor_cpu_usage_objects, METH_VARARGS, "Monitor CPU usage for a specified duration as Python objects. Args: (duration_seconds, interval_ms)"},
    {"monitor_ram_usage_duration", py_monitor_ram_usage, METH_VARARGS, "Monitor RAM usage for a specified duration. Args: (duration_seconds, interval_ms)"},
    {"monitor_ram_usage_duration_objects", py_monitor_ram_usage_objects, METH_VARARGS, "Monitor RAM usage for a specified duration as Python objects. Args: (duration_seconds, interval_ms)"},
    {"monitor_system_performance_duration", py_monitor_system_performance, METH_VARARGS, "Monitor system performance for a specified duration. Args: (duration_seconds, interval_ms)"},
    {"monitor_system_performance_duration_objects", py_monitor_system_performance_objects, METH_VARARGS, "Monitor system performance for a specified duration as Python objects. Args: (duration_seconds, interval_ms)"},
    // ===========================================

    {"get_partitions_info", py_get_partitions_info, METH_VARARGS, "Get advanced storage/partitions info as a JSON string."},
    {"get_partitions_info_objects", py_get_partitions_info_objects, METH_VARARGS, "Get advanced storage/partitions info as Python objects."},
    {"get_smart_info", py_get_smart_info, METH_NOARGS, "Get advanced disk info (Win32_DiskDrive) as a JSON string."},
    {"get_smart_info_objects", py_get_smart_info_objects, METH_VARARGS, "Get advanced disk info (Win32_DiskDrive) as Python objects."},
    {"get_baseboard_info_objects", py_get_baseboard_info_objects, METH_VARARGS, "Get baseboard information as Python objects."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef hardviewmodule = {
    PyModuleDef_HEAD_INIT,
    "HardView",
    "A module to get hardware information and system performance.",
    -1,
    HardViewMethods
};

PyMODINIT_FUNC PyInit_HardView(void) {
    return PyModule_Create(&hardviewmodule);
}
#endif // BUILD_PYTHON_MODULE
