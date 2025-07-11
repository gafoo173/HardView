import sys
from setuptools import setup, Extension

source_files = [
    'src/HardView.c',
    'src/helpers.c',
    'src/bios_info.c',
    'src/system_info.c',
    'src/baseboard_info.c',
    'src/chassis_info.c',
    'src/cpu_info.c',
    'src/ram_info.c',
    'src/disk_info.c',
    'src/gpu_info.c',
    'src/network_info.c',
    'src/performance_monitor.c',
    'src/advanced_storage_info.c',
    'src/Smart_disk.c'
]

if sys.platform.startswith('win'):
    source_files.append('src/win_helpers.c')
    libraries = ['wbemuuid', 'ole32', 'oleaut32']
    extra_compile_args = []
elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
    source_files.append('src/linux_helpers.c')
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
    include_dirs=['src']
)

setup(
    name='HardView',
    version='3.0.1',
    description='A comprehensive Python library for collecting hardware information and performance monitoring with dual JSON/Python object output support.',
    long_description='''
HardView 3.0.1 - Advanced Hardware Information and Performance Monitoring Library

A comprehensive Python library for querying low-level hardware information and monitoring system performance on Windows and Linux systems with enhanced dual output support.

NEW FEATURES IN 3.0.1:
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

bios_json = HardView.get_bios_info()
cpu_json = HardView.get_cpu_info()

bios_objects = HardView.get_bios_info_objects(0)
cpu_objects = HardView.get_cpu_info_objects(0)

cpu_usage = HardView.get_cpu_usage_objects(0)
ram_usage = HardView.get_ram_usage_objects(0)

cpu_monitoring = HardView.monitor_cpu_usage_duration_objects(10, 500)
```

Cross-platform Support:
- Windows: WMI (Windows Management Instrumentation) queries
- Linux: /proc, /sys filesystem reading and system calls
- Enhanced error handling and memory management
- Platform-specific optimizations

All functions return structured data in both JSON format and native Python objects for maximum flexibility.
    ''',
    long_description_content_type='text/markdown',
    author='gafoo',
    author_email='omarwaled3374@gmail.com',
    url='https://github.com/gafoo173/HardView',
    ext_modules=[hardview_module],
    classifiers=[
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
