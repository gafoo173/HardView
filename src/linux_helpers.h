#ifndef LINUX_HELPERS_H
#define LINUX_HELPERS_H
#include <stdbool.h>

char* _read_dmi_attribute_linux(const char* attribute_name);
char* _read_proc_sys_value(const char* path, const char* key);

#endif // LINUX_HELPERS_H 
