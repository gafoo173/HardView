import os
import shutil
import sys
import hashlib
from . import HardView
from . import LiveView


if os.name == "nt":
    from . import smbios

def _file_hash(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.digest()

def _copy_dlls_to_python_dir():
    if os.name != "nt":
        return

    python_dir = sys.exec_prefix
    dll_files = ["HardwareWrapper.dll", "LibreHardwareMonitorLib.dll", "HidSharp.dll"]
    current_dir = os.path.dirname(os.path.abspath(__file__))

    for dll in dll_files:
        src_path = os.path.join(current_dir, dll)
        dest_path = os.path.join(python_dir, dll)

        if os.path.exists(src_path):
            copy_file = True

            if os.path.exists(dest_path):
                try:
                    if _file_hash(src_path) == _file_hash(dest_path):
                        copy_file = False
                except Exception:
                    pass

            if copy_file:
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

if os.name == "nt":
    for name in dir(smbios):
        if not name.startswith("_"):
            globals()[name] = getattr(smbios, name)
