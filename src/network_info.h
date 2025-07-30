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


#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H
#include <stdbool.h>
#ifdef BUILD_PYTHON_MODULE
#include <Python.h>

// Helper function to create Python network adapter object
PyObject* create_network_python_object(const char* description, const char* mac_address, 
                                      const char* ip_addresses, const char* dns_host_name);

// Function that returns Python objects directly
PyObject* get_network_info_objects(bool Json);
#endif

char* get_network_info(bool Json);

#endif // NETWORK_INFO_H 
