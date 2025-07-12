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
    'src/gpu_info.c',          # GPU information
    'src/network_info.c',      # Network information
    'src/performance_monitor.c', # Performance monitoring
    'src/advanced_storage_info.c', # Advanced storage info
    'src/Smart_disk.c'         # SMART disk information
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
    version='3.0.0',
    description='A comprehensive Python library for collecting hardware information and performance monitoring with dual JSON/Python object output support.',
    long_description='''
HardView 3.0.0 - Advanced Hardware Information and Performance Monitoring Library

A comprehensive Python library for querying low-level hardware information and monitoring system performance on Windows and Linux systems with enhanced dual output support.

NEW FEATURES IN 3.0.0:
- Dual Output Support: All functions now support both JSON strings and Python objects
- Enhanced GPU Information: Detailed GPU data including driver version, memory size, video processor
- Advanced Network Information: IP address arrays, DNS hostnames, enhanced adapter details
- Improved Linux Support: Better compatibility with Linux systems
- Memory Management: Enhanced memory safety and cleanup
- Performance Monitoring Objects: Real-time monitoring with Python object output

Hardware Information Functions (JSON and Python Objects):
- get_bios_info() / get_bios_info_objects(): BIOS manufacturer, version, release date
- get_system_info() / get_system_info_objects(): System manufacturer, product name, UUID, serial
- get_baseboard_info() / get_baseboard_info_objects(): Motherboard manufacturer, product, serial, version
- get_chassis_info() / get_chassis_info_objects(): Chassis manufacturer, type, serial, version
- get_cpu_info() / get_cpu_info_objects(): CPU name, cores, threads, speed, processor ID
- get_ram_info() / get_ram_info_objects(): Total memory, individual memory modules
- get_disk_info() / get_disk_info_objects(): Disk drives, models, sizes, interfaces
- get_gpu_info() / get_gpu_info_objects(): GPU details, driver version, memory size, video processor
- get_network_info() / get_network_info_objects(): Network adapters, MAC addresses, IP addresses, DNS
- get_smart_info() / get_smart_info_objects(): Advanced disk information (SMART data)
- get_partitions_info() / get_partitions_info_objects(): Advanced storage and partition information

Performance Monitoring Functions (JSON and Python Objects):
- get_cpu_usage() / get_cpu_usage_objects(): Current CPU usage percentage
- get_ram_usage() / get_ram_usage_objects(): Current RAM usage statistics
- get_system_performance() / get_system_performance_objects(): Combined CPU and RAM usage
- monitor_cpu_usage_duration() / monitor_cpu_usage_duration_objects(): Monitor CPU over time period
- monitor_ram_usage_duration() / monitor_ram_usage_duration_objects(): Monitor RAM over time period
- monitor_system_performance_duration() / monitor_system_performance_duration_objects(): Monitor both over time period

Usage Examples:
```python
import HardView

# JSON output (default)
bios_json = HardView.get_bios_info()
cpu_json = HardView.get_cpu_info()

# Python objects output
bios_objects = HardView.get_bios_info_objects(0)  # 0 for Python objects
cpu_objects = HardView.get_cpu_info_objects(0)

# Performance monitoring with objects
cpu_usage = HardView.get_cpu_usage_objects(0)
ram_usage = HardView.get_ram_usage_objects(0)

# Duration monitoring with objects
cpu_monitoring = HardView.monitor_cpu_usage_duration_objects(10, 500)  # 10 seconds, 500ms intervals
```

Cross-platform Support:
- Windows: WMI (Windows Management Instrumentation) queries
- Linux: /proc, /sys filesystem reading and system calls
- Enhanced error handling and memory management
- Platform-specific optimizations

All functions return structured data in both JSON format and native Python objects for maximum flexibility.
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
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Intended Audience :: System Administrators',
        'Intended Audience :: Information Technology',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Topic :: System :: Systems Administration',
    ],
    keywords='hardware system monitoring performance cpu ram disk network gpu bios windows linux wmi python-objects json dual-output',
    python_requires='>=3.7',
    install_requires=[],
    extras_require={
        'dev': [
            'pytest>=6.0',
            'pytest-cov>=2.0',
        ],
        'docs': [
            'sphinx>=4.0',
            'sphinx-rtd-theme>=1.0',
        ],
    },
    project_urls={
        'Bug Reports': 'https://github.com/gafoo173/HardView/issues',
        'Source': 'https://github.com/gafoo173/HardView',
        'Documentation': 'https://github.com/gafoo173/HardView#readme',
        'Changelog': 'https://github.com/gafoo173/HardView/docs/CHANGELOG.md',
    },
) 