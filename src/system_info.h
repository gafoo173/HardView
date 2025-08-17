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


#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#include <Python.h>
#endif

// JSON output functions
char* get_system_info(bool Json);

#endif // SYSTEM_INFO_H 
