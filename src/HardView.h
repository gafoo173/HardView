#ifndef HARDVIEW_H
#define HARDVIEW_H
#include <stdbool.h>
char* get_bios_info(bool Json);
char* get_system_info(bool Json);
char* get_baseboard_info(bool Json);
char* get_chassis_info(bool Json);
char* get_cpu_info(bool Json);
char* get_ram_info(bool Json);
char* get_disk_info(bool Json);
char* get_network_info(bool Json);
char* get_cpu_usage(bool Json);
char* get_ram_usage(bool Json);
char* get_system_performance(bool Json);
char* monitor_cpu_usage_duration(int duration_seconds, int interval_ms);
char* monitor_ram_usage_duration(int duration_seconds, int interval_ms);
char* monitor_system_performance_duration(int duration_seconds, int interval_ms);
char* get_partitions_info(bool Json);
char* get_smart_info(bool Json);

#endif // HARDVIEW_H 
