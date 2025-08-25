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


def test_pysensor():
    """Test PySensor class functionality (Windows only)"""
    print_separator("Testing PySensor")
    
    if sys.platform != "win32":
        print_warning("PySensor is only supported on Windows - skipping tests")
        return
    
    try:
        from HardView.LiveView import PySensor
        print_success("Successfully imported PySensor")
        
        # Initialize sensor monitor
        sensor = PySensor()
        sensor.update()
        print_success("Successfully created PySensor instance And Update")
        
        # Test getting all sensors
        print_info("Testing sensor discovery...")
        all_sensors = sensor.getAllSensors()
        print(f"  Total sensors found: {len(all_sensors)}")
        
        # Test getting sensor values by name
        print_info("Testing sensor value retrieval (first 10 sensors)...")
        for i, sensor_name in enumerate(all_sensors[:10]):
            try:
                value = sensor.GetValueByName(sensor_name)
                print(f"  {i+1}. {sensor_name}: {value:.1f}")
            except:
                print(f"  {i+1}. {sensor_name}: Unable to read")
        
        # Test getting all fan RPMs
        print_info("Testing fan RPM retrieval...")
        fan_rpms = sensor.getAllFanRPMs()
        print(f"  Total fans found: {len(fan_rpms)}")
        
        if fan_rpms:
            print("  Fan speeds:")
            for fan_name, rpm in fan_rpms:
                print(f"    {fan_name}: {rpm:.0f} RPM")
        else:
            print("  No fan data available")
        
        # Test update functionality
        print_info("Testing sensor updates...")
        sensor.update()
        print("  Sensor data updated")
        
        # Test reget functionality
        print_info("Testing reget functionality...")
        sensor.reget()
        print("  Sensor data refreshed")
        
        print_success("PySensor tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PySensor: {e}")
    except Exception as e:
        print_error(f"PySensor test failed: {e}")
        print_info("Note: Advanced sensor monitoring requires proper hardware support")
        traceback.print_exc()

if __name__ == "__main__":
    test_pysensor()
