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
