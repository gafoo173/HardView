import os
import sys
import hashlib
from . import HardView
from . import LiveView

if os.name == "nt":
    from . import smbios

def _add_lib_dir_to_path():
    if os.name != "nt":
        return

    current_dir = os.path.dirname(os.path.abspath(__file__))
    if current_dir not in os.environ.get("PATH", ""):
        os.environ["PATH"] = current_dir + os.pathsep + os.environ.get("PATH", "")

_add_lib_dir_to_path()

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
