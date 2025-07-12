#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <stdbool.h>

#include <Python.h>

// JSON output functions
char* get_system_info(bool Json);

// Python object output functions
PyObject* get_system_info_objects(bool Json);

#endif // SYSTEM_INFO_H 
