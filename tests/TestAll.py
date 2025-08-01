import time
import platform

# Check the OS and import the correct module
# On Windows, it will be LiveView.pyd, on Linux LiveView.so
IS_WINDOWS = platform.system() == "Windows"
try:
    from HardView.LiveView import PyLiveCPU, PyLiveRam, PyLiveDisk, PyLiveNetwork
    if IS_WINDOWS:
        from HardView.LiveView import PyLiveGpu, PyRawInfo
except ImportError as e:
    print(f"Error importing module: {e}")
    print("Please ensure 'LiveView.pyd' (Windows) or 'LiveView.so' (Linux) is in the same directory.")
    exit()

def separator(title):
    """Prints a formatted separator line."""
    print("\n" + "=" * 40)
    print(f" {title}")
    print("=" * 40)

def timed_call(func, *args, **kwargs):
    """Calls a function, measures its execution time, and returns the result and duration."""
    start = time.perf_counter()
    result = func(*args, **kwargs)
    end = time.perf_counter()
    duration_us = (end - start) * 1_000_000  # Convert to microseconds
    return result, duration_us

def try_cpu():
    separator("CPU Monitoring")
    try:
        cpu = PyLiveCPU()
        
        # Test get_usage
        usage, elapsed = timed_call(cpu.get_usage, 1000)
        print(f"CPU Usage: {usage:.2f}% (Time: {elapsed:,.0f} µs)")

        # Test cpuid
        info, elapsed = timed_call(cpu.cpuid)
        print(f"CPUID Info (Time: {elapsed:,.0f} µs):")
        for line in info:
            print(f"  - {line}")

        # Test CpuSnapShot (Windows-only)
        if IS_WINDOWS:
            # Get core count
            core_count, elapsed = timed_call(cpu.CpuSnapShot, 0, coreNumbers=True)
            print(f"CPU Core Count: {core_count} (Time: {elapsed:,.0f} µs)")
            
            # Get snapshot for the first core
            if core_count > 0:
                snapshot, elapsed = timed_call(cpu.CpuSnapShot, 0)
                print(f"Snapshot for Core 0 (Time: {elapsed:,.0f} µs):")
                for key, value in snapshot.items():
                    print(f"  - {key}: {value:,.0f}")
                    
    except Exception as e:
        print(f"An error occurred in CPU tests: {e}")

def try_ram():
    separator("RAM Monitoring")
    try:
        ram = PyLiveRam()
        
        # Test get_usage (percentage)
        usage, elapsed = timed_call(ram.get_usage)
        print(f"RAM Usage: {usage:.2f}% (Time: {elapsed:,.0f} µs)")
        
        # Test get_usage (raw bytes)
        raw_usage, elapsed = timed_call(ram.get_usage, Raw=True)
        used_gb = raw_usage[0] / (1024**3)
        total_gb = raw_usage[1] / (1024**3)
        print(f"RAM Raw: {used_gb:.2f} GB / {total_gb:.2f} GB (Time: {elapsed:,.0f} µs)")

    except Exception as e:
        print(f"An error occurred in RAM tests: {e}")

def try_disk():
    separator("Disk I/O Monitoring")
    # Test mode 0 (% Disk Time - Windows only)
    if IS_WINDOWS:
        try:
            disk0 = PyLiveDisk(0)
            usage, elapsed = timed_call(disk0.get_usage, 1000)
            print(f"Disk % Time (mode 0): {usage:.2f}% (Time: {elapsed:,.0f} µs)")
        except Exception as e:
            print(f"Disk mode 0 error: {e}")
    else:
        print("Disk mode 0 (% usage) is not supported on this platform.")

    # Test mode 1 (Read/Write speed)
    try:
        disk1 = PyLiveDisk(1)
        usage, elapsed = timed_call(disk1.get_usage, 1000)
        read_label, read_val = usage[0]
        write_label, write_val = usage[1]
        print(f"Disk R/W (mode 1): {read_label}: {read_val:.2f}, {write_label}: {write_val:.2f} (Time: {elapsed:,.0f} µs)")
        
        # Test HighDiskUsage
        high, high_time = timed_call(disk1.HighDiskUsage, threshold_mbps=100.0)
        print(f"High Disk Usage (>100 MB/s): {high} (Time: {high_time:,.0f} µs)")
    except Exception as e:
        print(f"Disk mode 1 error: {e}")

def try_network():
    separator("Network I/O Monitoring")
    try:
        net = PyLiveNetwork()
        
        # Test mode 0 (Total MB/s)
        usage, elapsed = timed_call(net.get_usage, 1000, 0)
        print(f"Total Network Usage (mode 0): {usage:.4f} MB/s (Time: {elapsed:,.0f} µs)")

        # Test mode 1 (Per Adapter)
        usage, elapsed = timed_call(net.get_usage, 1000, 1)
        print(f"Per-Adapter Usage (mode 1) (Time: {elapsed:,.0f} µs):")
        if not usage:
            print("  - No active network adapters found.")
        else:
            for name, val in usage:
                print(f"  - {name}: {val:.4f} MB/s")
        
        # Test getHighCard
        high_card, high_time = timed_call(net.getHighCard)
        print(f"Highest Usage Card: {high_card} (Time: {high_time:,.0f} µs)")
    except Exception as e:
        print(f"An error occurred in Network tests: {e}")

def try_windows_only():
    if not IS_WINDOWS:
        return

    # Test GPU
    separator("GPU Monitoring (Windows-only)")
    try:
        gpu = PyLiveGpu()
        usage, elapsed = timed_call(gpu.get_usage, 1000)
        print(f"GPU Usage: {usage:.2f}% (Time: {elapsed:,.0f} µs)")
    except Exception as e:
        print(f"GPU Monitoring Error: {e}")

    # Test RawInfo (RSMB)
    separator("Raw Firmware Info (Windows-only)")
    try:
        data, elapsed = timed_call(PyRawInfo.RSMB)
        print(f"RSMB table retrieved successfully ({len(data)} bytes). (Time: {elapsed:,.0f} µs)")
    except Exception as e:
        print(f"RSMB Retrieval Error: {e}")

if __name__ == "__main__":
    print(f"Running LiveView Test Suite on: {platform.system()}")
    try_cpu()
    try_ram()
    try_disk()
    try_network()
    try_windows_only()

