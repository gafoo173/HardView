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


def test_pylive_ram():
    """Test PyLiveRam class functionality"""
    print_separator("Testing PyLiveRam")
    
    try:
        from HardView.LiveView import PyLiveRam
        print_success("Successfully imported PyLiveRam")
        
        # Initialize RAM monitor
        ram_monitor = PyLiveRam()
        print_success("Successfully created PyLiveRam instance")
        
        # Test RAM usage as percentage
        print_info("Testing RAM usage monitoring (percentage)...")
        for i in range(3):
            usage_percent = ram_monitor.get_usage()
            print(f"  Cycle {i+1}: RAM Usage = {usage_percent:.2f}%")
            time.sleep(0.5)
        
        # Test RAM usage in raw bytes
        print_info("Testing RAM usage monitoring (raw bytes)...")
        ram_raw = ram_monitor.get_usage(Raw=True)
        used_gb = ram_raw[0] / (1024**3)
        total_gb = ram_raw[1] / (1024**3)
        print(f"  Used RAM: {used_gb:.2f} GB")
        print(f"  Total RAM: {total_gb:.2f} GB")
        print(f"  Usage: {(used_gb/total_gb)*100:.2f}%")
        
        # Performance test
        print_info("Testing performance (10 rapid queries)...")
        start_time = time.time()
        for _ in range(10):
            ram_monitor.get_usage()
        end_time = time.time()
        avg_time = (end_time - start_time) / 10 * 1000  # Convert to milliseconds
        print(f"  Average query time: {avg_time:.3f} ms")
        
        print_success("PyLiveRam tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyLiveRam: {e}")
    except Exception as e:
        print_error(f"PyLiveRam test failed: {e}")
        traceback.print_exc()

if __name__ == "__main__":
    test_pylive_ram()
