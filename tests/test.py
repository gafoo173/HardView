import json
import platform
import inspect

# Try importing the module directly (as it's a Python extension)
try:
    import HardView
except ImportError as e:
    print(f"Error importing HardView module: {e}")
    print("Please make sure the HardView.pyd (or HardView.so) file is present in the same directory or in the PYTHONPATH.")
    exit()

def print_json_result(func_name, result_str):
    """
    Print the JSON result from a string and handle errors.
    """
    if result_str:
        try:
            json_obj = json.loads(result_str)
            print(f"\n--- {func_name} (JSON) ---")
            print(json.dumps(json_obj, indent=4, ensure_ascii=False))
        except json.JSONDecodeError:
            print(f"\n--- {func_name} (JSON) - JSON decoding error ---\n{result_str}")
    else:
        print(f"\n--- {func_name} (JSON) - No data returned ---")

def print_python_object_result(func_name, result_obj):
    """
    Print the result as a Python object.
    """
    if result_obj is not None:
        print(f"\n--- {func_name} (Python Object) ---")
        print(result_obj)
    else:
        print(f"\n--- {func_name} (Python Object) - No data returned ---")

def run_test(func_name, is_monitoring_func=False, *extra_args):
    """
    Run a function and print both JSON and Python object results (if available).
    """
    print(f"\n=== Testing function: {func_name} ===")
    
    json_func = getattr(HardView, func_name, None)
    obj_func = getattr(HardView, f"{func_name}_objects", None)

    # Test JSON version
    if json_func:
        json_result_str = None
        try:
            if is_monitoring_func:
                # Monitoring functions usually only take duration and interval
                json_result_str = json_func(*extra_args)
            else:
                # Try passing 'True' as JSON flag, fallback if fails
                try:
                    json_result_str = json_func(True, *extra_args)
                except TypeError as e:
                    if "takes no arguments (1 given)" in str(e) or "takes 0 positional arguments but 1 was given" in str(e):
                        print(f"  [Notice] {func_name} (JSON) does not expect JSON argument. Retrying without.")
                        json_result_str = json_func(*extra_args)
                    else:
                        raise
        except Exception as e:
            print(f"Unexpected error when calling {func_name} (JSON): {e}")
        
        if json_result_str is not None:
            print_json_result(func_name, json_result_str)
        else:
            print(f"  [Failure] {func_name} (JSON) returned no valid data after attempts.")
    else:
        print(f"Function {func_name} (JSON) not found in module.")

    # Test Python object version
    if obj_func:
        obj_result = None
        try:
            if is_monitoring_func:
                obj_result = obj_func(*extra_args)
            else:
                try:
                    obj_result = obj_func(False, *extra_args)
                except TypeError as e:
                    if "takes no arguments (1 given)" in str(e) or "takes 0 positional arguments but 1 was given" in str(e):
                        print(f"  [Notice] {obj_func.__name__} (Python Object) does not expect JSON argument. Retrying without.")
                        obj_result = obj_func(*extra_args)
                    else:
                        raise
        except Exception as e:
            print(f"Unexpected error when calling {obj_func.__name__} (Python Object): {e}")

        if obj_result is not None:
            print_python_object_result(func_name, obj_result)
        else:
            print(f"  [Failure] {func_name}_objects (Python Object) returned no valid data after attempts.")
    else:
        print(f"Function {func_name}_objects (Python Object) not found in module.")

# List of functions to test
functions_to_test = [
    "get_bios_info",
    "get_system_info",
    "get_baseboard_info",
    "get_chassis_info",
    "get_cpu_info",
    "get_ram_info",
    "get_disk_info",
    "get_network_info",
    "get_cpu_usage",
    "get_ram_usage",
    "get_system_performance",
    "get_partitions_info",
    "get_smart_info",
    "get_gpu_info",
]

# Monitoring functions (require extra arguments)
monitoring_functions_to_test = [
    ("monitor_cpu_usage_duration", 5, 1000), # 5 seconds, 1000 ms interval
    ("monitor_ram_usage_duration", 5, 1000),
    ("monitor_system_performance_duration", 5, 1000),
]

# Run standard tests
for func_name in functions_to_test:
    run_test(func_name)

# Run monitoring function tests with is_monitoring_func=True
for func_name, duration, interval in monitoring_functions_to_test:
    run_test(func_name, True, duration, interval)

print("\n=== Library test completed ===")
