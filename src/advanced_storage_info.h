   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
#ifndef ADVANCED_STORAGE_INFO_H
#define ADVANCED_STORAGE_INFO_H

// Function that returns Partition information as JSON
char* get_partitions_info_json();

// Function that returns SMART status for each disk as JSON
char* get_smart_status_json();

#endif // ADVANCED_STORAGE_INFO_H
