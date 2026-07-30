"""
Type stubs for the LiveView pybind11 extension module (HardView project).

Cross-platform system monitoring: CPU, RAM, Disk, Network, GPU usage and
hardware temperature sensors, for Windows and Linux.

Platform notes:
- PyLiveGpu, PyTempCpu, PyTempGpu, PyTempOther, PySensor, PyManageTemp,
  and PyLiveCPU.cpu_snapshot are Windows-only (compiled under _WIN32).
- PyLinuxSensor is Linux-only (compiled under __linux__).
- PyLiveCPU, PyLiveRam, PyLiveDisk, PyLiveNetwork are cross-platform,
  though PyLiveDisk mode=0 (% usage) is Windows-only; Linux only
  supports mode=1 (R/W speed).

Only the snake_case method names (the Pythonic bindings) are declared
here; their PascalCase/camelCase C++-style aliases are intentionally
omitted from this stub.
"""

from typing import List, Tuple, Union, Dict

# ------------------------------------------------------------------
# PyLiveCPU
# ------------------------------------------------------------------
class PyLiveCPU:
    """Provides CPU monitoring functionalities."""

    def __init__(self) -> None: ...

    def get_usage(self, interval_ms: int) -> float:
        """Returns total CPU usage percentage (0-100), sampled over interval_ms."""
        ...

    def cpuid(self) -> List[Tuple[str, str]]:
        """Returns CPU information using the CPUID instruction, as a list of
        (name, value) pairs.
        """
        ...

    def cpu_id(self) -> List[Tuple[str, str]]:
        """Alias for cpuid()."""
        ...

    def cpu_snapshot(
        self,
        core: int,
        coreNumbers: bool = False,
        Kernel: bool = True,
        User: bool = True,
        Idle: bool = True,
        PureKernalTime: bool = False,
    ) -> Union[int, Dict[str, float]]:
        """(Windows-only) CPU time-counter snapshot for a given core.
        If coreNumbers is True, returns the total number of cores (int)
        instead. Otherwise returns a dict that may contain
        'raw_kernel_time', 'pure_kernel_time', 'user_time', 'idle_time'
        depending on which flags are enabled.
        """
        ...


# ------------------------------------------------------------------
# PyLiveRam
# ------------------------------------------------------------------
class PyLiveRam:
    """Provides RAM monitoring functionalities."""

    def __init__(self) -> None: ...

    def get_usage(self, Raw: bool = False) -> Union[float, List[float]]:
        """Returns total RAM usage percentage (float), or if Raw is True,
        [used_bytes, total_bytes] as a list of floats.
        """
        ...


# ------------------------------------------------------------------
# PyLiveDisk
# ------------------------------------------------------------------
class PyLiveDisk:
    """Provides disk I/O monitoring functionalities."""

    def __init__(self, mode: int) -> None:
        """mode=0 for '% Disk Time' usage (Windows-only),
        mode=1 for R/W speed in MB/s (Windows and Linux).
        """
        ...

    def get_usage(
        self, interval: int = 1000
    ) -> Union[float, List[Tuple[str, float]]]:
        """Returns disk usage info based on the mode set at construction:
        mode 0 -> percentage as a float (or -0.1 on read failure);
        mode 1 -> [("Read MB/s", value), ("Write MB/s", value)].
        """
        ...

    def high_disk_usage(self, threshold_mbps: float = 80.0) -> bool:
        """(Mode 1 only) True if read or write speed exceeds threshold_mbps."""
        ...


# ------------------------------------------------------------------
# PyLiveNetwork
# ------------------------------------------------------------------
class PyLiveNetwork:
    """Provides network I/O monitoring functionalities."""

    def __init__(self) -> None: ...

    def get_usage(
        self, interval: int = 1000, mode: int = 0
    ) -> Union[float, List[Tuple[str, float]]]:
        """mode=0: total network throughput in MB/s (float).
        mode=1: per-interface throughput as [(interface_name, MB/s), ...].
        """
        ...

    def get_high_card(self) -> str:
        """Returns the name of the network interface with the highest usage,
        or 'N/A' if none available.
        """
        ...


# ------------------------------------------------------------------
# PyLiveGpu (Windows-only)
# ------------------------------------------------------------------
class PyLiveGpu:
    """(Windows-only) Provides GPU monitoring functionalities."""

    def __init__(self) -> None: ...

    def get_usage(self, interval_ms: int = 1000) -> float:
        """Total GPU usage percentage, summed across engines
        (can exceed 100 if multiple engines are active).
        """
        ...

    def get_average_usage(self, interval_ms: int = 1000) -> float:
        """Average GPU usage percentage across all engines."""
        ...

    def get_max_usage(self, interval_ms: int = 1000) -> float:
        """Maximum GPU usage percentage among all engines."""
        ...

    def get_counter_count(self) -> int:
        """Number of GPU performance counters being monitored."""
        ...


# ------------------------------------------------------------------
# PyTempCpu (Windows-only)
# ------------------------------------------------------------------
class PyTempCpu:
    """(Windows-only) Monitors CPU temperature and fan speed."""

    def __init__(self, init: bool = True) -> None: ...

    def get_temp(self) -> float:
        """Current CPU temperature in Celsius (-1 on error)."""
        ...

    def get_max_temp(self) -> float:
        """Max CPU core temperature."""
        ...

    def get_avg_temp(self) -> float:
        """Average CPU core temperature."""
        ...

    def get_fan_rpm(self) -> float:
        """CPU fan RPM."""
        ...

    def update(self) -> None:
        """Refreshes the underlying hardware monitor, then re-reads all values."""
        ...

    def re_get(self) -> None:
        """Re-reads all values without refreshing the underlying monitor."""
        ...


# ------------------------------------------------------------------
# PyTempGpu (Windows-only)
# ------------------------------------------------------------------
class PyTempGpu:
    """(Windows-only) Monitors GPU temperature and fan speed."""

    def __init__(self, init: bool = True) -> None: ...

    def get_temp(self) -> float:
        """Current GPU temperature in Celsius."""
        ...

    def get_fan_rpm(self) -> float:
        """GPU fan RPM."""
        ...

    def update(self) -> None:
        """Refreshes the underlying hardware monitor, then re-reads all values."""
        ...

    def re_get(self) -> None:
        """Re-reads all values without refreshing the underlying monitor."""
        ...


# ------------------------------------------------------------------
# PyTempOther (Windows-only)
# ------------------------------------------------------------------
class PyTempOther:
    """(Windows-only) Motherboard and storage temperature monitoring."""

    def __init__(self, init: bool = True) -> None: ...

    def get_mb_temp(self) -> float:
        """Motherboard temperature."""
        ...

    def get_storage_temp(self) -> float:
        """Storage device temperature."""
        ...

    def update(self) -> None:
        """Refreshes the underlying hardware monitor, then re-reads all values."""
        ...

    def re_get(self) -> None:
        """Re-reads all values without refreshing the underlying monitor."""
        ...


# ------------------------------------------------------------------
# PySensor (Windows-only)
# ------------------------------------------------------------------
class PySensor:
    """(Windows-only) Generic access to all sensors/fans exposed by
    HardwareWrapper.dll.
    """

    def __init__(self, init: bool = True) -> None: ...

    def get_data(self, init: bool = False) -> None:
        """Fetches and parses sensor + fan data from hardware. If init is
        True, (re)initializes the underlying hardware temperature monitor first.
        """
        ...

    def get_value_by_name(self, name: str) -> float:
        """Gets a sensor value by exact name. Raises if the sensor is not found."""
        ...

    def get_all_sensors(self) -> List[str]:
        """List of all known sensor names."""
        ...

    def get_all_fan_rpms(self) -> List[Tuple[str, float]]:
        """Deprecated since v4.0.0. Always returns an empty list, kept for
        backward compatibility.
        """
        ...

    def get_sensors(self) -> Dict[str, float]:
        """Dict mapping each sensor name to its current value."""
        ...

    def update(self) -> None:
        """Refreshes the underlying hardware monitor, then re-fetches sensor data."""
        ...

    def re_get(self) -> None:
        """Re-fetches sensor data without refreshing the underlying monitor."""
        ...


# ------------------------------------------------------------------
# PyManageTemp (Windows-only)
# ------------------------------------------------------------------
class PyManageTemp:
    """(Windows-only) Low-level lifecycle control of the hardware temperature
    monitor (HardwareWrapper.dll), for advanced/manual usage.
    """

    def __init__(self) -> None: ...

    def init(self) -> None:
        """Initializes the hardware temperature monitor."""
        ...

    def close(self) -> None:
        """Shuts down the hardware temperature monitor."""
        ...

    def update(self) -> None:
        """Updates all hardware temperature/fan data."""
        ...

    def specific_update(self, id: int) -> None:
        """Updates only the sensor group identified by id (valid range 1-11;
        out-of-range ids are silently ignored).
        """
        ...

    def multi_specific_update(self, ids: List[int]) -> None:
        """Updates several sensor groups by id (see specific_update)."""
        ...

    def get_hardware_id_by_name(self, name: str) -> int:
        """Resolves a hardware/sensor group name to its numeric id."""
        ...


# ------------------------------------------------------------------
# PyLinuxSensor (Linux-only)
# ------------------------------------------------------------------
class PyLinuxSensor:
    """(Linux-only) Reads sensor temperatures via libsensors."""

    def __init__(self) -> None: ...

    def get_cpu_temp(self) -> float:
        """Cached CPU package temperature."""
        ...

    def get_chipset_temp(self) -> float:
        """Cached chipset (PCH) temperature."""
        ...

    def get_motherboard_temp(self) -> float:
        """Cached motherboard temperature."""
        ...

    def get_vrm_temp(self) -> float:
        """Cached VRM/memory temperature."""
        ...

    def get_drive_temp(self) -> float:
        """Cached storage device temperature."""
        ...

    def get_all_sensor_names(self) -> List[str]:
        """All sensor feature names detected via libsensors."""
        ...

    def find_sensor_name(self, name: str) -> List[Tuple[str, int]]:
        """Finds all (name, index) occurrences of an exact sensor name among
        the cached sensor names.
        """
        ...

    def get_sensor_temp(self, name: str, Match: bool) -> float:
        """Live temperature reading for a sensor name. If Match is True,
        requires an exact label match; otherwise does substring matching.
        Returns -1 if name is empty.
        """
        ...

    def get_sensors_with_temp(self) -> List[Tuple[str, float]]:
        """Live (sensor_name, temperature) pairs for every cached sensor name."""
        ...

    def update(self, names: bool = False) -> None:
        """Refreshes cached temperature readings. If names is True, also
        re-scans the list of available sensor names.
        """
        ...
