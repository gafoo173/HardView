# setup.py
import sys
from setuptools import setup, Extension

source_files = ['HardView.c']
extra_compile_args = []
define_macros = [('BUILD_PYTHON_MODULE', '1')]
libraries = []
library_dirs = []

if sys.platform.startswith('win'):

    libraries = ['wbemuuid', 'ole32', 'oleaut32']


elif sys.platform.startswith('linux') or sys.platform.startswith('darwin'):

    pass

hardview_module = Extension( 
    'HardView', 
    sources=source_files,
    define_macros=define_macros,
    extra_compile_args=extra_compile_args,
    libraries=libraries,
    library_dirs=library_dirs,
    # include_dirs=[]  
)

setup(
    name='HardView',
    version='0.1.0',
    description='A Python library for collecting hardware information (BIOS, System, CPU, RAM, Disk, Network).',
    long_description='A comprehensive Python library for querying low-level hardware information on Windows and Linux, including BIOS, system, CPU, RAM, disk drives, and network adapters. Data is returned in JSON format.',
    author='gafoo',
    author_email='omarwaled3374@gmail.com',
    url='https://github.com/gafoo173/HardView',
    ext_modules=[hardview_module], 
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
        'Topic :: System :: Hardware',
        'Development Status :: 3 - Alpha',
    ],
    py_modules=['hardview'], 
)
