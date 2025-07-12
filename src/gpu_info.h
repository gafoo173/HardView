#ifndef GPU_INFO_H
#define GPU_INFO_H

#include <stdbool.h>

#ifdef BUILD_PYTHON_MODULE
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#endif

// هيكل يحمل معلومات كرت الشاشة
typedef struct {
    char* name;
    char* driver_version;
    char* memory_size;
    char* vendor;
} GPUInfo;

// Function that returns GPU information as JSON or Python objects
char* get_gpu_info(bool Json);

// Helper function to create GPU info structure (internal use)
GPUInfo* _create_gpu_info();

// دالة لتحرير الذاكرة المخصصة للهيكل
void free_gpu_info(GPUInfo* info);

#ifdef BUILD_PYTHON_MODULE
// Function that returns Python objects directly
PyObject* get_gpu_info_objects(bool Json);

// Helper function to create Python GPU object
PyObject* create_gpu_python_object(const char* name, const char* manufacturer, 
                                  const char* driver_version, long long memory_size,
                                  const char* video_processor, const char* video_mode_description);
#endif

#endif // GPU_INFO_H 