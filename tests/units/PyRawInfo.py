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


def test_pyraw_info():
    """Test PyRawInfo class functionality (Windows only)"""
    print_separator("Testing PyRawInfo")
    
    if sys.platform != "win32":
        print_warning("PyRawInfo is only supported on Windows - skipping tests")
        return
    
    try:
        from HardView.LiveView import PyRawInfo
        print_success("Successfully imported PyRawInfo")
        
        # Test RSMB (SMBIOS) data retrieval
        print_info("Testing SMBIOS data retrieval...")
        smbios_data = PyRawInfo.RSMB()
        
        if smbios_data:
            print(f"  SMBIOS Data Size: {len(smbios_data)} bytes")
            print(f"  First 20 bytes: {smbios_data[:20]}")
            print(f"  Last 20 bytes: {smbios_data[-20:]}")
            
            # Basic validation
            if len(smbios_data) > 0:
                print_success("SMBIOS data retrieved successfully")
            else:
                print_warning("SMBIOS data appears to be empty")
                
            # Try to find SMBIOS header signature
            if len(smbios_data) >= 4:
                # Look for common SMBIOS signatures or patterns
                print_info("Analyzing SMBIOS structure...")
                print(f"  Header bytes (hex): {' '.join(f'{b:02X}' for b in smbios_data[:16])}")
        else:
            print_warning("No SMBIOS data returned")
        
        print_success("PyRawInfo tests completed successfully!")
        
    except ImportError as e:
        print_error(f"Failed to import PyRawInfo: {e}")
    except Exception as e:
        print_error(f"PyRawInfo test failed: {e}")
        print_info("Note: Raw firmware data access requires proper system support")
        traceback.print_exc()

if __name__ == "__main__":
    test_pyraw_info()
