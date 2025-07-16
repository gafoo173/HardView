import sys
from setuptools import setup, Extension
import pybind11

# =================================================================
# ==                  HardView Main Extension (C)                ==
# =================================================================

hardview_source_files = [
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

hardview_libraries = []
hardview_extra_compile_args = []

if sys.platform.startswith('win'):
    hardview_source_files.append('src/win_helpers.c')
    hardview_libraries.extend(['wbemuuid', 'ole32', 'oleaut32'])
elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
    hardview_source_files.append('src/linux_helpers.c')

# Define the main C extension as a submodule of HardView
hardview_module = Extension(
    'HardView.HardView',
    sources=hardview_source_files,
    define_macros=[('BUILD_PYTHON_MODULE', '1')],
    extra_compile_args=hardview_extra_compile_args,
    libraries=hardview_libraries,
    library_dirs=[],
    include_dirs=['src']
)

# =================================================================
# ==               LiveView Extension (C++)                      ==
# =================================================================

liveview_libraries = []
liveview_extra_compile_args = []
liveview_extra_link_args = [] # Added for static linking on MinGW/GCC
liveview_include_dirs = ['src/LiveView', pybind11.get_include()]

if sys.platform.startswith('win'):
    liveview_libraries.extend(['pdh', 'ntdll'])
    # Add /MT for static linking with MSVC C++ runtime
    liveview_extra_compile_args.extend(['/std:c++17', '/MT'])
    # Add flags for static linking with MinGW/GCC C++ runtime
    liveview_extra_link_args.extend(['-static-libgcc', '-static-libstdc++'])
elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
    liveview_extra_compile_args.append('-std=c++17')
    # Add flag for static linking with GCC C++ runtime
    liveview_extra_link_args.append('-static-libstdc++')

# Define the new C++ extension as a submodule of HardView
liveview_module = Extension(
    'HardView.LiveView',
    sources=['src/LiveView/LiveView.cpp'],
    include_dirs=liveview_include_dirs,
    libraries=liveview_libraries,
    extra_compile_args=liveview_extra_compile_args,
    extra_link_args=liveview_extra_link_args, # Added for static linking
    language='c++'
)

# =================================================================
# ==                       Setup Function                        ==
# =================================================================

setup(
    name='HardView',
    version='3.1.0',
    description='A comprehensive Python library for collecting hardware information and real-time performance monitoring.',
    long_description='''
# HardView 3.1.0 - Advanced Hardware & Real-time Monitoring

A comprehensive Python library for querying low-level hardware information and monitoring system performance in real-time on Windows and Linux systems.

## NEW FEATURES IN 3.1.0
- **Added `LiveView` Extension**: A new, high-performance C++ extension for real-time, class-based monitoring of system resources. This is the new recommended way for live monitoring, replacing the older `monitor_*` functions.

---

## `LiveView` Extension API

The `LiveView` extension provides a simple, object-oriented interface for live resource monitoring.

### **Usage Example:**
```python
from HardView import LiveView

# Create a CPU monitoring object
cpu_monitor = LiveView.CPU()
cpu_usage_percent = cpu_monitor.get_usage(interval_ms=1000)
print(f"CPU Usage: {cpu_usage_percent:.2f}%")

# Create a RAM monitoring object
ram_monitor = LiveView.RAM()
ram_usage_percent = ram_monitor.get_usage()
print(f"RAM Usage: {ram_usage_percent:.2f}%")
```

---

### **Available Classes and Methods:**

#### `LiveView.CPU()`
- **Description**: Creates a monitor for CPU usage.
- **Methods**:
    - `get_usage(interval_ms)`: Returns the average CPU usage over a specified interval in milliseconds.
- **Platform Support**: `[Cross-platform: Windows, Linux]`

---

#### `LiveView.RAM()`
- **Description**: Creates a monitor for RAM usage.
- **Methods**:
    - `get_usage()`: Returns the current RAM usage percentage.
- **Platform Support**: `[Cross-platform: Windows, Linux]`

---

#### `LiveView.Disk()`
- **Description**: Creates a monitor for physical disk activity (busy time).
- **Methods**:
    - `get_usage(interval)`: Returns the disk busy percentage over a specified interval in milliseconds.
- **Platform Support**: `[Windows-only]`

---

#### `LiveView.GPU()`
- **Description**: Creates a monitor for GPU engine usage.
- **Methods**:
    - `get_usage(interval_ms)`: Returns the GPU usage percentage over a specified interval. The most suitable engine (e.g., 3D, Total) is selected automatically.
- **Platform Support**: `[Windows-only]`

    ''',
    long_description_content_type='text/markdown',
    author='gafoo',
    author_email='omarwaled3374@gmail.com',
    url='https://github.com/gafoo173/HardView',
    ext_modules=[hardview_module, liveview_module],
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
    ],
    keywords='hardware system monitoring performance cpu ram disk network gpu bios windows linux wmi real-time live',
    python_requires='>=3.7',
    install_requires=[
        'pybind11>=2.6'
    ],
    extras_require={
        'dev': [
            'pytest>=6.0',
        ],
    },
    project_urls={
        'Bug Reports': 'https://github.com/gafoo173/HardView/issues',
        'Source': 'https://github.com/gafoo173/HardView',
    },
)
