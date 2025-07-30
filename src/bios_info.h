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


#ifndef BIOS_INFO_H
#define BIOS_INFO_H

#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

// Function that returns BIOS information as JSON or Python objects
char* get_bios_info(bool Json);

#ifdef BUILD_PYTHON_MODULE
// Function that returns Python objects directly
PyObject* get_bios_info_objects(bool Json);

// Helper function to create Python BIOS object
PyObject* create_bios_python_object(const char* manufacturer, const char* version, 
                                   const char* release_date);
#endif

#endif // BIOS_INFO_H
