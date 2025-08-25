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


def test_pytemp_gpu():
    """Test PyTempGpu class functionality (Windows only)"""
    print_separator("Testing PyTempGpu")
    
    if sys.platform != "win32":
        print_warning("PyTempGpu is only supported on Windows - skipping tests")
        return
    
    try:
        from HardView.LiveView import PyTempGpu
        print_success("Successfully imported PyTempGpu")
        
        # Initialize GPU temperature monitor
        gpu_temp = PyTempGpu()
        print_success("Successfully created PyTempGpu instance")
        
        # Test temperature readings
        print_info("Testing GPU temperature readings...")
        for i in range(3):
            gpu_temp.update()  # Update readings
            
            temp = gpu_temp.get_temp()
            fan_rpm = gpu_temp.get_fan_rpm()
            
            print(f"  Cycle {i+1}:")
            print(f"    GPU Temperature: {temp:.1f}°C")
            print(f"    GPU Fan RPM: {fan_rpm:.0f} RPM")
            time.sleep(1)
        
        # Test reget functionality
        print_info("Testing reget functionality...")
        gpu_temp.reget()
        temp_after_reget = gpu_temp.get_temp()
        print(f"  Temperature after reget: {temp_after_reget:.1f}°C")
        
        print_success("PyTempGpu tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyTempGpu: {e}")
    except Exception as e:
        print_error(f"PyTempGpu test failed: {e}")
        print_info("Note: GPU temperature monitoring requires proper hardware support")
        traceback.print_exc()

if __name__ == "__main__":
    test_pytemp_gpu()
