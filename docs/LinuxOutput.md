# Linux System Information Output Examples

This document provides examples of system information output on Linux systems, demonstrating various hardware and performance monitoring functions.

## BIOS Information

### get_bios_info()
```json
{
    "manufacturer": "American Megatrends Inc.",
    "version": "090008 ",
    "release_date": "12/07/2018"
}
```

**Python Object:**
```python
{
    'manufacturer': 'American Megatrends Inc.', 
    'version': '090008 ', 
    'release_date': '12/07/2018'
}
```

## System Information

### get_system_info()
```json
{
    "manufacturer": "Microsoft Corporation",
    "product_name": "Virtual Machine",
    "uuid": "b6a4c6cb-c0f2-1941-ad6b-c8d6fd2e34c7",
    "serial_number": "N/A"
}
```

**Python Object:**
```python
{
    'manufacturer': 'Microsoft Corporation', 
    'product_name': 'Virtual Machine', 
    'uuid': 'b6a4c6cb-c0f2-1941-ad6b-c8d6fd2e34c7', 
    'serial_number': 'N/A'
}
```

## Baseboard Information

### get_baseboard_info()
```json
{
    "manufacturer": "Microsoft Corporation",
    "product": "Virtual Machine",
    "serial_number": "N/A",
    "version": "7.0"
}
```

**Python Object:**
```python
{
    'manufacturer': 'Microsoft Corporation', 
    'product': 'Virtual Machine', 
    'serial_number': 'N/A', 
    'version': '7.0'
}
```

## Chassis Information

### get_chassis_info()
```json
{
    "manufacturer": "Microsoft Corporation",
    "model": "N/A",
    "serial_number": "N/A",
    "chassis_type": "3"
}
```

**Python Object:**
```python
{
    'manufacturer': 'Microsoft Corporation', 
    'model': 'N/A', 
    'serial_number': 'N/A', 
    'chassis_type': '3'
}
```

## CPU Information

### get_cpu_info()
**Note:** JSON decoding error occurred for this function.

**Python Object:**
```python
{
    'name': 'N/A', 
    'manufacturer': 'N/A', 
    'architecture': 'N/A', 
    'cores': 0, 
    'threads': 0, 
    'max_clock_speed': 0.0, 
    'socket_designation': 'N/A'
}
```

## RAM Information

### get_ram_info()
```json
{
    "total_mem_bytes": 8330170368,
    "free_mem_bytes": 6166085632,
    "buffers_bytes": 51146752,
    "cached_bytes": 1555075072
}
```

**Python Object:**
```python
{
    'total_mem_bytes': 8330170368, 
    'free_mem_bytes': 6166085632, 
    'buffers_bytes': 51146752, 
    'cached_bytes': 1555075072
}
```

## Network Information

### get_network_info()
```json
{
    "network_adapters": [
        {
            "name": "lo",
            "mac_address": "00:00:00:00:00:00",
            "ipv4_addresses": [
                "127.0.0.1"
            ],
            "ipv6_addresses": [
                "::1"
            ]
        },
        {
            "name": "eth0",
            "mac_address": "00:22:48:91:79:99",
            "ipv4_addresses": [
                "10.1.0.131"
            ],
            "ipv6_addresses": [
                "fe80::222:48ff:fe91:7999"
            ]
        },
        {
            "name": "docker0",
            "mac_address": "2a:b8:e2:d9:c3:8d",
            "ipv4_addresses": [
                "172.17.0.1"
            ],
            "ipv6_addresses": []
        }
    ]
}
```

**Python Object:**
```python
[
    {
        'description': 'lo', 
        'mac_address': '00:00:00:00:00:00', 
        'ip_addresses': ['127.0.0.1"]', '["::1'], 
        'dns_host_name': 'N/A'
    }, 
    {
        'description': 'eth0', 
        'mac_address': '00:22:48:91:79:99', 
        'ip_addresses': ['10.1.0.131"]', '["fe80::222:48ff:fe91:7999'], 
        'dns_host_name': 'N/A'
    }, 
    {
        'description': 'docker0', 
        'mac_address': '2a:b8:e2:d9:c3:8d', 
        'ip_addresses': ['172.17.0.1"]', '['], 
        'dns_host_name': 'N/A'
    }
]
```

## CPU Usage

### get_cpu_usage()
```json
{
    "cpu_usage": {
        "load_1min": 0.17,
        "load_5min": 0.07,
        "load_15min": 0.02
    }
}
```

**Python Object:**
```python
{
    'cpu_usage': {
        'load_1min': 0.17, 
        'load_5min': 0.07, 
        'load_15min': 0.02
    }
}
```

## RAM Usage

### get_ram_usage()
```json
{
    "ram_usage": {
        "total_memory_kb": 8134932,
        "free_memory_kb": 6021440,
        "available_memory_kb": 7343396,
        "used_memory_kb": 791536,
        "usage_percent": 9.73
    }
}
```

**Python Object:**
```python
{
    'ram_usage': {
        'total_memory_kb': 8134932, 
        'free_memory_kb': 6021440, 
        'available_memory_kb': 7343396, 
        'used_memory_kb': 791536, 
        'usage_percent': 9.730087479526565
    }
}
```

## System Performance

### get_system_performance()
```json
{
    "system_performance": {
        "cpu": {
            "cpu_usage": {
                "load_1min": 0.17,
                "load_5min": 0.07,
                "load_15min": 0.02
            }
        },
        "ram": {
            "ram_usage": {
                "total_memory_kb": 8134932,
                "free_memory_kb": 6021440,
                "available_memory_kb": 7343396,
                "used_memory_kb": 791536,
                "usage_percent": 9.73
            }
        }
    }
}
```

**Python Object:**
```python
{
    'system_performance': {
        'cpu': {
            'load_1min': 0.17, 
            'load_5min': 0.07, 
            'load_15min': 0.02
        }, 
        'ram': {
            'total_memory_kb': 8134932, 
            'free_memory_kb': 6021440, 
            'available_memory_kb': 7343396, 
            'used_memory_kb': 791536, 
            'usage_percent': 9.730087479526565
        }
    }
}
```

## Disk Partitions Information

### get_partitions_info()
```json
{
    "blockdevices": [
        {
            "name": "sda",
            "size": "75G",
            "model": "Virtual Disk",
            "serial": "60022480a9fe70588c526b7a470549f9",
            "type": "disk",
            "vendor": "Msft    ",
            "children": [
                {
                    "name": "sda1",
                    "size": "74G",
                    "model": null,
                    "serial": null,
                    "type": "part",
                    "vendor": null
                },
                {
                    "name": "sda14",
                    "size": "4M",
                    "model": null,
                    "serial": null,
                    "type": "part",
                    "vendor": null
                },
                {
                    "name": "sda15",
                    "size": "106M",
                    "model": null,
                    "serial": null,
                    "type": "part",
                    "vendor": null
                },
                {
                    "name": "sda16",
                    "size": "913M",
                    "model": null,
                    "serial": null,
                    "type": "part",
                    "vendor": null
                }
            ]
        }
    ]
}
```

## GPU Information

### get_gpu_info()
**Note:** JSON decoding error occurred for this function.

**Python Object:**
```python
[
    {
        'name': 'N/A', 
        'manufacturer': 'N/A', 
        'driver_version': 'N/A', 
        'memory_size': 0, 
        'video_processor': 'N/A', 
        'video_mode_description': 'N/A'
    }
]
```

## Monitoring Functions

### CPU Usage Monitoring (5 seconds)

#### monitor_cpu_usage_duration()
```json
{
    "cpu_monitoring": {
        "duration_seconds": 5,
        "interval_ms": 1000,
        "readings": [
            {
                "cpu_usage": {
                    "load_1min": 0.17,
                    "load_5min": 0.07,
                    "load_15min": 0.02
                }
            },
            {
                "cpu_usage": {
                    "load_1min": 0.17,
                    "load_5min": 0.07,
                    "load_15min": 0.02
                }
            },
            {
                "cpu_usage": {
                    "load_1min": 0.17,
                    "load_5min": 0.07,
                    "load_15min": 0.02
                }
            },
            {
                "cpu_usage": {
                    "load_1min": 0.17,
                    "load_5min": 0.07,
                    "load_15min": 0.02
                }
            },
            {
                "cpu_usage": {
                    "load_1min": 0.15,
                    "load_5min": 0.07,
                    "load_15min": 0.02
                }
            }
        ]
    }
}
```

### RAM Usage Monitoring (5 seconds)

#### monitor_ram_usage_duration()
```json
{
    "ram_monitoring": {
        "duration_seconds": 5,
        "interval_ms": 1000,
        "readings": [
            {
                "ram_usage": {
                    "total_memory_kb": 8134932,
                    "free_memory_kb": 6050036,
                    "available_memory_kb": 7372136,
                    "used_memory_kb": 762796,
                    "usage_percent": 9.38
                }
            },
            {
                "ram_usage": {
                    "total_memory_kb": 8134932,
                    "free_memory_kb": 6050036,
                    "available_memory_kb": 7372136,
                    "used_memory_kb": 762796,
                    "usage_percent": 9.38
                }
            },
            {
                "ram_usage": {
                    "total_memory_kb": 8134932,
                    "free_memory_kb": 6050036,
                    "available_memory_kb": 7372136,
                    "used_memory_kb": 762796,
                    "usage_percent": 9.38
                }
            },
            {
                "ram_usage": {
                    "total_memory_kb": 8134932,
                    "free_memory_kb": 6050036,
                    "available_memory_kb": 7372136,
                    "used_memory_kb": 762796,
                    "usage_percent": 9.38
                }
            },
            {
                "ram_usage": {
                    "total_memory_kb": 8134932,
                    "free_memory_kb": 6050036,
                    "available_memory_kb": 7372136,
                    "used_memory_kb": 762796,
                    "usage_percent": 9.38
                }
            }
        ]
    }
}
```

### System Performance Monitoring (5 seconds)

#### monitor_system_performance_duration()
```json
{
    "system_performance_monitoring": {
        "duration_seconds": 5,
        "interval_ms": 1000,
        "readings": [
            {
                "system_performance": {
                    "cpu": {
                        "cpu_usage": {
                            "load_1min": 0.12,
                            "load_5min": 0.07,
                            "load_15min": 0.02
                        }
                    },
                    "ram": {
                        "ram_usage": {
                            "total_memory_kb": 8134932,
                            "free_memory_kb": 6050036,
                            "available_memory_kb": 7372140,
                            "used_memory_kb": 762792,
                            "usage_percent": 9.38
                        }
                    }
                }
            },
            {
                "system_performance": {
                    "cpu": {
                        "cpu_usage": {
                            "load_1min": 0.12,
                            "load_5min": 0.07,
                            "load_15min": 0.02
                        }
                    },
                    "ram": {
                        "ram_usage": {
                            "total_memory_kb": 8134932,
                            "free_memory_kb": 6049012,
                            "available_memory_kb": 7371124,
                            "used_memory_kb": 763808,
                            "usage_percent": 9.39
                        }
                    }
                }
            },
            {
                "system_performance": {
                    "cpu": {
                        "cpu_usage": {
                            "load_1min": 0.12,
                            "load_5min": 0.07,
                            "load_15min": 0.02
                        }
                    },
                    "ram": {
                        "ram_usage": {
                            "total_memory_kb": 8134932,
                            "free_memory_kb": 6048956,
                            "available_memory_kb": 7371076,
                            "used_memory_kb": 763856,
                            "usage_percent": 9.39
                        }
                    }
                }
            },
            {
                "system_performance": {
                    "cpu": {
                        "cpu_usage": {
                            "load_1min": 0.12,
                            "load_5min": 0.07,
                            "load_15min": 0.02
                        }
                    },
                    "ram": {
                        "ram_usage": {
                            "total_memory_kb": 8134932,
                            "free_memory_kb": 6048956,
                            "available_memory_kb": 7371076,
                            "used_memory_kb": 763856,
                            "usage_percent": 9.39
                        }
                    }
                }
            },
            {
                "system_performance": {
                    "cpu": {
                        "cpu_usage": {
                            "load_1min": 0.11,
                            "load_5min": 0.07,
                            "load_15min": 0.02
                        }
                    },
                    "ram": {
                        "ram_usage": {
                            "total_memory_kb": 8134932,
                            "free_memory_kb": 6048956,
                            "available_memory_kb": 7371076,
                            "used_memory_kb": 763856,
                            "usage_percent": 9.39
                        }
                    }
                }
            }
        ]
    }
}
```

## Error Cases

### SMART Information
```json
{
    "error": "Disk info not implemented for this OS yet"
}
```

### Disk Information
**Note:** JSON decoding error occurred, returns empty Python list `[]`.

## Notes

- This output is from a Microsoft Virtual Machine environment running Linux
- **JSON Format Issues**: Some functions like `get_cpu_info()`, `get_disk_info()`, and `get_gpu_info()` show JSON decoding errors due to formatting issues with `N/A` values in the JSON output
- **Recommendation**: It's preferable to use **Python Objects** instead of JSON format in newer versions of the HardView library to avoid these parsing errors
---
*Generated from HardView library JSON function test results on Linux*
