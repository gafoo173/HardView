"""
==============================================================================
 HardView Tools License (HVTL-1.0)

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 License Notice:
 This file is licensed under the HardView Tools License (HVTL-1.0),
 located in the "Tools/LICENSE" file at the project root.

 Note:
 While the main HardView project is licensed under the MIT License,
 all tool-related files under the "Tools" directory are governed by HVTL-1.0.
==============================================================================
"""
#!/usr/bin/env python
# -*- coding: utf-8 -*-

# ================================================
# HardView ID CLI Generator - Enhanced Version
#
# Commands:
#   --type [name]             → Generate ID from a specific component (cpu, ram, gpu...)
#   --all                     → Generate IDs for all components
#   --combined                → Generate one unified ID from all components
#   --dis                     → Disable logo animation
#   -qr                       → Generate QR code for the ID
#   -enc <path>               → Encrypt a file using hardware hash as key
#   -enc -static <path>       → Encrypt using static hardware hash (no key file)
#   -enc -ns <path>           → Encrypt using non-static method (hardware + random)
#   -enc -c <component> <path>→ Encrypt using specific component only
#   -dec <path>               → Decrypt a file using saved key file
#   -dec -auto <path>         → Auto-decrypt using hardware hash (reads header automatically)
#   -key                      → Force save key file even for static encryption
#   -help                     → Show help message
# ================================================

import argparse
import json
import hashlib
import HardView
import sys
import time
import os
import random
import qrcode
import base64
from cryptography.fernet import Fernet
import struct

# ASCII art for HARD ID logo
hard_id_logo = [
    " ___   ___  ________  ________  ________          ___  ________     ",
    "|\\  \\ |\\  \\|\\   __  \\|\\   __  \\|\\   ___ \\         |\\  \\|\\   ___ \\    ",
    "\\ \\  \\\\ \\  \\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\_|\\ \\         \\ \\  \\ \\  \\_|\\ \\   ",
    " \\ \\   __  \\ \\   __  \\ \\   _  _\\ \\  \\ \\\\ \\         \\ \\  \\ \\  \\ \\\\ \\  ",
    "  \\ \\  \\ \\  \\ \\  \\ \\  \\ \\  \\\\  \\\\ \\  \\_\\\\ \\         \\ \\  \\ \\  \\_\\\\ \\ ",
    "   \\ \\__\\ \\__\\ \\__\\ \\__\\ \\__\\\\ _\\\\ \\_______\\         \\ \\__\\ \\_______\\",
    "    \\|__|\\|__|\\|__|\\|__|\\|__|\\|__|\\|_______|          \\|__|\\|_______|",
    "                                                                    ",
    "              Hardware Identity Generator v2.4                      ",
    "              Generate unique IDs for your hardware                 "
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

# Header structure for encrypted files
HEADER_MAGIC = b"HARDID"  # 6 bytes
HEADER_VERSION = 1        # 1 byte
# Encryption types
ENCRYPTION_STATIC = 1     # Static encryption with stability levels (1-3)
ENCRYPTION_NONSTATIC = 2  # Non-static encryption
ENCRYPTION_COMPONENT = 3  # Component-specific encryption
# Total header size: 6 + 1 + 1 + 1 + (component_name_length if component) = variable

def clear_screen():
    """Clear the terminal screen"""
    os.system("cls" if os.name == "nt" else "clear")

def show_logo():
    """Display the HARD ID logo with animation"""
    clear_screen()
    
    for line in hard_id_logo:
        print(logo_color + line + reset_color)
        time.sleep(0.12)
    
    print("\n" + "="*60)
    
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

def generate_hardware_hash(hw_type=None, stability_level=1):
    """Generate hardware hash for specific component or combined with stability levels"""
    try:
        if hw_type and hw_type in info_sources:
            print(f"Scanning {hw_type.upper()} hardware...")
            obj = info_sources[hw_type](False)
            all_data = extract_values(obj)
        else:
            print(f"Scanning hardware components (Stability Level {stability_level})...")
            combined_data = []
            
            # Define components based on stability level
            if stability_level == 1:
                # Level 1: All components
                components_to_scan = ['cpu', 'gpu', 'ram', 'disk', 'smart', 'bios', 'system', 'baseboard', 'chassis', 'net']
                print("Including: CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS, NET")
            elif stability_level == 2:
                # Level 2: All except NET
                components_to_scan = ['cpu', 'gpu', 'ram', 'disk', 'smart', 'bios', 'system', 'baseboard', 'chassis']
                print("Including: CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS")
            elif stability_level == 3:
                # Level 3: All except NET, RAM, GPU, SMART
                components_to_scan = ['cpu', 'disk', 'bios', 'system', 'baseboard', 'chassis']
                print("Including: CPU, DISK, BIOS, SYSTEM, BASEBOARD, CHASSIS")
            else:
                # Default to level 1 if invalid level
                components_to_scan = ['cpu', 'gpu', 'ram', 'disk', 'smart', 'bios', 'system', 'baseboard', 'chassis', 'net']
                print("Invalid stability level, using default (Level 1)")
            
            for key in components_to_scan:
                try:
                    obj = info_sources[key](False)
                    combined_data += extract_values(obj)
                    print(".", end="", flush=True)
                    time.sleep(0.1)
                except Exception as e:
                    print(f"\nWarning: Could not read {key} info: {e}")
                    continue
            all_data = combined_data
        
        if not all_data:
            print("Error: No hardware data could be collected")
            return None
        
        raw_string = '|'.join(all_data)
        hash_id = hashlib.sha256(raw_string.encode()).hexdigest()
        
        print(f"\nHardware hash generated successfully!")
        return hash_id
        
    except Exception as e:
        print(f"Error while generating hardware hash: {e}")
        return None

def generate_id(hw_type, generate_qr=False):
    """Generate hardware ID for a specific component type"""
    if hw_type not in info_sources:
        print(f"Unsupported type: {hw_type}")
        return None
    
    try:
        print(f"Scanning {hw_type.upper()} hardware...")
        
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
        
        if generate_qr:
            generate_qr_code(hash_id, f"{hw_type}_id.png")
        
        return hash_id
        
    except Exception as e:
        print(f"Error while reading {hw_type} info: {e}")
        return None

def generate_combined_id(generate_qr=False):
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
            return None
        
        raw_string = '|'.join(combined_data)
        hash_id = hashlib.sha256(raw_string.encode()).hexdigest()
        
        print(f"Combined scan complete!")
        print(f"Combined System ID: {hash_id}\n")
        
        if generate_qr:
            generate_qr_code(hash_id, "combined_hardware_id.png")
        
        return hash_id
        
    except Exception as e:
        print(f"Error while generating combined ID: {e}")
        return None

def generate_qr_code(data, filename):
    """Generate QR code from the given data and save to file"""
    try:
        print(f"Generating QR code ({filename})...")
        qr = qrcode.QRCode(
            version=1,
            error_correction=qrcode.constants.ERROR_CORRECT_L,
            box_size=10,
            border=4,
        )
        qr.add_data(data)
        qr.make(fit=True)
        
        img = qr.make_image(fill_color="black", back_color="white")
        img.save(filename)
        print(f"QR code saved as {filename}\n")
    except Exception as e:
        print(f"Error generating QR code: {e}")

def create_fernet_key_from_hash(hardware_hash):
    """Create a Fernet-compatible key from hardware hash"""
    # Use the hardware hash to create a 32-byte key for Fernet
    key_material = hashlib.sha256(hardware_hash.encode()).digest()
    return base64.urlsafe_b64encode(key_material)

def create_file_header(encryption_type, level_or_component=None):
    """Create file header with encryption info"""
    header = HEADER_MAGIC + struct.pack('B', HEADER_VERSION) + struct.pack('B', encryption_type)
    
    if encryption_type == ENCRYPTION_STATIC:
        # For static encryption, store stability level
        header += struct.pack('B', level_or_component)
    elif encryption_type == ENCRYPTION_COMPONENT:
        # For component encryption, store component name
        component_bytes = level_or_component.encode('utf-8')
        header += struct.pack('B', len(component_bytes)) + component_bytes
    elif encryption_type == ENCRYPTION_NONSTATIC:
        # For non-static, no additional data needed
        header += struct.pack('B', 0)
    
    return header

def parse_file_header(encrypted_data):
    """Parse file header to extract encryption info"""
    if len(encrypted_data) < 9:
        return None, None, None
    
    magic = encrypted_data[:6]
    if magic != HEADER_MAGIC:
        return None, None, None
    
    version = struct.unpack('B', encrypted_data[6:7])[0]
    encryption_type = struct.unpack('B', encrypted_data[7:8])[0]
    
    header_end = 8
    
    if encryption_type == ENCRYPTION_STATIC:
        # Read stability level
        stability_level = struct.unpack('B', encrypted_data[8:9])[0]
        header_end = 9
        return version, encryption_type, stability_level
    elif encryption_type == ENCRYPTION_COMPONENT:
        # Read component name
        component_length = struct.unpack('B', encrypted_data[8:9])[0]
        header_end = 9 + component_length
        if len(encrypted_data) < header_end:
            return None, None, None
        component_name = encrypted_data[9:header_end].decode('utf-8')
        return version, encryption_type, component_name
    elif encryption_type == ENCRYPTION_NONSTATIC:
        # No additional data
        header_end = 9
        return version, encryption_type, None
    
    return None, None, None

def get_header_size(encrypted_data):
    """Get the size of the header to know where encrypted data starts"""
    if len(encrypted_data) < 9:
        return 0
    
    magic = encrypted_data[:6]
    if magic != HEADER_MAGIC:
        return 0
    
    encryption_type = struct.unpack('B', encrypted_data[7:8])[0]
    
    if encryption_type == ENCRYPTION_STATIC:
        return 9
    elif encryption_type == ENCRYPTION_COMPONENT:
        component_length = struct.unpack('B', encrypted_data[8:9])[0]
        return 9 + component_length
    elif encryption_type == ENCRYPTION_NONSTATIC:
        return 9
    
    return 0

def encrypt_file_static(file_path, hw_type=None, save_key=False, stability_level=1):
    """Encrypt a file using static hardware hash (no random data)"""
    try:
        # Generate hardware hash with stability level (ignoring hw_type for static)
        hardware_hash = generate_hardware_hash(None, stability_level)
        if not hardware_hash:
            return
        
        # Read file data
        with open(file_path, 'rb') as f:
            data = f.read()
        
        # Create Fernet key from hardware hash
        fernet_key = create_fernet_key_from_hash(hardware_hash)
        cipher_suite = Fernet(fernet_key)
        
        # Encrypt data
        encrypted_data = cipher_suite.encrypt(data)
        
        # Create header with encryption type and stability level
        header = create_file_header(ENCRYPTION_STATIC, stability_level)
        
        # Combine header with encrypted data
        final_data = header + encrypted_data
        
        # Save encrypted file
        encrypted_path = file_path + '.encrypted'
        with open(encrypted_path, 'wb') as f:
            f.write(final_data)
        
        print(f"File encrypted successfully (STATIC): {encrypted_path}")
        print(f"Encryption method: Static (hardware-based, Level {stability_level})")
        
        # Display level description
        if stability_level == 1:
            print("Level 1: CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS, NET")
        elif stability_level == 2:
            print("Level 2: CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS")
        elif stability_level == 3:
            print("Level 3: CPU, DISK, BIOS, SYSTEM, BASEBOARD, CHASSIS")
        
        # Save key if requested
        if save_key:
            key_path = file_path + '.key'
            with open(key_path, 'wb') as f:
                f.write(fernet_key)
            print(f"Key saved to: {key_path}")
        else:
            print("No key file saved (use -dec -auto to decrypt)")
        
    except Exception as e:
        print(f"Error encrypting file: {e}")

def encrypt_file_component(file_path, component_name, save_key=False):
    """Encrypt a file using specific hardware component only"""
    try:
        if component_name not in info_sources:
            print(f"Error: Unsupported component: {component_name}")
            print(f"Available components: {', '.join(info_sources.keys())}")
            return
        
        # Generate hardware hash for specific component
        hardware_hash = generate_hardware_hash(component_name)
        if not hardware_hash:
            return
        
        # Read file data
        with open(file_path, 'rb') as f:
            data = f.read()
        
        # Create Fernet key from hardware hash
        fernet_key = create_fernet_key_from_hash(hardware_hash)
        cipher_suite = Fernet(fernet_key)
        
        # Encrypt data
        encrypted_data = cipher_suite.encrypt(data)
        
        # Create header with encryption type and component name
        header = create_file_header(ENCRYPTION_COMPONENT, component_name)
        
        # Combine header with encrypted data
        final_data = header + encrypted_data
        
        # Save encrypted file
        encrypted_path = file_path + '.encrypted'
        with open(encrypted_path, 'wb') as f:
            f.write(final_data)
        
        print(f"File encrypted successfully (COMPONENT): {encrypted_path}")
        print(f"Encryption method: Component-specific ({component_name.upper()})")
        
        # Save key if requested
        if save_key:
            key_path = file_path + '.key'
            with open(key_path, 'wb') as f:
                f.write(fernet_key)
            print(f"Key saved to: {key_path}")
        else:
            print("No key file saved (use -dec -auto to decrypt)")
        
    except Exception as e:
        print(f"Error encrypting file: {e}")

def encrypt_file_nonstatic(file_path, hw_type=None, save_key=True, stability_level=1):
    """Encrypt a file using non-static method (hardware + random data)"""
    try:
        # Generate hardware hash with stability level
        hardware_hash = generate_hardware_hash(hw_type, stability_level)
        if not hardware_hash:
            return
        
        # Read file data
        with open(file_path, 'rb') as f:
            data = f.read()
        
        # Generate random data and combine with hardware hash
        random_data = os.urandom(32).hex()
        combined_key_material = hardware_hash + random_data
        
        # Create Fernet key
        key_material = hashlib.sha256(combined_key_material.encode()).digest()
        fernet_key = base64.urlsafe_b64encode(key_material)
        cipher_suite = Fernet(fernet_key)
        
        # Encrypt data
        encrypted_data = cipher_suite.encrypt(data)
        
        # Create header with encryption type
        header = create_file_header(ENCRYPTION_NONSTATIC, None)
        
        # Combine header with encrypted data
        final_data = header + encrypted_data
        
        # Save encrypted file
        encrypted_path = file_path + '.encrypted'
        with open(encrypted_path, 'wb') as f:
            f.write(final_data)
        
        print(f"File encrypted successfully (NON-STATIC): {encrypted_path}")
        print(f"Encryption method: Non-static (hardware + random, Level {stability_level})")
        
        # Save key (required for non-static)
        if save_key:
            key_path = file_path + '.key'
            with open(key_path, 'wb') as f:
                f.write(fernet_key)
            print(f"Key saved to: {key_path}")
            print("Warning: Keep the key file secure!")
        
    except Exception as e:
        print(f"Error encrypting file: {e}")

def decrypt_file(encrypted_path, key_path):
    """Decrypt a file using saved key file"""
    try:
        with open(encrypted_path, 'rb') as f:
            file_data = f.read()
        
        with open(key_path, 'rb') as f:
            fernet_key = f.read()
        
        # Get header size to extract only encrypted data
        header_size = get_header_size(file_data)
        encrypted_data = file_data[header_size:]
        
        cipher_suite = Fernet(fernet_key)
        decrypted_data = cipher_suite.decrypt(encrypted_data)
        
        decrypted_path = encrypted_path.replace('.encrypted', '')
        with open(decrypted_path, 'wb') as f:
            f.write(decrypted_data)
        
        print(f"File decrypted successfully: {decrypted_path}")
        
    except Exception as e:
        print(f"Error decrypting file: {e}")

def decrypt_file_auto(encrypted_path):
    """Auto-decrypt a file using hardware hash (reads header automatically)"""
    try:
        # Read encrypted file
        with open(encrypted_path, 'rb') as f:
            file_data = f.read()
        
        # Parse header to get encryption info
        version, encryption_type, info = parse_file_header(file_data)
        
        if version is None or encryption_type is None:
            print("Error: Cannot parse file header or file is corrupted")
            return
        
        print(f"Detected encryption type: {encryption_type}")
        
        # Generate hardware hash based on encryption type
        hardware_hash = None
        
        if encryption_type == ENCRYPTION_STATIC:
            stability_level = info
            print(f"Detected stability level: {stability_level}")
            hardware_hash = generate_hardware_hash(None, stability_level)
            
            # Display level description
            if stability_level == 1:
                print("Level 1: CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS, NET")
            elif stability_level == 2:
                print("Level 2: CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS")
            elif stability_level == 3:
                print("Level 3: CPU, DISK, BIOS, SYSTEM, BASEBOARD, CHASSIS")
                
        elif encryption_type == ENCRYPTION_COMPONENT:
            component_name = info
            print(f"Detected component: {component_name.upper()}")
            hardware_hash = generate_hardware_hash(component_name)
            
        elif encryption_type == ENCRYPTION_NONSTATIC:
            print("Error: Non-static encryption detected")
            print("Non-static encrypted files require a key file for decryption")
            print("Use: python HardID.py -dec filename.encrypted")
            return
            
        if not hardware_hash:
            print("Error: Could not generate hardware hash")
            return
        
        # Get header size and extract encrypted data
        header_size = get_header_size(file_data)
        encrypted_data = file_data[header_size:]
        
        # Create Fernet key from hardware hash
        fernet_key = create_fernet_key_from_hash(hardware_hash)
        cipher_suite = Fernet(fernet_key)
        
        # Decrypt data
        decrypted_data = cipher_suite.decrypt(encrypted_data)
        
        # Save decrypted file
        decrypted_path = encrypted_path.replace('.encrypted', '')
        with open(decrypted_path, 'wb') as f:
            f.write(decrypted_data)
        
        print(f"File auto-decrypted successfully: {decrypted_path}")
        
        if encryption_type == ENCRYPTION_STATIC:
            print(f"Decryption method: Auto (hardware-based, Level {stability_level})")
        elif encryption_type == ENCRYPTION_COMPONENT:
            print(f"Decryption method: Auto (component-based, {component_name.upper()})")
        
    except Exception as e:
        print(f"Error auto-decrypting file: {e}")
        print("Note: Make sure you're using the same hardware that was used for encryption")

def show_help():
    """Display help information"""
    print("="*70)
    print("                      HARD ID - Hardware Identity Generator v2.4")
    print("="*70)
    print("\nBasic Commands:")
    print("  --type [name]       → Generate ID from one component")
    print("                        Available: cpu, ram, gpu, disk, smart, bios,")
    print("                                   system, baseboard, chassis, net")
    print("  --all               → Generate all component IDs")
    print("  --combined          → Generate one global ID from everything")
    print("  --dis               → Disable logo animation")
    print("  -qr                 → Generate QR code for the ID")
    print("  -help               → Show this help message")
    
    print("\nStability Levels for Static Encryption:")
    print("  -1  → Level 1 (Default): CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS, NET")
    print("  -2  → Level 2: CPU, GPU, RAM, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS")
    print("  -3  → Level 3: CPU, DISK, BIOS, SYSTEM, BASEBOARD, CHASSIS")
    
    print("\nEncryption Commands:")
    print("  -enc <path>               → Encrypt file (default: non-static method)")
    print("  -enc -static <path>       → Encrypt using static hardware hash")
    print("  -enc -ns <path>           → Encrypt using non-static method")
    print("  -enc -c <component> <path>→ Encrypt using specific component only")
    print("  -key                      → Force save key file (even for static)")
    print("  --type [name] -enc        → Use specific hardware component for encryption")
    
    print("\nDecryption Commands:")
    print("  -dec <path>         → Decrypt using saved key file")
    print("  -dec -auto <path>   → Auto-decrypt using hardware hash (reads header)")
    
    print("\nEncryption Methods:")
    print("  STATIC:     Uses combined hardware hash as key (no key file needed)")
    print("              Encryption level is embedded in file header")
    print("              Can be decrypted with -dec -auto on same hardware")
    print("  COMPONENT:  Uses specific hardware component as key")
    print("              Component name is embedded in file header")
    print("              Can be decrypted with -dec -auto on same hardware")
    print("  NON-STATIC: Uses hardware hash + random data (key file required)")
    print("              More secure but requires key file for decryption")
    
    print("\nExamples:")
    print("  python HardID.py --type cpu -qr")
    print("  python HardID.py --all")
    print("  python HardID.py --combined --dis")
    print("  python HardID.py -enc -static secret.txt -2")
    print("  python HardID.py -enc -c cpu secret.txt")
    print("  python HardID.py -enc -c bios secret.txt -key")
    print("  python HardID.py -dec -auto secret.txt.encrypted")

def main():
    """Main function to handle command line arguments and execute appropriate actions"""
    if "-help" in sys.argv or len(sys.argv) == 1:
        show_help()
        return

    # --- Argument Parsing ---
    args_dict = {
        'type': None, 'all': False, 'combined': False, 'dis': False,
        'qr': False, 'enc': None, 'dec': None, 'static': False,
        'ns': False, 'c': None, 'auto': False, 'key': False,
        'stability_level': 1
    }
    
    # Simple loop to gather flags and find the file path for enc/dec
    flags = []
    path_args = []
    for arg in sys.argv[1:]:
        if arg.startswith('-'):
            flags.append(arg)
        else:
            path_args.append(arg)
            
    # Process flags
    i = 0
    while i < len(flags):
        flag = flags[i]
        if flag == '--type':
            args_dict['type'] = path_args.pop(0) if path_args else None
        elif flag == '--all': args_dict['all'] = True
        elif flag == '--combined': args_dict['combined'] = True
        elif flag == '--dis': args_dict['dis'] = True
        elif flag == '-qr': args_dict['qr'] = True
        elif flag == '-enc': args_dict['enc'] = "placeholder"
        elif flag == '-dec': args_dict['dec'] = "placeholder"
        elif flag == '-static': args_dict['static'] = True
        elif flag == '-ns': args_dict['ns'] = True
        elif flag == '-c':
            args_dict['c'] = path_args.pop(0) if path_args else None
        elif flag == '-auto': args_dict['auto'] = True
        elif flag == '-key': args_dict['key'] = True
        elif flag in ['-1', '-2', '-3']:
            args_dict['stability_level'] = int(flag[1])
        i += 1
        
    # Assign file path if available
    if path_args:
        file_path = path_args[0]
        if args_dict['enc'] == "placeholder": args_dict['enc'] = file_path
        elif args_dict['dec'] == "placeholder": args_dict['dec'] = file_path

    # --- Initial Checks ---
    if not args_dict['dis']:
        show_logo()
    else:
        clear_screen()

    # --- Action Logic ---
    try:
        # Encryption
        if args_dict['enc'] and args_dict['enc'] != "placeholder":
            file_path = args_dict['enc']
            if not os.path.exists(file_path):
                raise FileNotFoundError(f"File not found: {file_path}")

            if args_dict['c']:
                encrypt_file_component(file_path, args_dict['c'], args_dict['key'])
            elif args_dict['static']:
                encrypt_file_static(file_path, None, args_dict['key'], args_dict['stability_level'])
            else: # Default to non-static
                encrypt_file_nonstatic(file_path, args_dict['type'], True, args_dict['stability_level'])

        # Decryption
        elif args_dict['dec'] and args_dict['dec'] != "placeholder":
            file_path = args_dict['dec']
            if not os.path.exists(file_path):
                raise FileNotFoundError(f"File not found: {file_path}")

            if args_dict['auto']:
                decrypt_file_auto(file_path)
            else:
                key_path = file_path.replace('.encrypted', '') + '.key'
                if not os.path.exists(key_path):
                    raise FileNotFoundError(f"Key file not found: {key_path}. Use -dec -auto for static/component decryption.")
                decrypt_file(file_path, key_path)

        # ID Generation
        elif args_dict['type']:
            generate_id(args_dict['type'], args_dict['qr'])
        elif args_dict['all']:
            for component in sorted(info_sources.keys()):
                print("-" * 30)
                generate_id(component, args_dict['qr'])
        elif args_dict['combined']:
            generate_combined_id(args_dict['qr'])
        else:
            print("No valid command specified. Use -help for options.")

    except Exception as e:
        print(f"\nAn unexpected error occurred: {e}")

if __name__ == "__main__":
        
    main()
