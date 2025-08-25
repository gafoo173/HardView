#!/usr/bin/env python3
"""
HardView.LiveView Unit Test
Generated on: 2025-08-25 12:12:14
Auto-generated test file for HardView.LiveView classes
"""

import sys
import time
import traceback

def print_separator(title):
    """Print a formatted separator"""
    print("\n" + "="*60)
    print(f" {title}")
    print("="*60)

def print_error(error_msg):
    """Print formatted error message"""
    print(f"[ERROR] Error: {error_msg}")

def print_success(success_msg):
    """Print formatted success message"""
    print(f"[OK] {success_msg}")

def print_info(info_msg):
    """Print formatted info message"""
    print(f"[INFO]  {info_msg}")

def print_warning(warning_msg):
    """Print formatted warning message"""
    print(f"[W]  {warning_msg}")


def test_pylinux_sensor():
    """Test PyLinuxSensor class functionality (Linux only)"""
    print_separator("Testing PyLinuxSensor")
    
    if sys.platform != "linux":
        print_warning("PyLinuxSensor is only supported on Linux - skipping tests")
        return
    
    try:
        from HardView.LiveView import PyLinuxSensor
        print_success("Successfully imported PyLinuxSensor")
        
        # Initialize Linux sensor monitor
        linux_sensor = PyLinuxSensor()
        print_success("Successfully created PyLinuxSensor instance")
        
        # Test specific temperature readings
        print_info("Testing specific temperature readings...")
        cpu_temp = linux_sensor.getCpuTemp()
        mb_temp = linux_sensor.getMotherboardTemp()
        chipset_temp = linux_sensor.getChipsetTemp()
        vrm_temp = linux_sensor.getVRMTemp()
        drive_temp = linux_sensor.getDriveTemp()
        
        print(f"  CPU Temperature: {cpu_temp:.1f}°C" if cpu_temp > 0 else "  CPU Temperature: Not available")
        print(f"  Motherboard Temperature: {mb_temp:.1f}°C" if mb_temp > 0 else "  Motherboard Temperature: Not available")
        print(f"  Chipset Temperature: {chipset_temp:.1f}°C" if chipset_temp > 0 else "  Chipset Temperature: Not available")
        print(f"  VRM Temperature: {vrm_temp:.1f}°C" if vrm_temp > 0 else "  VRM Temperature: Not available")
        print(f"  Drive Temperature: {drive_temp:.1f}°C" if drive_temp > 0 else "  Drive Temperature: Not available")
        
        # Test getting all sensor names
        print_info("Testing sensor discovery...")
        all_sensors = linux_sensor.getAllSensorNames()
        print(f"  Total sensors found: {len(all_sensors)}")
        
        # Test finding specific sensors
        print_info("Testing sensor search...")
        core_sensors = linux_sensor.findSensorName("Core")
        print(f"  Core sensors found: {len(core_sensors)}")
        
        if core_sensors:
            print("  Core sensor details:")
            for sensor_name, index in core_sensors[:5]:  # Show first 5
                temp = linux_sensor.GetSensorTemp(sensor_name, True)
                if temp > 0:
                    print(f"    {sensor_name}: {temp:.1f}°C")
        
        # Test getting all sensors with temperatures
        print_info("Testing all sensors with temperatures...")
        sensors_with_temp = linux_sensor.GetSensorsWithTemp()
        valid_temps = [(name, temp) for name, temp in sensors_with_temp if temp > 0]
        print(f"  Sensors with valid temperatures: {len(valid_temps)}")
        
        if valid_temps:
            print("  First 10 valid temperature readings:")
            for i, (sensor_name, temp) in enumerate(valid_temps[:10]):
                print(f"    {i+1}. {sensor_name}: {temp:.1f}°C")
        
        # Test update functionality
        print_info("Testing sensor updates...")
        linux_sensor.update()
        print("  Sensor data updated")
        
        # Test update with names refresh
        print_info("Testing update with names refresh...")
        linux_sensor.update(names=True)
        print("  Sensor data and names updated")
        
        print_success("PyLinuxSensor tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyLinuxSensor: {e}")
        print_info("Note: PyLinuxSensor requires lm-sensors library to be installed")
        print_info("Install with: sudo apt-get install lm-sensors libsensors4-dev")
    except Exception as e:
        print_error(f"PyLinuxSensor test failed: {e}")
        print_info("Note: Linux sensor monitoring requires lm-sensors and proper hardware support")
        traceback.print_exc()

if __name__ == "__main__":
    test_pylinux_sensor()
