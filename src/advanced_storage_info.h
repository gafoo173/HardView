#ifndef ADVANCED_STORAGE_INFO_H
#define ADVANCED_STORAGE_INFO_H

#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

// Function that returns Partition information as JSON or Python objects
char* get_partitions_info(bool Json);

// Function that returns SMART status for each disk as JSON
char* get_smart_status(bool Json);

#ifdef BUILD_PYTHON_MODULE
// Function that returns Python objects directly
PyObject* get_partitions_info_objects(bool Json);

// Helper function to create Python objects
PyObject* create_partition_python_object(const char* disk_model, const char* disk_serial, 
                                       const char* disk_interface, long long disk_size,
                                       const char* disk_media, const char* partition_device_id,
                                       const char* partition_type, long long partition_size,
                                       unsigned long partition_index);
#endif

#endif // ADVANCED_STORAGE_INFO_H
