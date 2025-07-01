import json
import sys
import time

try:
    from HardView import (
        get_bios_info,
        get_system_info,
        get_baseboard_info,
        get_chassis_info,
        get_cpu_info,
        get_ram_info,
        get_disk_info,
        get_network_info,
        get_cpu_usage,
        get_ram_usage,
        get_system_performance,
        monitor_cpu_usage,
        monitor_ram_usage,
        monitor_system_performance,
        get_partitions_info,
        get_smart_info
    )
except ImportError:
    print("Error: HardView C extension not found or failed to load.", file=sys.stderr)
    print("Make sure the library is properly built and installed.", file=sys.stderr)
    sys.exit(1)

def _parse_json_or_error(json_str):
    if not json_str:
        return {"error": "No data returned from C extension."}
    try:
        data = json.loads(json_str)
        if isinstance(data, dict) and "error" in data:
            raise RuntimeError(f"Hardware info error: {data['error']}")
        return data
    except json.JSONDecodeError as e:
        raise ValueError(f"Failed to parse JSON from C extension: {e}\nRaw: {json_str}")
    except RuntimeError:
        raise

def get_bios(): return _parse_json_or_error(get_bios_info())
def get_system(): return _parse_json_or_error(get_system_info())
def get_baseboard(): return _parse_json_or_error(get_baseboard_info())
def get_chassis(): return _parse_json_or_error(get_chassis_info())
def get_cpu(): return _parse_json_or_error(get_cpu_info())
def get_ram(): return _parse_json_or_error(get_ram_info())
def get_disk_drives(): return _parse_json_or_error(get_disk_info())
def get_network_adapters(): return _parse_json_or_error(get_network_info())

def get_cpu_usage_current(): return _parse_json_or_error(get_cpu_usage())
def get_ram_usage_current(): return _parse_json_or_error(get_ram_usage())
def get_system_performance_current(): return _parse_json_or_error(get_system_performance())

def monitor_cpu_usage_duration(duration_seconds, interval_ms): 
    return _parse_json_or_error(monitor_cpu_usage(duration_seconds, interval_ms))

def monitor_ram_usage_duration(duration_seconds, interval_ms): 
    return _parse_json_or_error(monitor_ram_usage(duration_seconds, interval_ms))

def monitor_system_performance_duration(duration_seconds, interval_ms): 
    return _parse_json_or_error(monitor_system_performance(duration_seconds, interval_ms))

def get_partitions():
    json_str = get_partitions_info()
    return _parse_json_or_error(json_str)

def get_smart_info_json():
    json_str = get_smart_info()
    return _parse_json_or_error(json_str)

if __name__ == "__main__":
    print("=== HardView Library Test ===\n")
    print("1. Testing Hardware Information Functions...")
    print("=" * 50)

    benchmark_times = {}
    components = {
        "BIOS": get_bios,
        "System": get_system,
        "Baseboard": get_baseboard,
        "Chassis": get_chassis,
        "CPU": get_cpu,
        "RAM": get_ram,
        "Disk Drives": get_disk_drives,
        "Network Adapters": get_network_adapters,
    }

    collected_data = {}

    for name, func in components.items():
        try:
            start = time.perf_counter()
            data = func()
            end = time.perf_counter()
            benchmark_times[name] = end - start
            collected_data[name] = data
            print(f"\n{name} Info:")
            print(json.dumps(data, indent=2, ensure_ascii=False))
        except Exception as e:
            benchmark_times[name] = None
            print(f"\nError getting {name} info: {e}")

    print("\nTesting Advanced Storage Info (Partitions)...")
    print("=" * 50)
    try:
        start = time.perf_counter()
        partitions_data = get_partitions()
        end = time.perf_counter()
        print("\nPartitions Info:")
        print(json.dumps(partitions_data, indent=2, ensure_ascii=False))
        print(f"Execution time: {end - start:.6f} sec")
    except Exception as e:
        print(f"\nError getting partitions info: {e}")

    print("\nTesting Advanced Disk Info (SMART/Win32_DiskDrive)...")
    print("=" * 50)
    try:
        start = time.perf_counter()
        smart_data = get_smart_info_json()
        end = time.perf_counter()
        print("\nSMART/Disk Info:")
        print(json.dumps(smart_data, indent=2, ensure_ascii=False))
        print(f"Execution time: {end - start:.6f} sec")
    except Exception as e:
        print(f"\nError getting SMART/disk info: {e}")

    print("\n\nHardware Info Benchmark Results (Seconds):")
    print("-" * 40)
    for name in components.keys():
        t = benchmark_times.get(name)
        if t is not None:
            print(f"{name:15}: {t:.6f} sec")
        else:
            print(f"{name:15}: Failed")

    print("\n\n2. Testing Current Performance Functions...")
    print("=" * 50)

    performance_components = {
        "CPU Usage": get_cpu_usage_current,
        "RAM Usage": get_ram_usage_current,
        "System Performance": get_system_performance_current,
    }

    for name, func in performance_components.items():
        try:
            start = time.perf_counter()
            data = func()
            end = time.perf_counter()
            print(f"\n{name}:")
            print(json.dumps(data, indent=2, ensure_ascii=False))
            print(f"Execution time: {end - start:.6f} sec")
        except Exception as e:
            print(f"\nError getting {name}: {e}")

    print("\n\n3. Testing Performance Monitoring Functions...")
    print("=" * 50)

    print("\nMonitoring CPU usage for 5 seconds (1 second intervals)...")
    try:
        start = time.perf_counter()
        cpu_monitoring = monitor_cpu_usage_duration(5, 1000)
        end = time.perf_counter()

        print(f"CPU Monitoring Results (collected in {end - start:.2f} seconds):")
        print(f"Duration: {cpu_monitoring.get('cpu_monitoring', {}).get('duration_seconds', 'N/A')} seconds")
        print(f"Interval: {cpu_monitoring.get('cpu_monitoring', {}).get('interval_ms', 'N/A')} ms")
        print(f"Readings collected: {len(cpu_monitoring.get('cpu_monitoring', {}).get('readings', []))}")

        readings = cpu_monitoring.get('cpu_monitoring', {}).get('readings', [])
        if readings:
            print(f"First reading: {readings[0]}")
            print(f"Last reading: {readings[-1]}")
    except Exception as e:
        print(f"Error in CPU monitoring: {e}")

    print("\nMonitoring RAM usage for 3 seconds (500ms intervals)...")
    try:
        start = time.perf_counter()
        ram_monitoring = monitor_ram_usage_duration(3, 500)
        end = time.perf_counter()

        print(f"RAM Monitoring Results (collected in {end - start:.2f} seconds):")
        print(f"Duration: {ram_monitoring.get('ram_monitoring', {}).get('duration_seconds', 'N/A')} seconds")
        print(f"Interval: {ram_monitoring.get('ram_monitoring', {}).get('interval_ms', 'N/A')} ms")
        print(f"Readings collected: {len(ram_monitoring.get('ram_monitoring', {}).get('readings', []))}")

        readings = ram_monitoring.get('ram_monitoring', {}).get('readings', [])
        if readings:
            print(f"First reading: {readings[0]}")
            print(f"Last reading: {readings[-1]}")
    except Exception as e:
        print(f"Error in RAM monitoring: {e}")

    print("\nMonitoring System performance for 2 seconds (1 second intervals)...")
    try:
        start = time.perf_counter()
        system_monitoring = monitor_system_performance_duration(2, 1000)
        end = time.perf_counter()

        print(f"System Performance Monitoring Results (collected in {end - start:.2f} seconds):")
        print(f"Duration: {system_monitoring.get('system_performance_monitoring', {}).get('duration_seconds', 'N/A')} seconds")
        print(f"Interval: {system_monitoring.get('system_performance_monitoring', {}).get('interval_ms', 'N/A')} ms")
        print(f"Readings collected: {len(system_monitoring.get('system_performance_monitoring', {}).get('readings', []))}")

        readings = system_monitoring.get('system_performance_monitoring', {}).get('readings', [])
        if readings:
            print(f"First reading: {readings[0]}")
            print(f"Last reading: {readings[-1]}")
    except Exception as e:
        print(f"Error in System performance monitoring: {e}")

    print("\n\n=== Test Completed Successfully! ===")
