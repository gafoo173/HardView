#ifndef CHASSIS_INFO_H
#define CHASSIS_INFO_H

#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

// Function that returns chassis information as JSON or Python objects
char* get_chassis_info(bool Json);

#ifdef BUILD_PYTHON_MODULE
// Function that returns Python objects directly
PyObject* get_chassis_info_objects(bool Json);

// Helper function to create Python chassis object
PyObject* create_chassis_python_object(const char* manufacturer, const char* model, 
                                      const char* serial_number, const char* chassis_type);
#endif

#endif // CHASSIS_INFO_H 