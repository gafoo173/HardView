#ifndef RAM_INFO_H
#define RAM_INFO_H

#include <Python.h>

// JSON output functions
char* get_ram_info(bool Json);

// Python object output functions
PyObject* get_ram_info_objects(bool Json);

#endif // RAM_INFO_H 