"""
Type stubs for the SMART pybind11 extension module (HardView project).

Python bindings for SmartReader / SMART.hpp - low-level ATA/NVMe SMART
attribute reading, error log parsing and SSD vendor/controller detection.
"""

from typing import List, Tuple, Optional

# ------------------------------------------------------------------
# SmartAttribute
# ------------------------------------------------------------------
class SmartAttribute:
    """A single SMART attribute entry (id/current/worst/raw value)."""

    id: int
    """Attribute ID"""

    flags: int
    """Attribute flags"""

    current: int
    """Current value"""

    worst: int
    """Worst value"""

    def __init__(self) -> None: ...

    @property
    def raw_value(self) -> int:
        """Get raw value as 64-bit integer"""
        ...

    @property
    def name(self) -> str:
        """Get human-readable attribute name"""
        ...

    def __repr__(self) -> str: ...


# SmartThreshold (per-attribute failure thresholds, from SmartReader.get_smart_thresholds)
class SmartThreshold:
    """Per-attribute SMART failure threshold."""

    id: int
    """Attribute ID this threshold applies to"""

    threshold: int
    """Failure threshold value"""

    def __init__(self) -> None: ...
    def __repr__(self) -> str: ...


# StateByte (decodes the device status byte found in the SMART error log)
class StateByte:
    """Decoded device status byte found in the SMART error log."""

    byte: int
    device_fault: bool
    stream_error: bool

    def __init__(self, byte: int) -> None: ...

    def get_device_state(self) -> str:
        """Human-readable device state (e.g. 'Active/Idle', 'Standby')"""
        ...

    def __repr__(self) -> str: ...


# ErrorCommand / ErrorLogData / ErrorLog (SMART Summary Error Log, log page 0x01)
class ErrorCommand:
    """One of the 5 ATA commands that preceded a logged error."""

    spvalue: int
    feature: int
    sector_count: int
    device: int
    command: int
    timestamp: int

    def __init__(self) -> None: ...

    @property
    def lba(self) -> Tuple[int, int, int]:
        """LBA bytes as a (low, mid, high) tuple"""
        ...

    def __repr__(self) -> str: ...


class ErrorLogData:
    """A single entry in the SMART Summary Error Log."""

    cerror: int
    """Error register at time of error"""

    sector_count: int
    device: int
    written_status: int

    life_timestamp: int
    """Power-on hours at the time this error occurred"""

    def __init__(self) -> None: ...

    @property
    def error_commands(self) -> List[ErrorCommand]:
        """The 5 commands that preceded this error"""
        ...

    @property
    def lba(self) -> Tuple[int, int, int]: ...

    @property
    def state(self) -> StateByte:
        """Decoded device state at time of error"""
        ...


class ErrorLog:
    """SMART Summary Error Log (log page 0x01)."""

    log_version: int
    log_index: int

    error_count: int
    """Total number of errors logged by the device (lifetime)"""

    checksum: int

    def __init__(self) -> None: ...

    @property
    def errors(self) -> List[ErrorLogData]:
        """Up to 5 most recent error log entries"""
        ...

    def __repr__(self) -> str: ...


# ------------------------------------------------------------------
# SmartValues struct
# ------------------------------------------------------------------
class SmartValues:
    """Raw SMART READ DATA structure (subset of fields exposed)."""

    revision_number: int
    offline_data_collection_status: int
    self_test_execution_status: int
    total_time_to_complete_offline_data_collection: int

    def __init__(self) -> None: ...
    def __repr__(self) -> str: ...


# ------------------------------------------------------------------
# SmartReader class
# ------------------------------------------------------------------
class SmartReader:
    """Reads SMART / IDENTIFY DEVICE data from a physical drive."""

    def __init__(self, drive_number: int) -> None:
        """Create SMART reader for physical drive number (0, 1, 2, ...)"""
        ...

    def __init__(self, drive_path: str) -> None:  # type: ignore[no-redef]
        """Create SMART reader for drive path (e.g., '\\\\.\\PhysicalDrive0')"""
        ...

    # Properties
    @property
    def is_valid(self) -> bool:
        """Check if SMART data was read successfully"""
        ...

    @property
    def drive_path(self) -> str:
        """Get the drive path"""
        ...

    @property
    def revision_number(self) -> int:
        """Get SMART revision number"""
        ...

    @property
    def valid_attributes(self) -> List[SmartAttribute]:
        """Get list of valid SMART attributes"""
        ...

    @property
    def raw_data(self) -> SmartValues:
        """Get raw SMART data structure"""
        ...

    # Methods
    def refresh(self) -> bool:
        """Refresh SMART data from drive"""
        ...

    def find_attribute(self, attribute_id: int) -> Optional[SmartAttribute]:
        """Find specific attribute by ID. Returns None if not found.
        The returned SmartAttribute is a reference tied to this
        SmartReader's lifetime (do not use after the reader is refreshed
        or destroyed).
        """
        ...

    # Common attributes
    def get_temperature(self) -> int:
        """Get drive temperature in Celsius (-1 if not available)"""
        ...

    def get_power_on_hours(self) -> int:
        """Get power-on hours (0 if not available)"""
        ...

    def get_power_cycle_count(self) -> int:
        """Get power cycle count (0 if not available)"""
        ...

    def get_reallocated_sectors_count(self) -> int:
        """Get reallocated sectors count (0 if not available)"""
        ...

    # SSD specific
    def get_ssd_life_left(self) -> int:
        """Get SSD life remaining percentage (-1 if not available)"""
        ...

    def get_total_bytes_written(self) -> int:
        """Get total bytes written (SSD only, 0 if not available)"""
        ...

    def get_total_bytes_read(self) -> int:
        """Get total bytes read (SSD only, 0 if not available)"""
        ...

    def get_wear_leveling_count(self) -> int:
        """Get wear leveling count (SSD only, 0 if not available)"""
        ...

    # Drive type detection
    def is_probably_ssd(self) -> bool:
        """Check if drive is likely an SSD"""
        ...

    def is_probably_hdd(self) -> bool:
        """Check if drive is likely an HDD"""
        ...

    def get_drive_type(self) -> str:
        """Get drive type as string ('SSD', 'HDD', or 'Unknown')"""
        ...

    # Newly bound members
    def fill_disk_info(self) -> Optional[dict]:
        """Send IDENTIFY DEVICE and return a dict with model_number, serial_number,
        firmware_revision, user_addressable_sectors and nominal_media_rotation_rate.
        Returns None on failure. (A simplified view of the full ATA IDENTIFY
        structure, which is not exposed field-by-field.)
        """
        ...

    def get_smart_thresholds(self) -> List[SmartThreshold]:
        """Read the SMART attribute thresholds table, returns list[SmartThreshold]"""
        ...

    def read_log(self, log_number: int) -> Optional[bytes]:
        """Read a raw SMART log page (e.g. 1 = Summary Error Log) and return
        512 raw bytes, or None on failure.
        """
        ...

    def read_error_log(self) -> Optional[ErrorLog]:
        """Read and parse the SMART Summary Error Log (log page 0x01).
        Returns an ErrorLog, or None on failure.
        """
        ...

    def run_test(self, test_type: int = 0x01) -> bool:
        """Start a SMART self-test (SMART EXECUTE OFF-LINE IMMEDIATE).
        test_type defaults to 0x01 (short off-line test).
        """
        ...

    def __repr__(self) -> str: ...


def scan_all_drives(
    max_drives: int = 8,
) -> Tuple[List[SmartReader], List[Tuple[int, str]]]:
    """Scan all available drives and return tuple of (readers_list, errors_list)
    Returns: ([SmartReader, ...], [(drive_num, error_msg), ...])
    """
    ...


# ------------------------------------------------------------------
# SSDType enum
# ------------------------------------------------------------------
class SSDType:
    """Detected SSD controller/vendor family"""

    HDD_GENERAL: "SSDType"
    ADATA_INDUSTRIAL: "SSDType"
    SANDISK: "SSDType"
    WDC: "SSDType"
    SEAGATE: "SSDType"
    MTRON: "SSDType"
    TOSHIBA: "SSDType"
    JMICRON_66X: "SSDType"
    JMICRON_61X: "SSDType"
    JMICRON_60X: "SSDType"
    INDILINX: "SSDType"
    INTEL_DC: "SSDType"
    INTEL: "SSDType"
    SAMSUNG: "SSDType"
    MICRON_MU03: "SSDType"
    MICRON: "SSDType"
    SANDFORCE: "SSDType"
    OCZ: "SSDType"
    OCZ_VECTOR: "SSDType"
    SSSTC: "SSDType"
    PLEXTOR: "SSDType"
    KINGSTON: "SSDType"
    CORSAIR: "SSDType"
    REALTEK: "SSDType"
    SK_HYNIX: "SSDType"
    KIOXIA: "SSDType"
    SILICON_MOTION_CVC: "SSDType"
    SILICON_MOTION: "SSDType"
    PHISON: "SSDType"
    MARVELL: "SSDType"
    MAXIOTEK: "SSDType"
    APACER: "SSDType"
    YMTC: "SSDType"
    SCY: "SSDType"
    RECADATA: "SSDType"
    GENERAL_SSD: "SSDType"

    name: str
    value: int

    def __init__(self, value: int) -> None: ...
    def __int__(self) -> int: ...
    def __eq__(self, other: object) -> bool: ...
    def __repr__(self) -> str: ...


class SMARTInfoS:
    """Model/firmware/attributes bundle used as input to detect_ssd_type()"""

    model_upper: str
    attributes: List[SmartAttribute]
    firmware_rev: str
    is_ssd: bool

    def __init__(self) -> None: ...
    def __repr__(self) -> str: ...


def get_disk_info_s(drive_number: int) -> Optional[SMARTInfoS]:
    """Open the given physical drive, read SMART + IDENTIFY data and return
    a SMARTInfoS ready to pass to detect_ssd_type(). Returns None on failure.
    """
    ...


def detect_ssd_type(
    info: SMARTInfoS, raw_smart_data: Optional[bytes] = None
) -> SSDType:
    """Detect the SSD controller/vendor type (or HDD_GENERAL) from a SMARTInfoS.
    raw_smart_data is an optional 512-byte SMART READ DATA buffer, needed only
    to disambiguate a small number of Silicon Motion / ADATA models.
    """
    ...


def ssd_type_to_string(type: SSDType) -> str:
    """Human-readable name for an SSDType, e.g. 'Phison', 'Samsung', 'HDD'."""
    ...


def get_attribute_name_by_id_and_type(type: SSDType, attribute_id: int) -> str:
    """Vendor-specific human-readable name for a SMART attribute ID, given the
    SSDType returned by detect_ssd_type(). Falls back to a generic ATA name
    when the vendor doesn't define anything special for that ID.
    """
    ...


# ------------------------------------------------------------------
# Per-vendor detection heuristics (used internally by detect_ssd_type,
# exposed individually since they are free functions in the header).
# Most users only need detect_ssd_type() above.
# ------------------------------------------------------------------

def is_ssd_old(model_upper: str) -> bool: ...

def is_ssd_mtron(
    attributes: List[SmartAttribute], model_upper: str, attribute_count: int
) -> bool: ...

def is_ssd_jmicron_60x(attributes: List[SmartAttribute]) -> bool: ...

def is_ssd_jmicron_61x(attributes: List[SmartAttribute]) -> bool: ...

def is_ssd_jmicron_66x(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_indilinx(attributes: List[SmartAttribute]) -> bool: ...

def is_ssd_intel_dc(model_upper: str) -> bool: ...

def is_ssd_intel(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_samsung(
    attributes: List[SmartAttribute], model_upper: str, is_ssd: bool
) -> bool: ...

def is_ssd_sandforce(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_micron_mu03(model_upper: str, firmware_rev: str) -> bool: ...

def is_ssd_micron(
    attributes: List[SmartAttribute], model_upper: str, firmware_rev: str
) -> bool: ...

def is_ssd_ocz(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_ocz_vector(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_ssstc(model_upper: str) -> bool: ...

def is_ssd_plextor(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_sandisk(model_upper: str) -> bool: ...

def is_ssd_kingston(model_upper: str) -> bool: ...

def is_ssd_corsair(model_upper: str) -> bool: ...

def is_ssd_toshiba(model_upper: str, is_ssd: bool) -> bool: ...

def is_ssd_realtek(attributes: List[SmartAttribute]) -> bool: ...

def is_ssd_skhynix(model_upper: str) -> bool: ...

def is_ssd_kioxia(model_upper: str) -> bool: ...

def is_ssd_apacer(model_upper: str, firmware_rev: str) -> bool: ...

def is_ssd_ymtc(model_upper: str) -> bool: ...

def is_ssd_scy(model_upper: str) -> bool: ...

def is_ssd_recadata(model_upper: str) -> bool: ...

def is_ssd_silicon_motion_cvc(model_upper: str) -> bool: ...

def is_ssd_silicon_motion(
    attributes: List[SmartAttribute],
    model_upper: str,
    firmware_rev: str,
    raw_smart_data: Optional[bytes] = None,
) -> bool: ...

def is_ssd_phison(
    attributes: List[SmartAttribute], model_upper: str, firmware_rev: str
) -> bool: ...

def is_ssd_wdc(model_upper: str) -> bool: ...

def is_ssd_seagate(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_marvell(
    attributes: List[SmartAttribute], model_upper: str, firmware_rev: str
) -> bool: ...

def is_ssd_maxiotek(
    attributes: List[SmartAttribute], model_upper: str
) -> bool: ...

def is_ssd_adata_industrial(model_upper: str) -> bool: ...


# ------------------------------------------------------------------
# Low-level SCSI / NVMe helpers
# ------------------------------------------------------------------

def get_scsi_path(path: str) -> str:
    """Resolve a device path (e.g. '\\\\.\\PhysicalDrive0') to its underlying
    '\\\\.\\SCSIn:' path, or '' on failure.
    """
    ...


def get_scsi_address(path: str) -> Optional[Tuple[int, int, int, int]]:
    """Get the (port, path_id, target_id, lun) SCSI address of a device path,
    or None on failure.
    """
    ...


def get_smart_attribute_nvme_intel(drive_number: int) -> Optional[bytes]:
    """Read the raw NVMe SMART/Health log page via generic Intel NVMe
    pass-through. Returns 512 bytes, or None on failure.
    """
    ...


def get_smart_attribute_nvme_samsung(drive_number: int) -> Optional[bytes]:
    """Read the raw NVMe SMART/Health log page via Samsung's vendor-specific
    SCSI security protocol commands. Returns 512 bytes, or None on failure.
    """
    ...


def get_smart_attribute_nvme_storage_query(drive_number: int) -> Optional[bytes]:
    """Read the raw NVMe SMART/Health log page via the standard Windows
    IOCTL_STORAGE_QUERY_PROPERTY protocol-specific query. Returns 512
    bytes, or None on failure. This is usually the first one to try.
    """
    ...


def get_smart_attribute_nvme_intel_rst(
    drive_number: int = -1, scsi_port: int = 0, scsi_target_id: int = 0
) -> Optional[bytes]:
    """Read the raw NVMe SMART/Health log page through an Intel Rapid Storage
    Technology (RST) SCSI miniport pass-through. Pass drive_number=-1 with
    an explicit scsi_port/scsi_target_id if you already know the SCSI
    address; otherwise pass a physical drive number and it will be resolved
    automatically. Returns 512 bytes, or None on failure.
    """
    ...


def get_smart_attribute_nvme_intel_vroc(
    drive_number: int = -1, scsi_port: int = 0, scsi_target_id: int = 0
) -> Optional[bytes]:
    """Read the raw NVMe SMART/Health log page through an Intel Virtual RAID
    on CPU (VROC) SCSI miniport pass-through. Same drive_number/scsi_port/
    scsi_target_id conventions as get_smart_attribute_nvme_intel_rst().
    Returns 512 bytes, or None on failure.
    """
    ...
