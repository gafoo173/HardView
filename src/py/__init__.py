from . import HardView
from . import LiveView


for name in dir(HardView):
    if not name.startswith("_"):
        globals()[name] = getattr(HardView, name)


for name in dir(LiveView):
    if not name.startswith("_"):
        globals()[name] = getattr(LiveView, name)
