#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

char* get_cpu_usage_json();
char* get_ram_usage_json();
char* get_system_performance_json();
char* monitor_cpu_usage_duration(int duration_seconds, int interval_ms);
char* monitor_ram_usage_duration(int duration_seconds, int interval_ms);
char* monitor_system_performance_duration(int duration_seconds, int interval_ms);

#endif // PERFORMANCE_MONITOR_H 