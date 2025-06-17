import json
import sys
import time

try:
    # Import the compiled C extension
    from HardView import (
        get_bios_info,
        get_system_info,
        get_baseboard_info,
        get_chassis_info,
        get_cpu_info,
        get_ram_info,
        get_disk_info,
        get_network_info
    )
except ImportError:
    print("Error: HardView C extension not found or failed to load.", file=sys.stderr)
    print("Make sure the library is properly built and installed.", file=sys.stderr)
    sys.exit(1)

def _parse_json_or_error(json_str):
    """Parse JSON string or raise an error if it contains 'error'."""
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

if __name__ == "__main__":
    print("Collecting Hardware Information...")

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

    print("\n\nBenchmark Results (Seconds):")
    for name in components.keys():
        t = benchmark_times.get(name)
        if t is not None:
            print(f"{name:15}: {t:.6f} sec")
        else:
            print(f"{name:15}: Failed")
