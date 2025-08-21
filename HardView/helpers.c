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


#include "helpers.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* _create_json_string(const char* format, ...) {
    va_list args, args_copy;
    char* json_str = NULL;
    int len = 0;

    va_start(args, format);
    va_copy(args_copy, args);
    len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (len < 0) {
        return strdup("{\"error\": \"Failed to estimate JSON string size during formatting.\"}");
    }

    json_str = (char*)malloc(len + 1);
    if (json_str == NULL) {
        return strdup("{\"error\": \"Memory allocation failed for JSON string.\"}");
    }

    va_start(args, format);
    vsnprintf(json_str, len + 1, format, args);
    va_end(args);

    return json_str;
}
