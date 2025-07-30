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


#ifndef LINUX_HELPERS_H
#define LINUX_HELPERS_H
#include <stdbool.h>

char* _read_dmi_attribute_linux(const char* attribute_name);
char* _read_proc_sys_value(const char* path, const char* key);

#endif // LINUX_HELPERS_H 
