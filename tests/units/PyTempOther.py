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


def test_pytemp_other():
    """Test PyTempOther class functionality (Windows only)"""
    print_separator("Testing PyTempOther")
    
    if sys.platform != "win32":
        print_warning("PyTempOther is only supported on Windows - skipping tests")
        return
    
    try:
        from HardView.LiveView import PyTempOther
        print_success("Successfully imported PyTempOther")
        
        # Initialize other temperature monitor
        other_temp = PyTempOther()
        print_success("Successfully created PyTempOther instance")
        
        # Test temperature readings
        print_info("Testing other temperature readings...")
        for i in range(3):
            other_temp.update()  # Update readings
            
            mb_temp = other_temp.get_mb_temp()
            storage_temp = other_temp.get_Storage_temp()
            
            print(f"  Cycle {i+1}:")
            print(f"    Motherboard Temperature: {mb_temp:.1f}°C")
            print(f"    Storage Temperature: {storage_temp:.1f}°C")
            time.sleep(1)
        
        # Test reget functionality
        print_info("Testing reget functionality...")
        other_temp.reget()
        mb_temp_after_reget = other_temp.get_mb_temp()
        print(f"  Motherboard temperature after reget: {mb_temp_after_reget:.1f}°C")
        
        print_success("PyTempOther tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyTempOther: {e}")
    except Exception as e:
        print_error(f"PyTempOther test failed: {e}")
        print_info("Note: Temperature monitoring requires proper hardware support")
        traceback.print_exc()

if __name__ == "__main__":
    test_pytemp_other()
