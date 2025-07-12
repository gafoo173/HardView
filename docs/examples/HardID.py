#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ================================================
# HardView ID CLI Generator
#
# Commands:
#   --type [name]    → Generate ID from a specific component (cpu, ram, gpu...)
#   --all            → Generate IDs for all components
#   --combined       → Generate one unified ID from all components
#   --dis            → Disable logo animation
#   -help            → Show help message
# ================================================

import argparse
import json
import hashlib
import HardView
import sys
import time
import os
import random

# ASCII art for HARD ID logo
hard_id_logo = [
    " ___   ___  ________  ________  ________          ___  ________     ",
    "|\\  \\ |\\  \\|\\   __  \\|\\   __  \\|\\   ___ \\        |\\  \\|\\   ___ \\    ",
    "\\ \\  \\\\ \\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\_|\\ \\       \\ \\  \\ \\  \\_|\\ \\   ",
    " \\ \\   __  \\ \\   __  \\ \\   _  _\\ \\  \\ \\\\ \\       \\ \\  \\ \\  \\ \\\\ \\  ",
    "  \\ \\  \\ \\  \\ \\  \\ \\  \\ \\  \\\\  \\\\ \\  \\_\\\\ \\       \\ \\  \\ \\  \\_\\\\ \\ ",
    "   \\ \\__\\ \\__\\ \\__\\ \\__\\ \\__\\\\ _\\\\ \\_______\\       \\ \\__\\ \\_______\\",
    "    \\|__|\\|__|\\|__|\\|__|\\|__|\\|__|\\|_______|        \\|__|\\|_______|",
    "                                                                   ",
    "                Hardware Identity Generator v2.0                  ",
    "                Generate unique IDs for your hardware             "
]

# Single orange color for the logo
logo_color = "\033[38;5;208m"  # Orange
reset_color = "\033[0m"        # Reset to default color

info_sources = {
    "cpu": HardView.get_cpu_info_objects,
    "gpu": HardView.get_gpu_info_objects,
    "ram": HardView.get_ram_info_objects,
    "disk": HardView.get_disk_info_objects,
    "smart": HardView.get_smart_info_objects,
    "bios": HardView.get_bios_info_objects,
    "system": HardView.get_system_info_objects,
    "baseboard": HardView.get_baseboard_info_objects,
    "chassis": HardView.get_chassis_info_objects,
    "net": HardView.get_network_info_objects
}

def clear_screen():
    """Clear the terminal screen"""
    os.system("cls" if os.name == "nt" else "clear")

def show_logo():
    """Display the HARD ID logo with animation"""
    clear_screen()
    
    # Gradual appearance animation with single color
    for line in hard_id_logo:
        print(logo_color + line + reset_color)
        time.sleep(0.12)
    
    print("\n" + "="*60)
    
    # Simple loading dots animation
    loading_text = "Initializing Hardware Scanner"
    print(f"\n{loading_text}", end="")
    
    for i in range(8):
        print(".", end="", flush=True)
        time.sleep(0.1)
    
    print(" Ready!")
    time.sleep(0.5)
    clear_screen()

def extract_values(data):
    """Extract all values from nested data structures"""
    if isinstance(data, dict):
        values = []
        for v in data.values():
            values += extract_values(v)
        return values
    elif isinstance(data, list):
        values = []
        for item in data:
            values += extract_values(item)
        return values
    else:
        return [str(data)]

def generate_id(hw_type):
    """Generate hardware ID for a specific component type"""
    if hw_type not in info_sources:
        print(f"Unsupported type: {hw_type}")
        return
    
    try:
        print(f"Scanning {hw_type.upper()} hardware...")
        
        # Simple dots animation
        for i in range(4):
            print(".", end="", flush=True)
            time.sleep(0.15)
        print("")
        
        obj = info_sources[hw_type](False)
        all_data = extract_values(obj)
        raw_string = '|'.join(all_data)
        hash_id = hashlib.sha256(raw_string.encode()).hexdigest()
        
        print(f"Success!")
        print(f"ID [{hw_type.upper()}]: {hash_id}\n")
        
    except Exception as e:
        print(f"Error while reading {hw_type} info: {e}")

def generate_combined_id():
    """Generate a single combined ID from all hardware components"""
    try:
        print("Scanning all hardware components...")
        
        combined_data = []
        for key in info_sources:
            try:
                obj = info_sources[key](False)
                combined_data += extract_values(obj)
                print(".", end="", flush=True)
                time.sleep(0.1)
            except Exception as e:
                print(f"\nWarning: Could not read {key} info: {e}")
                continue
        
        print("")
        
        if not combined_data:
            print("Error: No hardware data could be collected")
            return
        
        raw_string = '|'.join(combined_data)
        hash_id = hashlib.sha256(raw_string.encode()).hexdigest()
        
        print(f"Combined scan complete!")
        print(f"Combined System ID: {hash_id}\n")
        
    except Exception as e:
        print(f"Error while generating combined ID: {e}")

def show_help():
    """Display help information"""
    print("="*60)
    print("             HARD ID - Hardware Identity Generator")
    print("="*60)
    print("\nUsage:")
    print("  --type [name]     → Generate ID from one component")
    print("                      Available: cpu, ram, gpu, disk, smart, bios,")
    print("                               system, baseboard, chassis, net")
    print("  --all             → Generate all component IDs")
    print("  --combined        → Generate one global ID from everything")
    print("  --dis             → Disable logo animation")
    print("  -help             → Show this help message")
    print("\nExamples:")
    print("  python hardview_id.py --type cpu")
    print("  python hardview_id.py --all")
    print("  python hardview_id.py --combined --dis")
    print("\nNote: Requires HardView module to be installed")

def main():
    """Main function to handle command line arguments and execute appropriate actions"""
    if "-help" in sys.argv:
        show_help()
        return

    parser = argparse.ArgumentParser(description="HardView Hardware ID Generator", add_help=False)
    parser.add_argument("--type", choices=info_sources.keys(), help="Specify the hardware component to generate ID from")
    parser.add_argument("--all", action="store_true", help="Generate ID for all supported types")
    parser.add_argument("--combined", action="store_true", help="Generate one combined ID from all hardware info")
    parser.add_argument("--dis", action="store_true", help="Disable logo animation")

    args = parser.parse_args()

    # Show logo animation unless disabled
    if not args.dis:
        show_logo()

    # Execute based on arguments
    if args.combined:
        generate_combined_id()
    elif args.all:
        print(f"Generating IDs for all hardware components...\n")
        for key in info_sources:
            generate_id(key)
            time.sleep(0.2)  # Short pause between each ID
    elif args.type:
        generate_id(args.type)
    else:
        show_help()

if __name__ == "__main__":
    main()