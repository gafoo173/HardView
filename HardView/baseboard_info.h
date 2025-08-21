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


#ifndef BASEBOARD_INFO_H
#define BASEBOARD_INFO_H

#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

// Function that returns Baseboard information as JSON or Python objects
char* get_baseboard_info(bool Json);

#ifdef BUILD_PYTHON_MODULE
// Function that returns Python objects directly
PyObject* get_baseboard_info_objects(bool Json);

// Helper function to create Python baseboard object
PyObject* create_baseboard_python_object(const char* manufacturer, const char* product, 
                                        const char* serial_number, const char* version);
#endif

#endif // BASEBOARD_INFO_H
