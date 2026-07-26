"""
liveview_helper.py

A thin Python wrapper layer built on top of HardView.LiveView.

It adds:
  - PyTempDisk        : a PyTempOther subclass that only refreshes Storage data
  - PyTempMotherboard : a PyTempOther subclass that only refreshes Motherboard data
  - PyTempCPU         : a PyTempCpu subclass whose update() uses SpecificUpdate(CPU)
  - PyTempGPU         : a PyTempGpu subclass whose update() uses SpecificUpdate(GPU)
  - update_hardware() : a generic updater that inspects the object's type,
                         resolves the correct hardware component id, calls
                         PyManageTemp().specific_update(id), and then re_get()
                         on the object to pull the refreshed cached values.
  - HardwareType      : IntEnum mirroring COMPONENT_IDS (Motherboard, CPU, ...)
  - SensorType         : Enum for the sensor "type" segment in a raw sensor name
                         (Data, Load, Power, Clock, Temperature, Voltage, Throughput)
  - ParsedSensor       : simple container returned by parse_sensor()
  - parse_sensor()     : parses a raw sensor name string like
                         "HS-SSD-E100 256G - Throughput - Write Rate" into
                         (hardware_type, sensor_type, name)
"""

from enum import IntEnum, Enum

from HardView import LiveView

# --- Component IDs
COMPONENT_IDS = {
    "Motherboard": 1,
    "SuperIO": 2,
    "CPU": 3,
    "Memory": 4,
    "GPU": 5,
    "Storage": 6,
    "Network": 7,
    "Cooler": 8,
    "EmbeddedController": 9,
    "Psu": 10,
    "Battery": 11
}

# HardwareType enum, built directly from COMPONENT_IDS so both stay in sync.
HardwareType = IntEnum("HardwareType", COMPONENT_IDS)


class SensorType(Enum):
    """
    The sensor "type" segment that appears in a raw LiveView sensor name,
    e.g. the "Throughput" in "HS-SSD-E100 256G - Throughput - Write Rate".
    """
    Voltage = "Voltage"
    Current = "Current"
    Power = "Power"
    Clock = "Clock"
    Temperature = "Temperature"
    Load = "Load"
    Frequency = "Frequency"
    Fan = "Fan"
    Flow = "Flow"
    Control = "Control"
    Level = "Level"
    Factor = "Factor"
    Data = "Data"
    SmallData = "SmallData"
    Throughput = "Throughput"
    TimeSpan = "TimeSpan"
    Timing = "Timing"
    Energy = "Energy"
    Noise = "Noise"
    Conductivity = "Conductivity"
    Humidity = "Humidity"
    

# Case-insensitive lookup table, built once: "throughput" -> SensorType.Throughput
_SENSOR_TYPE_BY_LOWER = {member.value.lower(): member for member in SensorType}


class PyTempDisk(LiveView.PyTempOther):
    """
    Disk-only view built on top of PyTempOther.

    PyTempOther internally caches both mb_temp and storage_temp, but this
    subclass represents Storage only: its own update() must refresh just
    the Storage branch of the hardware monitor, then re_get() to pull the
    refreshed cached value(s) from the underlying PyTempOther object.
    """

    def __init__(self, init: bool = True):
        super().__init__(init)

    def update(self):
        manager = LiveView.PyManageTemp()
        manager.specific_update(COMPONENT_IDS["Storage"])
        self.re_get()

    def get_disk_temp(self):
        return self.get_storage_temp()


class PyTempMotherboard(LiveView.PyTempOther):
    """
    Motherboard-only view built on top of PyTempOther.

    Same idea as PyTempDisk, but only refreshes the Motherboard branch.
    """

    def __init__(self, init: bool = True):
        super().__init__(init)

    def update(self):
        manager = LiveView.PyManageTemp()
        manager.specific_update(COMPONENT_IDS["Motherboard"])
        self.re_get()

    def get_motherboard_temp(self):
        return self.get_mb_temp()


class PyTempCPU(LiveView.PyTempCpu):
    """
    Thin wrapper around PyTempCpu.

    Inherits everything from PyTempCpu (get_temp, get_fan_rpm, re_get, ...),
    but overrides update() so it only refreshes the CPU sensors specifically
    (via SpecificUpdate) instead of triggering a full/generic update.
    """

    def __init__(self, init: bool = True):
        super().__init__(init)

    def update(self):
        manager = LiveView.PyManageTemp()
        manager.specific_update(COMPONENT_IDS["CPU"])
        self.re_get()


class PyTempGPU(LiveView.PyTempGpu):
    """
    Thin wrapper around PyTempGpu.

    Inherits everything from PyTempGpu, but overrides update() so it only
    refreshes the GPU sensors specifically (via SpecificUpdate) instead of
    triggering a full/generic update.
    """

    def __init__(self, init: bool = True):
        super().__init__(init)

    def update(self):
        manager = LiveView.PyManageTemp()
        manager.specific_update(COMPONENT_IDS["GPU"])
        self.re_get()


def update_hardware(temp_obj):
    """
    Generic updater for any LiveView temperature object.

    Behavior:
      - PySensor:
          Calls its own normal update() (it handles everything internally
          via UpdateHardwareMonitorTemp + GetData, no id needed).

      - PyTempCPU (subclass of PyTempCpu):
          SpecificUpdate(CPU id) then re_get().

      - PyTempGPU (subclass of PyTempGpu):
          SpecificUpdate(GPU id) then re_get().

      - PyTempCpu (plain, not PyTempCPU):
          SpecificUpdate(CPU id) then re_get().

      - PyTempGpu (plain, not PyTempGPU):
          SpecificUpdate(GPU id) then re_get().

      - PyTempDisk (subclass of PyTempOther):
          SpecificUpdate(Storage id) only, then re_get(). Only the Storage
          side is refreshed, not the Motherboard side.

      - PyTempMotherboard (subclass of PyTempOther):
          SpecificUpdate(Motherboard id) only, then re_get(). Only the
          Motherboard side is refreshed, not the Storage side.

      - Plain PyTempOther (not one of the two subclasses above):
          Since this object caches BOTH mb_temp and storage_temp,
          MultiSpecificUpdate([Motherboard id, Storage id]) is used to
          refresh both branches, then re_get().

    Note: the PyTempDisk / PyTempMotherboard / PyTempCPU / PyTempGPU checks
    MUST come before the plain PyTempOther / PyTempCpu / PyTempGpu checks,
    since these subclasses are also instances of their parent class
    (isinstance would otherwise match the wrong branch first).
    """
    manager = LiveView.PyManageTemp()

    if isinstance(temp_obj, LiveView.PySensor):
        temp_obj.update()
        return temp_obj

    if isinstance(temp_obj, LiveView.PyTempCpu):
        manager.specific_update(COMPONENT_IDS["CPU"])
        temp_obj.re_get()
        return temp_obj

    if isinstance(temp_obj, LiveView.PyTempGpu):
        manager.specific_update(COMPONENT_IDS["GPU"])
        temp_obj.re_get()
        return temp_obj

    if isinstance(temp_obj, PyTempDisk):
        manager.specific_update(COMPONENT_IDS["Storage"])
        temp_obj.re_get()
        return temp_obj

    if isinstance(temp_obj, PyTempMotherboard):
        manager.specific_update(COMPONENT_IDS["Motherboard"])
        temp_obj.re_get()
        return temp_obj

    if isinstance(temp_obj, LiveView.PyTempOther):
        manager.multi_specific_update(
            [COMPONENT_IDS["Motherboard"], COMPONENT_IDS["Storage"]]
        )
        temp_obj.re_get()
        return temp_obj

    raise TypeError(
        f"update_hardware: unsupported LiveView temperature object type: {type(temp_obj)}"
    )


class ParsedSensor:
    """
    Result of parse_sensor(): the decoded pieces of a raw LiveView sensor name.
    """

    def __init__(self, hardware_type: "HardwareType", sensor_type: "SensorType", name: str):
        self.hardware_type = hardware_type
        self.sensor_type = sensor_type
        self.name = name

    def __repr__(self):
        return (
            f"ParsedSensor(hardware_type={self.hardware_type!r}, "
            f"sensor_type={self.sensor_type!r}, name={self.name!r})"
        )


def parse_sensor(sensor_name: str) -> ParsedSensor:
    """
    Parse a raw LiveView sensor name of the form:
        "<Hardware> - <Type> - <Name>"

    Example:
        "HS-SSD-E100 256G - Throughput - Write Rate"
        -> hardware_type = HardwareType.Storage
        -> sensor_type   = SensorType.Throughput
        -> name          = "Write Rate"

    The hardware segment (first part, before the first " - ") is resolved
    to a component id using PyManageTemp.get_hardware_id_by_name(), then
    mapped to the HardwareType enum. A negative id returned by
    get_hardware_id_by_name means the hardware name could not be resolved,
    and this function raises a ValueError in that case.

    Parameters
    ----------
    sensor_name : str
        The raw sensor name as returned by LiveView (e.g. from PySensor).

    Returns
    -------
    ParsedSensor
        Object exposing `.hardware_type` (HardwareType), `.sensor_type`
        (SensorType), and `.name` (str, the sensor's own name).
    """
    # maxsplit=2 keeps the remainder of the string intact even if the
    # sensor's own name happens to contain " - " itself.
    parts = sensor_name.split(" - ", 2)
    if len(parts) < 3:
        raise ValueError(
            f"parse_sensor: unexpected sensor name format: {sensor_name!r}"
        )

    hardware_str = parts[0].strip()
    type_str = parts[1].strip()
    name_str = parts[2].strip()

    manager = LiveView.PyManageTemp()
    hardware_id = manager.get_hardware_id_by_name(hardware_str)
    if hardware_id < 0:
        raise ValueError(
            f"parse_sensor: could not resolve hardware id for {hardware_str!r} "
            f"(get_hardware_id_by_name returned {hardware_id})"
        )

    try:
        hardware_type = HardwareType(hardware_id)
    except ValueError:
        raise ValueError(
            f"parse_sensor: unknown hardware id {hardware_id} for {hardware_str!r}"
        )

    sensor_type = _SENSOR_TYPE_BY_LOWER.get(type_str.lower())
    if sensor_type is None:
        raise ValueError(f"parse_sensor: unknown sensor type {type_str!r}")

    return ParsedSensor(hardware_type, sensor_type, name_str)


if __name__ == "__main__":
    cpu = PyTempCPU()
    update_hardware(cpu)
    print("CPU:", cpu.get_temp(), cpu.get_fan_rpm())

    gpu = PyTempGPU()
    update_hardware(gpu)
    print("GPU:", gpu.get_temp())

    disk = PyTempDisk()
    update_hardware(disk)
    print("Disk:", disk.get_disk_temp())

    mb = PyTempMotherboard()
    update_hardware(mb)
    print("Motherboard:", mb.get_motherboard_temp())

    other = LiveView.PyTempOther()
    update_hardware(other)
    print("Other (MB+Storage):", other.get_mb_temp(), other.get_Storage_temp())

    sensor = LiveView.PySensor()
    update_hardware(sensor)
    print("Sensors:", sensor.getAllSensors())

    parsed = parse_sensor("HS-SSD-E100 256G - Throughput - Write Rate")
    print("Parsed:", parsed)