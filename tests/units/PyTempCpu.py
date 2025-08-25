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


def test_pytemp_cpu():
    """Test PyTempCpu class functionality (Windows only)"""
    print_separator("Testing PyTempCpu")
    
    if sys.platform != "win32":
        print_warning("PyTempCpu is only supported on Windows - skipping tests")
        return
    
    try:
        from HardView.LiveView import PyTempCpu
        print_success("Successfully imported PyTempCpu")
        
        # Initialize CPU temperature monitor
        cpu_temp = PyTempCpu()
        print_success("Successfully created PyTempCpu instance")
        
        # Test temperature readings
        print_info("Testing CPU temperature readings...")
        for i in range(3):
            cpu_temp.update()  # Update readings
            
            temp = cpu_temp.get_temp()
            max_temp = cpu_temp.get_max_temp()
            avg_temp = cpu_temp.get_avg_temp()
            fan_rpm = cpu_temp.get_fan_rpm()
            
            print(f"  Cycle {i+1}:")
            print(f"    CPU Temperature: {temp:.1f}°C")
            print(f"    Max Core Temperature: {max_temp:.1f}°C")
            print(f"    Average Core Temperature: {avg_temp:.1f}°C")
            print(f"    CPU Fan RPM: {fan_rpm:.0f} RPM")
            time.sleep(1)
        
        # Test reget functionality
        print_info("Testing reget functionality...")
        cpu_temp.reget()
        temp_after_reget = cpu_temp.get_temp()
        print(f"  Temperature after reget: {temp_after_reget:.1f}°C")
        
        print_success("PyTempCpu tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyTempCpu: {e}")
    except Exception as e:
        print_error(f"PyTempCpu test failed: {e}")
        print_info("Note: Temperature monitoring requires proper hardware support")
        traceback.print_exc()

if __name__ == "__main__":
    test_pytemp_cpu()
