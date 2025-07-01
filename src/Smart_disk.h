#ifndef SMART_DISK_H
#define SMART_DISK_H

#ifdef __cplusplus
extern "C" {
#endif

// دالة رئيسية لجلب معلومات SMART كـ JSON
char* get_smart_info_json();

// يمكن إضافة دوال مساعدة لاحقًا

#ifdef __cplusplus
}
#endif

#endif // SMART_DISK_H 