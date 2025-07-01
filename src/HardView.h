#ifndef HARDVIEW_H
#define HARDVIEW_H

char* get_bios_info_json();
char* get_system_info_json();
char* get_baseboard_info_json();
char* get_chassis_info_json();
char* get_cpu_info_json();
char* get_ram_info_json();
char* get_disk_info_json();
char* get_network_info_json();
char* get_cpu_usage_json();
char* get_ram_usage_json();
char* get_system_performance_json();
char* monitor_cpu_usage_duration(int duration_seconds, int interval_ms);
char* monitor_ram_usage_duration(int duration_seconds, int interval_ms);
char* monitor_system_performance_duration(int duration_seconds, int interval_ms);
char* get_partitions_info_json();
char* get_smart_info_json();

#endif // HARDVIEW_H 