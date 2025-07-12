#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <Python.h>

// JSON output functions
char* get_cpu_usage(bool Json);
char* get_ram_usage(bool Json);
char* get_system_performance(bool Json);
char* monitor_cpu_usage_duration(int duration_seconds, int interval_ms);
char* monitor_ram_usage_duration(int duration_seconds, int interval_ms);
char* monitor_system_performance_duration(int duration_seconds, int interval_ms);

// Python object output functions
PyObject* get_cpu_usage_objects(bool Json);
PyObject* get_ram_usage_objects(bool Json);
PyObject* get_system_performance_objects(bool Json);
PyObject* monitor_cpu_usage_duration_objects(int duration_seconds, int interval_ms);
PyObject* monitor_ram_usage_duration_objects(int duration_seconds, int interval_ms);
PyObject* monitor_system_performance_duration_objects(int duration_seconds, int interval_ms);

#endif // PERFORMANCE_MONITOR_H 