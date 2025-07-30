/*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License.
 See the LICENSE file in the project root for more details.
================================================================================
*/


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
