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
    version='3.0.3',
    description='A comprehensive Python library for collecting hardware information and performance monitoring with dual JSON/Python object output support.',
    long_description='''
HardView 3.0.3 - Advanced Hardware Information and Performance Monitoring Library

A comprehensive Python library for querying low-level hardware information and monitoring system performance on Windows and Linux systems with enhanced dual output support.

NEW FEATURES IN 3.0.3:

-Fixed the default value error in the objects functions that was causing them to return JSON when false was not explicitly specified.


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
        'API Guide': 'https://github.com/gafoo173/HardView/What.md',
        'Changelog': 'https://github.com/gafoo173/HardView/CHANGELOG.md',
    },
)
