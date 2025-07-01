#include "HardView.h"
#include "bios_info.h"
#include "system_info.h"
#include "baseboard_info.h"
#include "chassis_info.h"
#include "cpu_info.h"
#include "ram_info.h"
#include "disk_info.h"
#include "network_info.h"
#include "performance_monitor.h"
#include "advanced_storage_info.h"

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

static PyObject* py_get_cpu_usage_json(PyObject* self, PyObject* args) {
    char* json_str = get_cpu_usage_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_ram_usage_json(PyObject* self, PyObject* args) {
    char* json_str = get_ram_usage_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_system_performance_json(PyObject* self, PyObject* args) {
    char* json_str = get_system_performance_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_monitor_cpu_usage_duration(PyObject* self, PyObject* args) {
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

static PyObject* py_monitor_ram_usage_duration(PyObject* self, PyObject* args) {
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

static PyObject* py_monitor_system_performance_duration(PyObject* self, PyObject* args) {
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

static PyObject* py_get_partitions_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_partitions_info_json();
    if (!json_str)
        json_str = strdup("{\"error\": \"get_partitions_info_json returned NULL (unexpected).\"}");
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyObject* py_get_smart_info_json(PyObject* self, PyObject* args) {
    char* json_str = get_smart_info_json();
    if (!json_str) Py_RETURN_NONE;
    PyObject* py_str = PyUnicode_DecodeUTF8(json_str, strlen(json_str), "strict");
    free(json_str);
    if (!py_str) {
        PyErr_SetString(PyExc_UnicodeDecodeError, "Failed to decode C string to Python Unicode (UTF-8).");
        return NULL;
    }
    return py_str;
}

static PyMethodDef HardViewMethods[] = {
    {"get_bios_info", py_get_bios_info_json, METH_NOARGS, "Get BIOS information as a JSON string."},
    {"get_system_info", py_get_system_info_json, METH_NOARGS, "Get system information as a JSON string."},
    {"get_baseboard_info", py_get_baseboard_info_json, METH_NOARGS, "Get baseboard information as a JSON string."},
    {"get_chassis_info", py_get_chassis_info_json, METH_NOARGS, "Get chassis information as a JSON string."},
    {"get_cpu_info", py_get_cpu_info_json, METH_NOARGS, "Get CPU information as a JSON string."},
    {"get_ram_info", py_get_ram_info_json, METH_NOARGS, "Get RAM information as a JSON string."},
    {"get_disk_info", py_get_disk_info_json, METH_NOARGS, "Get disk information as a JSON string."},
    {"get_network_info", py_get_network_info_json, METH_NOARGS, "Get network adapter information as a JSON string."},
    {"get_cpu_usage", py_get_cpu_usage_json, METH_NOARGS, "Get current CPU usage as a JSON string."},
    {"get_ram_usage", py_get_ram_usage_json, METH_NOARGS, "Get current RAM usage as a JSON string."},
    {"get_system_performance", py_get_system_performance_json, METH_NOARGS, "Get system performance (CPU and RAM usage) as a JSON string."},
    {"monitor_cpu_usage", py_monitor_cpu_usage_duration, METH_VARARGS, "Monitor CPU usage for a specified duration. Args: (duration_seconds, interval_ms)"},
    {"monitor_ram_usage", py_monitor_ram_usage_duration, METH_VARARGS, "Monitor RAM usage for a specified duration. Args: (duration_seconds, interval_ms)"},
    {"monitor_system_performance", py_monitor_system_performance_duration, METH_VARARGS, "Monitor system performance for a specified duration. Args: (duration_seconds, interval_ms)"},
    {"get_partitions_info", py_get_partitions_info_json, METH_NOARGS, "Get advanced storage/partitions info as a JSON string."},
    {"get_smart_info", py_get_smart_info_json, METH_NOARGS, "Get advanced disk info (Win32_DiskDrive) as a JSON string."},
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
