import os
import shutil
import sys
from . import HardView
from . import LiveView

def _copy_dlls_to_python_dir():
    if os.name != "nt":
        return

    python_dir = sys.exec_prefix
    dll_files = ["HardwareWrapper.dll", "LibreHardwareMonitorLib.dll"]
    current_dir = os.path.dirname(os.path.abspath(__file__))

    for dll in dll_files:
        dest_path = os.path.join(python_dir, dll)
        if not os.path.exists(dest_path):
            src_path = os.path.join(current_dir, dll)
            if os.path.exists(src_path):
                try:
                    shutil.copy2(src_path, dest_path)
                except Exception:
                    pass

_copy_dlls_to_python_dir()

for name in dir(HardView):
    if not name.startswith("_"):
        globals()[name] = getattr(HardView, name)

for name in dir(LiveView):
    if not name.startswith("_"):
        globals()[name] = getattr(LiveView, name)
