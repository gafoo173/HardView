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


def test_pylive_gpu():
    """Test PyLiveGpu class functionality (Windows only)"""
    print_separator("Testing PyLiveGpu")
    
    if sys.platform != "win32":
        print_warning("PyLiveGpu is only supported on Windows - skipping tests")
        return
    
    try:
        from HardView.LiveView import PyLiveGpu
        print_success("Successfully imported PyLiveGpu")
        
        # Initialize GPU monitor
        gpu_monitor = PyLiveGpu()
        print_success("Successfully created PyLiveGpu instance")
        
        # Test GPU counter count
        counter_count = gpu_monitor.get_counter_count()
        print(f"  GPU Counter Count: {counter_count}")
        
        # Test different usage measurements
        print_info("Testing GPU usage measurements...")
        for i in range(3):
            total_usage = gpu_monitor.get_usage(interval_ms=1000)
            avg_usage = gpu_monitor.get_average_usage(interval_ms=1000)
            max_usage = gpu_monitor.get_max_usage(interval_ms=1000)
            
            print(f"  Cycle {i+1}:")
            print(f"    Total GPU Usage: {total_usage:.2f}%")
            print(f"    Average GPU Usage: {avg_usage:.2f}%")
            print(f"    Max GPU Usage: {max_usage:.2f}%")
            time.sleep(0.5)
        
        print_success("PyLiveGpu tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyLiveGpu: {e}")
    except Exception as e:
        print_error(f"PyLiveGpu test failed: {e}")
        print_info("Note: PyLiveGpu might not work optimally with integrated GPUs")
        traceback.print_exc()

if __name__ == "__main__":
    test_pylive_gpu()
