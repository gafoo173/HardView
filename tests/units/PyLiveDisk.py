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


def test_pylive_disk():
    """Test PyLiveDisk class functionality"""
    print_separator("Testing PyLiveDisk")
    
    try:
        from HardView.LiveView import PyLiveDisk
        print_success("Successfully imported PyLiveDisk")
        
        # Test Mode 0 (Percentage) - Windows only
        if sys.platform == "win32":
            print_info("Testing PyLiveDisk Mode 0 (Percentage Usage - Windows only)...")
            try:
                disk_monitor_percent = PyLiveDisk(mode=0)
                print_success("Successfully created PyLiveDisk instance (mode=0)")
                
                for i in range(3):
                    usage_percent = disk_monitor_percent.get_usage(interval=1000)
                    print(f"  Cycle {i+1}: Disk % Time = {usage_percent:.2f}%")
                    time.sleep(0.5)
                    
            except Exception as e:
                print_warning(f"Mode 0 test failed: {e}")
        else:
            print_info("Mode 0 (Percentage) skipped - Windows only feature")
        
        # Test Mode 1 (Read/Write Speed) - Cross-platform
        print_info("Testing PyLiveDisk Mode 1 (Read/Write Speed)...")
        disk_monitor_speed = PyLiveDisk(mode=1)
        print_success("Successfully created PyLiveDisk instance (mode=1)")
        
        for i in range(3):
            rw_speed = disk_monitor_speed.get_usage(interval=1000)
            read_speed = rw_speed[0][1]
            write_speed = rw_speed[1][1]
            print(f"  Cycle {i+1}: Read = {read_speed:.2f} MB/s, Write = {write_speed:.2f} MB/s")
            time.sleep(0.5)
        
        # Test high disk usage detection
        print_info("Testing high disk usage detection...")
        is_high_50 = disk_monitor_speed.HighDiskUsage(threshold_mbps=50.0)
        is_high_100 = disk_monitor_speed.HighDiskUsage(threshold_mbps=100.0)
        print(f"  High usage (>50 MB/s): {is_high_50}")
        print(f"  High usage (>100 MB/s): {is_high_100}")
        
        print_success("PyLiveDisk tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyLiveDisk: {e}")
    except Exception as e:
        print_error(f"PyLiveDisk test failed: {e}")
        traceback.print_exc()

if __name__ == "__main__":
    test_pylive_disk()
