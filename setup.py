# setup.py
import sys
from setuptools import setup, Extension

# Source files for the modular structure
source_files = [
    'src/HardView.c',           # Main Python bindings
    'src/helpers.c',           # Shared helper functions
    'src/bios_info.c',         # BIOS information
    'src/system_info.c',       # System information
    'src/baseboard_info.c',    # Baseboard information
    'src/chassis_info.c',      # Chassis information
    'src/cpu_info.c',          # CPU information
    'src/ram_info.c',          # RAM information
    'src/disk_info.c',         # Disk information
    'src/network_info.c',      # Network information
    'src/performance_monitor.c', # Performance monitoring
    'src/advanced_storage_info.c', # Advanced storage info (new)
    'src/Smart_disk.c'
]
# Platform-specific source files
if sys.platform.startswith('win'):
    source_files.append('src/win_helpers.c')  # Windows-specific helpers
    libraries = ['wbemuuid', 'ole32', 'oleaut32']
    extra_compile_args = []
elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
    source_files.append('src/linux_helpers.c')  # Linux-specific helpers
    libraries = []
    extra_compile_args = []
else:
    libraries = []
    extra_compile_args = []

define_macros = [('BUILD_PYTHON_MODULE', '1')]
library_dirs = []

hardview_module = Extension( 
    'HardView', 
    sources=source_files,
    define_macros=define_macros,
    extra_compile_args=extra_compile_args,
    libraries=libraries,
    library_dirs=library_dirs,
    include_dirs=['src']  # Include src directory for header files
)

setup(
    name='HardView',
    version='2.0.0',
    description='A comprehensive Python library for collecting hardware information and performance monitoring on Windows and Linux.',
    long_description='''
HardView 2.0.0 - Advanced Hardware Information and Performance Monitoring Library

A comprehensive Python library for querying low-level hardware information and monitoring system performance on Windows and Linux systems.

Features:
- Hardware Information: BIOS, System, Baseboard, Chassis, CPU, RAM, Disk, Network
- Performance Monitoring: Real-time CPU and RAM usage monitoring
- Cross-platform Support: Windows (WMI) and Linux (/proc, /sys)
- JSON Output: All data returned in structured JSON format
- Modular Architecture: Clean, maintainable codebase
- Duration-based Monitoring: Monitor system performance over specified time periods

Hardware Information Functions:
- get_bios_info(): BIOS manufacturer, version, release date
- get_system_info(): System manufacturer, product name, UUID, serial
- get_baseboard_info(): Motherboard manufacturer, product, serial, version
- get_chassis_info(): Chassis manufacturer, type, serial, version
- get_cpu_info(): CPU name, cores, threads, speed, processor ID
- get_ram_info(): Total memory, individual memory modules
- get_disk_info(): Disk drives, models, sizes, interfaces
- get_network_info(): Network adapters, MAC addresses, IP addresses

Performance Monitoring Functions:
- get_cpu_usage(): Current CPU usage percentage
- get_ram_usage(): Current RAM usage statistics
- get_system_performance(): Combined CPU and RAM usage
- monitor_cpu_usage_duration(): Monitor CPU over time period
- monitor_ram_usage_duration(): Monitor RAM over time period
- monitor_system_performance_duration(): Monitor both over time period

All functions return JSON-formatted data for easy parsing and integration.
    ''',
    author='gafoo',
    author_email='omarwaled3374@gmail.com',
    url='https://github.com/gafoo173/HardView',
    ext_modules=[hardview_module], 
    classifiers=[
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'License :: OSI Approved :: MIT License',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
        'Topic :: System :: Hardware',
        'Topic :: System :: Monitoring',
        'Topic :: System :: Networking',
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'Intended Audience :: System Administrators',
        'Intended Audience :: Information Technology',
    ],
    keywords='hardware system monitoring performance cpu ram disk network bios windows linux wmi',
    python_requires='>=3.7',
    install_requires=[],
    extras_require={
        'dev': [
            'pytest>=6.0',
            'pytest-cov>=2.0',
        ],
    },
    project_urls={
        'Bug Reports': 'https://github.com/gafoo173/HardView/issues',
        'Source': 'https://github.com/gafoo173/HardView',
        'Documentation': 'https://github.com/gafoo173/HardView#readme',
    },
) 
