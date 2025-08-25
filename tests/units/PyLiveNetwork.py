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


def test_pylive_network():
    """Test PyLiveNetwork class functionality"""
    print_separator("Testing PyLiveNetwork")
    
    try:
        from HardView.LiveView import PyLiveNetwork
        print_success("Successfully imported PyLiveNetwork")
        
        # Initialize network monitor
        net_monitor = PyLiveNetwork()
        print_success("Successfully created PyLiveNetwork instance")
        
        # Test Mode 0 (Total Usage)
        print_info("Testing Mode 0 (Total Network Usage)...")
        for i in range(3):
            total_traffic = net_monitor.get_usage(interval=1000, mode=0)
            print(f"  Cycle {i+1}: Total Network Usage = {total_traffic:.4f} MB/s")
            time.sleep(0.5)
        
        # Test Mode 1 (Per-Interface Usage)
        print_info("Testing Mode 1 (Per-Interface Usage)...")
        interface_traffic = net_monitor.get_usage(interval=1000, mode=1)
        print(f"  Found {len(interface_traffic)} network interfaces:")
        for interface, speed in interface_traffic:
            print(f"    {interface}: {speed:.4f} MB/s")
        
        # Test highest usage interface
        print_info("Testing highest usage interface detection...")
        busiest_card = net_monitor.getHighCard()
        print(f"  Busiest Interface: {busiest_card}")
        
        print_success("PyLiveNetwork tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyLiveNetwork: {e}")
    except Exception as e:
        print_error(f"PyLiveNetwork test failed: {e}")
        traceback.print_exc()

if __name__ == "__main__":
    test_pylive_network()
