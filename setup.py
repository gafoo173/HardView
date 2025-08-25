import sys
import os
import glob
import shutil
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as _build_ext
import pybind11

# =================================================================
# ==      HardView Main Extension (C)                          ==
# =================================================================

hardview_source_files = [
    'HardView/HardView.c',
    'HardView/helpers.c',
    'HardView/bios_info.c',
    'HardView/system_info.c',
    'HardView/baseboard_info.c',
    'HardView/chassis_info.c',
    'HardView/cpu_info.c',
    'HardView/ram_info.c',
    'HardView/disk_info.c',
    'HardView/gpu_info.c',
    'HardView/network_info.c',
    'HardView/performance_monitor.c',
    'HardView/advanced_storage_info.c',
    'HardView/Smart_disk.c'
]

hardview_libraries = []
hardview_extra_compile_args = []

if sys.platform.startswith('win'):
    hardview_source_files.append('HardView/win_helpers.c')
    hardview_libraries.extend(['wbemuuid', 'ole32', 'oleaut32'])
elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):
    hardview_source_files.append('HardView/linux_helpers.c')

# Define the main C extension as a submodule of HardView
hardview_module = Extension(
    'HardView.HardView',
    sources=hardview_source_files,
    define_macros=[('BUILD_PYTHON_MODULE', '1')],
    extra_compile_args=hardview_extra_compile_args,
    libraries=hardview_libraries,
    library_dirs=[],
    include_dirs=['HardView']
)

# =================================================================
# ==      LiveView Extension (C++)                             ==
# =================================================================

liveview_libraries = []
liveview_extra_compile_args = []
liveview_extra_link_args = [] # Added for static linking on MinGW/GCC
liveview_include_dirs = ['HardView/LiveView', pybind11.get_include()]

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
    liveview_libraries.append('sensors')
# Define the new C++ extension as a submodule of HardView
liveview_module = Extension(
    'HardView.LiveView',
    sources=['HardView/LiveView/LiveView.cpp'],
    include_dirs=liveview_include_dirs,
    libraries=liveview_libraries,
    extra_compile_args=liveview_extra_compile_args,
    extra_link_args=liveview_extra_link_args, # Added for static linking
    language='c++'
)

# =================================================================
# ==      Package Configuration                                  ==
# =================================================================

# Base packages and package directories
packages = ['HardView']
package_dir = {'HardView': 'HardView/py'}
package_data = {}

# =================================================================
# ==      Custom Build for Extensions                            ==
# =================================================================

# This custom class overrides the build_ext command.
class build_ext(_build_ext):
    """
    Custom build_ext command to:
    1. Remove platform-specific tags from the compiled extension filename.
    2. Copy necessary DLLs for the LiveView extension on Windows (32-bit and 64-bit).
    """
    def run(self):
        # Run the original build process first
        super().run()

        # NEW: After building, check if we are on Windows and copy appropriate DLLs
        if sys.platform.startswith('win'):
            # Determine the DLL source directory based on architecture
            if sys.maxsize > 2**32:
                # 64-bit Windows
                dll_source_dir = 'HardView/LiveView/64'
            else:
                # 32-bit Windows
                dll_source_dir = 'HardView/LiveView/32'

            dll_source_path = os.path.join(dll_source_dir, '*.dll')

            # The build directory for the package (e.g., build/lib.win-amd64-...)
            build_lib = self.build_lib

            # The destination for the DLLs is the same directory as the LiveView extension.
            liveview_ext_path = self.get_ext_filename('HardView.LiveView') # e.g., 'HardView/LiveView.pyd'
            dest_dir = os.path.join(build_lib, os.path.dirname(liveview_ext_path))

            # Ensure the destination directory exists
            os.makedirs(dest_dir, exist_ok=True)

            # Find and copy each DLL
            for dll_file in glob.glob(dll_source_path):
                print(f"Copying DLL: {dll_file} to {dest_dir}")
                shutil.copy(dll_file, dest_dir)

    def get_ext_filename(self, ext_name):
        # Get the original filename from the superclass to ensure the correct suffix
        original_filename = super().get_ext_filename(ext_name)

        # Extract the file suffix (e.g., .pyd or .so)
        _, ext_suffix = os.path.splitext(original_filename)

        # Split the full extension name (e.g., 'HardView.LiveView')
        ext_path = ext_name.split('.')

        # The desired filename is the last part of the name + suffix (e.g., 'LiveView.pyd')
        filename = ext_path[-1] + ext_suffix

        # The directory path is the initial part of the name (e.g., 'HardView')
        path_parts = ext_path[:-1]

        # Join the path and filename
        return os.path.join(*path_parts, filename)

# =================================================================
# ==                       Setup Function                        ==
# =================================================================

setup(
    name='HardView',
    version='3.1.0b2',
    description='A comprehensive Python library for collecting hardware information and real-time performance monitoring.',
    long_description='''
# HardView 3.1.0 - Advanced Hardware & Real-time Monitoring

A comprehensive Python library for querying low-level hardware information and monitoring system performance in real-time on Windows and Linux systems.

---

##  New Features in 3.1.0

- **Added `LiveView` Extension**  
  A new, high-performance C++ extension for real-time, class-based monitoring of system resources.  
  This is the new recommended way for live monitoring, replacing the older `monitor_*` functions.

---

## 🌡 Temperature Monitoring Support

HardView 3.1.0 now includes **cross-platform temperature monitoring**:

- **Windows**: Implemented via [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor) through a **C++/CLI wrapper**, seamlessly exposed to Python.
- **Linux**: Implemented using `lm-sensors` for accurate thermal readings.

> The `LibreHardwareMonitorLib.dll` required for Windows temperature monitoring is **bundled within the HardView wheel** and is **automatically copied to the Python installation directory** during the library’s initial setup.  
> This means you do **not** need to manually download or place it alongside your script — unless you encounter specific import or loading issues.



The included `LibreHardwareMonitorLib.dll` is the **original, unmodified build** of LibreHardwareMonitor, licensed under **MPL-2.0**, with full source code available in their [official repository](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor).
The included `libsensors` library is the original, unmodified build of lm-sensors, licensed under **LGPL-2.1-or-later**, with full source code available in their [official repository](https://github.com/lm-sensors/lm-sensors).
The included `HidSharp.dll` library is the original, unmodified build of HidSharp, licensed under **Apache License 2.0**, with full source code available in their [official repository](https://github.com/IntergatedCircuits/HidSharp). This project does **not** depend on it directly, but it is a dependency of `LibreHardwareMonitorLib`.

---

## 📦 `LiveView` Extension API

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
    license="MIT", 
    packages=packages,
    package_dir=package_dir,
    # Include non-python files specified in package_data
    package_data=package_data,
    include_package_data=True,
    ext_modules=[hardview_module, liveview_module],
    # Use the custom build command
    cmdclass={'build_ext': build_ext},
    classifiers=[
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
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




