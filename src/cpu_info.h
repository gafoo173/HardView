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


#ifndef CPU_INFO_H
#define CPU_INFO_H

#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

// Function that returns CPU information as JSON or Python objects
char* get_cpu_info(bool Json);

#ifdef BUILD_PYTHON_MODULE
// Function that returns Python objects directly
PyObject* get_cpu_info_objects(bool Json);

// Helper function to create Python CPU object
PyObject* create_cpu_python_object(const char* name, const char* manufacturer, 
                                  const char* architecture, int cores, int threads,
                                  double max_clock_speed, const char* socket_designation);
#endif

#endif // CPU_INFO_H
