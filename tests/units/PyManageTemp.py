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


def test_pymanage_temp():
    """Test PyManageTemp class functionality (Windows only)"""
    print_separator("Testing PyManageTemp")
    
    if sys.platform != "win32":
        print_warning("PyManageTemp is only supported on Windows - skipping tests")
        return
    
    try:
        from HardView.LiveView import PyManageTemp, PyTempCpu
        print_success("Successfully imported PyManageTemp")
        
        # Initialize temperature manager
        temp_manager = PyManageTemp()
        print_success("Successfully created PyManageTemp instance")
        
        # Test initialization
        print_info("Testing hardware monitor initialization...")
        temp_manager.Init()
        print_success("Hardware monitor initialized")
        
        # Test with CPU temperature monitor (without auto-init)
        print_info("Testing with CPU temperature monitor...")
        try:
            cpu_temp = PyTempCpu(init=False)
            print_success("Created CPU temperature monitor without auto-init")
            
            # Update hardware monitor
            temp_manager.Update()
            print_info("Hardware monitor data updated")
            
            # Get temperature (should use reget since we used global Update)
            cpu_temp.reget()
            temp = cpu_temp.get_temp()
            print(f"  CPU Temperature: {temp:.1f}°C")
            
        except Exception as e:
            print_warning(f"CPU temperature test failed: {e}")
        
        # Test multiple updates
        print_info("Testing multiple updates...")
        for i in range(3):
            temp_manager.Update()
            print(f"  Update cycle {i+1} completed")
            time.sleep(0.5)
        
        # Test cleanup
        print_info("Testing cleanup...")
        temp_manager.Close()
        print_success("Hardware monitor closed successfully")
        
        print_success("PyManageTemp tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyManageTemp: {e}")
    except Exception as e:
        print_error(f"PyManageTemp test failed: {e}")
        print_info("Note: Temperature management requires proper hardware support")
        traceback.print_exc()

if __name__ == "__main__":
    test_pymanage_temp()
