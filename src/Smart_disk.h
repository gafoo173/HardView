#ifndef SMART_DISK_H
#define SMART_DISK_H

#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

// دالة رئيسية لجلب معلومات SMART كـ JSON
char* get_smart_info(bool Json);

// Python object output functions
PyObject* get_smart_info_objects(bool Json);

// يمكن إضافة دوال مساعدة لاحقًا

#ifdef __cplusplus
}
#endif

#endif // SMART_DISK_H 