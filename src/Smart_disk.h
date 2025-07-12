#ifndef SMART_DISK_H
#define SMART_DISK_H
#include <stdbool.h>

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif


char* get_smart_info(bool Json);

// Python object output functions
PyObject* get_smart_info_objects(bool Json);



#ifdef __cplusplus
}
#endif

#endif // SMART_DISK_H 
