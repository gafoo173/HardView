#include "linux_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INFO_LEN 1024
#define DMI_PATH_PREFIX "/sys/class/dmi/id/"

char* _read_dmi_attribute_linux(const char* attribute_name) {
    char path[MAX_INFO_LEN];
    snprintf(path, sizeof(path), "%s%s", DMI_PATH_PREFIX, attribute_name);
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return strdup("N/A");
    }
    char* buffer = (char*)malloc(MAX_INFO_LEN);
    if (buffer == NULL) {
        fclose(fp);
        return strdup("MEMORY_ERROR");
    }
    if (fgets(buffer, MAX_INFO_LEN, fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    } else {
        strcpy(buffer, "N/A");
    }
    fclose(fp);
    return buffer;
}

char* _read_proc_sys_value(const char* path, const char* key) {
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        return strdup("N/A");
    }
    char line[MAX_INFO_LEN];
    char* value = strdup("N/A");
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strlen(key) == 0 || strstr(line, key) == line) {
            char* start = line;
            if (strlen(key) > 0) {
                start = strchr(line, ':');
                if (!start) continue;
                start += 1;
            }
            while (*start == ' ' || *start == '\t') start++;
            size_t end_ptr = strcspn(start, "\n");
            if (end_ptr > 0) {
                char temp[MAX_INFO_LEN];
                strncpy(temp, start, end_ptr);
                temp[end_ptr] = '\0';
                free(value);
                value = strdup(temp);
                break;
            }
        }
    }
    fclose(fp);
    return value;
}
