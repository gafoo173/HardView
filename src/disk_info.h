#ifndef DISK_INFO_H
#define DISK_INFO_H

#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

// Function that returns disk information as JSON or Python objects
char* get_disk_info(bool Json);

#ifdef BUILD_PYTHON_MODULE
// Function that returns Python objects directly
PyObject* get_disk_info_objects(bool Json);

// Helper function to create Python disk object
PyObject* create_disk_python_object(const char* model, const char* manufacturer, 
                                   const char* interface_type, long long size,
                                   const char* serial_number, const char* media_type);
#endif

#endif // DISK_INFO_H 