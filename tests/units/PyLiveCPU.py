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


def test_pylive_cpu():
    """Test PyLiveCPU class functionality"""
    print_separator("Testing PyLiveCPU")
    
    try:
        from HardView.LiveView import PyLiveCPU
        print_success("Successfully imported PyLiveCPU")
        
        # Initialize CPU monitor
        cpu_monitor = PyLiveCPU()
        print_success("Successfully created PyLiveCPU instance")
        
        # Test CPU usage monitoring
        print_info("Testing CPU usage monitoring...")
        for i in range(3):
            usage = cpu_monitor.get_usage(interval_ms=1000)
            print(f"  Cycle {i+1}: CPU Usage = {usage:.2f}%")
            time.sleep(0.5)
        
        # Test CPUID information
        print_info("Testing CPUID information...")
        cpu_info = cpu_monitor.cpuid()
        print(f"  Retrieved {len(cpu_info)} CPU features")
        
        # Display first 10 features
        print("  First 10 CPU Features:")
        for i, (feature, value) in enumerate(cpu_info[:10]):
            print(f"    {i+1}. {feature}: {value}")
        
        # Windows-specific tests
        if sys.platform == "win32":
            print_info("Testing Windows-specific CPU features...")
            try:
                # Test core count
                core_count = cpu_monitor.CpuSnapShot(core=0, coreNumbers=True)
                print(f"  Total CPU cores: {core_count}")
                
                # Test CPU snapshot for first core
                snapshot = cpu_monitor.CpuSnapShot(core=0)
                print("  Core 0 Snapshot:")
                for key, value in snapshot.items():
                    print(f"    {key}: {value}")
                    
            except Exception as e:
                print_warning(f"Windows-specific CPU features failed: {e}")
        else:
            print_info("Windows-specific features skipped (not on Windows)")
            
        print_success("PyLiveCPU tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyLiveCPU: {e}")
    except Exception as e:
        print_error(f"PyLiveCPU test failed: {e}")
        traceback.print_exc()

if __name__ == "__main__":
    test_pylive_cpu()
