/*
  █████████  ██████   ██████   █████████   ███████████   ███████████
 ███░░░░░███░░██████ ██████   ███░░░░░███ ░░███░░░░░███ ░█░░░███░░░█
░███    ░░░  ░███░█████░███  ░███    ░███  ░███    ░███ ░   ░███  ░ 
░░█████████  ░███░░███ ░███  ░███████████  ░██████████      ░███    
 ░░░░░░░░███ ░███ ░░░  ░███  ░███░░░░░███  ░███░░░░░███     ░███    
 ███    ░███ ░███      ░███  ░███    ░███  ░███    ░███     ░███    
░░█████████  █████     █████ █████   █████ █████   █████    █████   
 ░░░░░░░░░  ░░░░░     ░░░░░ ░░░░░   ░░░░░ ░░░░░   ░░░░░    ░░░░░    

===============================================================================
MIT License

Copyright (c) 2026 gafoo

This file is part of the HardView project:
https://github.com/gafoo173/HardView

Licensed under the MIT License
See the LICENSE file in the project root for more details.
================================================================================
*/
/**
 * @version 3.0
 */
#pragma once
#include <windows.h>
#include <winioctl.h>
#include <algorithm> // for std::find_if, std::reverse_iterator
#include <cctype>    // for std::isspace
#include <cstdint>
#include <iomanip>
#include <memory>
#include <ntddscsi.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <nvme.h>
#include <stddef.h>
namespace smart_reader {

#pragma pack(push, 1) // No Padding

struct SmartThreshold {
  BYTE Id;
  BYTE Threshold;
  BYTE Reserved[10];
};

struct SmartAttribute {
  BYTE Id;
  WORD Flags;
  BYTE Current;
  BYTE Worst;
  BYTE RawValue[6];
  BYTE Reserved;

  inline ULONGLONG GetRawValue() const {
    ULONGLONG rawValue = 0;
    for (int j = 0; j < 6; j++) {
      rawValue |= ((ULONGLONG)RawValue[j]) << (j * 8);
    }
    return rawValue;
  }
  inline std::string GetAttributeName() const {
    switch (Id) {
    case 0x01:
      return "Read Error Rate";
    case 0x02:
      return "Throughput Performance";
    case 0x03:
      return "Spin-Up Time";
    case 0x04:
      return "Start/Stop Count";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x06:
      return "Read Channel Margin";
    case 0x07:
      return "Seek Error Rate";
    case 0x08:
      return "Seek Time Performance";
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count";
    case 0x0B:
      return "Recalibration Retries";
    case 0x0C:
      return "Power Cycle Count";
    case 0x0D:
      return "Soft Read Error Rate stab";
    case 0x16:
      return "Current Helium Level";
    case 0x17:
      return "Helium Condition Lower";
    case 0x18:
      return "Helium Condition Upper";
    case 0x1B:
      return "MAMR Health Monitor";
    case 0xB8:
      return "End-to-End Error";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xBC:
      return "Command Timeout";
    case 0xBD:
      return "High Fly Writes";
    case 0xBE:
      return "Airflow Temperature";
    case 0xBF:
      return "G-Sense Error Rate";
    case 0xC0:
      return "Power-off Retract Count (HDD) Or Unsafe Shutdown Count (SSD)";
    case 0xC1:
      return "Load/Unload Cycle Count";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "Hardware ECC recovered";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xC7:
      return "UltraDMA CRC Error Count";
    case 0xC8:
      return "Write Error Rate";
    case 0xC9:
      return "Soft Read Error Rate";
    case 0xCA:
      return "Data Address Mark Error";
    case 0xCB:
      return "Run Out Cancel";
    case 0xCC:
      return "Soft ECC Correction";
    case 0xCD:
      return "Thermal Asperity Rate";
    case 0xCE:
      return "Flying Height";
    case 0xCF:
      return "Spin High Current";
    case 0xD0:
      return "Spin Buzz";
    case 0xD1:
      return "Offline Seek Performance";
    case 0xD3:
      return "Vibration During Write";
    case 0xD4:
      return "Shock During Write";
    case 0xDC:
      return "Disk Shift";
    case 0xDD:
      return "G-Sense Error Rate";
    case 0xDE:
      return "Loaded Hours";
    case 0xDF:
      return "Load/Unload Retry Count";
    case 0xE0:
      return "Load Friction";
    case 0xE1:
      return "Load/Unload Cycle Count";
    case 0xE2:
      return "Load 'In'-time";
    case 0xE3:
      return "Torque Amplification Count";
    case 0xE4:
      return "Power-Off Retract Cycle";
    case 0xE6:
      return "GMR Head Amplitude";
    case 0xE7:
      return "Temperature";
    case 0xF0:
      return "Head Flying Hours";
    case 0xF1:
      return "Total Host Writes";
    case 0xF2:
      return "Total Host Reads";
    case 0xFA:
      return "Read Error Retry Rate";
    case 0xFE:
      return "Free Fall Protection";
    case 0xFF:
      return "Remaining Life";
    default:
      return "Unknown Attribute";
    }
  }
};

// The struct must be exactly 512 bytes, and the order of the members is also
// important to get the correct data.
struct SmartValues {
  WORD RevisionNumber;
  SmartAttribute Attributes[30];
  BYTE OfflineDataCollectionStatus;
  BYTE SelfTestExecutionStatus;
  WORD TotalTimeToCompleteOfflineDataCollection;
  BYTE VendorSpecific;
  BYTE OfflineDataCollectionCapability;
  WORD SmartCapability;
  BYTE ErrorLoggingCapability;
  BYTE VendorSpecific2;
  BYTE ShortSelfTestPollingTime;
  BYTE ExtendedSelfTestPollingTime;
  BYTE Reserved[12];
  BYTE VendorSpecific3[125];
  BYTE Checksum;
};
// this struct From
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data
typedef struct _IDENTIFY_DEVICE_DATA {
  struct {
    USHORT Reserved1 : 1;
    USHORT Retired3 : 1;
    USHORT ResponseIncomplete : 1;
    USHORT Retired2 : 3;
    USHORT FixedDevice : 1;
    USHORT RemovableMedia : 1;
    USHORT Retired1 : 7;
    USHORT DeviceType : 1;
  } GeneralConfiguration;
  USHORT NumCylinders;
  USHORT SpecificConfiguration;
  USHORT NumHeads;
  USHORT Retired1[2];
  USHORT NumSectorsPerTrack;
  USHORT VendorUnique1[3];
  UCHAR SerialNumber[20];
  USHORT Retired2[2];
  USHORT Obsolete1;
  UCHAR FirmwareRevision[8];
  UCHAR ModelNumber[40];
  UCHAR MaximumBlockTransfer;
  UCHAR VendorUnique2;
  struct {
    USHORT FeatureSupported : 1;
    USHORT Reserved : 15;
  } TrustedComputing;
  struct {
    UCHAR CurrentLongPhysicalSectorAlignment : 2;
    UCHAR ReservedByte49 : 6;
    UCHAR DmaSupported : 1;
    UCHAR LbaSupported : 1;
    UCHAR IordyDisable : 1;
    UCHAR IordySupported : 1;
    UCHAR Reserved1 : 1;
    UCHAR StandybyTimerSupport : 1;
    UCHAR Reserved2 : 2;
    USHORT ReservedWord50;
  } Capabilities;
  USHORT ObsoleteWords51[2];
  USHORT TranslationFieldsValid : 3;
  USHORT Reserved3 : 5;
  USHORT FreeFallControlSensitivity : 8;
  USHORT NumberOfCurrentCylinders;
  USHORT NumberOfCurrentHeads;
  USHORT CurrentSectorsPerTrack;
  ULONG CurrentSectorCapacity;
  UCHAR CurrentMultiSectorSetting;
  UCHAR MultiSectorSettingValid : 1;
  UCHAR ReservedByte59 : 3;
  UCHAR SanitizeFeatureSupported : 1;
  UCHAR CryptoScrambleExtCommandSupported : 1;
  UCHAR OverwriteExtCommandSupported : 1;
  UCHAR BlockEraseExtCommandSupported : 1;
  ULONG UserAddressableSectors;
  USHORT ObsoleteWord62;
  USHORT MultiWordDMASupport : 8;
  USHORT MultiWordDMAActive : 8;
  USHORT AdvancedPIOModes : 8;
  USHORT ReservedByte64 : 8;
  USHORT MinimumMWXferCycleTime;
  USHORT RecommendedMWXferCycleTime;
  USHORT MinimumPIOCycleTime;
  USHORT MinimumPIOCycleTimeIORDY;
  struct {
    USHORT ZonedCapabilities : 2;
    USHORT NonVolatileWriteCache : 1;
    USHORT ExtendedUserAddressableSectorsSupported : 1;
    USHORT DeviceEncryptsAllUserData : 1;
    USHORT ReadZeroAfterTrimSupported : 1;
    USHORT Optional28BitCommandsSupported : 1;
    USHORT IEEE1667 : 1;
    USHORT DownloadMicrocodeDmaSupported : 1;
    USHORT SetMaxSetPasswordUnlockDmaSupported : 1;
    USHORT WriteBufferDmaSupported : 1;
    USHORT ReadBufferDmaSupported : 1;
    USHORT DeviceConfigIdentifySetDmaSupported : 1;
    USHORT LPSAERCSupported : 1;
    USHORT DeterministicReadAfterTrimSupported : 1;
    USHORT CFastSpecSupported : 1;
  } AdditionalSupported;
  USHORT ReservedWords70[5];
  USHORT QueueDepth : 5;
  USHORT ReservedWord75 : 11;
  struct {
    USHORT Reserved0 : 1;
    USHORT SataGen1 : 1;
    USHORT SataGen2 : 1;
    USHORT SataGen3 : 1;
    USHORT Reserved1 : 4;
    USHORT NCQ : 1;
    USHORT HIPM : 1;
    USHORT PhyEvents : 1;
    USHORT NcqUnload : 1;
    USHORT NcqPriority : 1;
    USHORT HostAutoPS : 1;
    USHORT DeviceAutoPS : 1;
    USHORT ReadLogDMA : 1;
    USHORT Reserved2 : 1;
    USHORT CurrentSpeed : 3;
    USHORT NcqStreaming : 1;
    USHORT NcqQueueMgmt : 1;
    USHORT NcqReceiveSend : 1;
    USHORT DEVSLPtoReducedPwrState : 1;
    USHORT Reserved3 : 8;
  } SerialAtaCapabilities;
  struct {
    USHORT Reserved0 : 1;
    USHORT NonZeroOffsets : 1;
    USHORT DmaSetupAutoActivate : 1;
    USHORT DIPM : 1;
    USHORT InOrderData : 1;
    USHORT HardwareFeatureControl : 1;
    USHORT SoftwareSettingsPreservation : 1;
    USHORT NCQAutosense : 1;
    USHORT DEVSLP : 1;
    USHORT HybridInformation : 1;
    USHORT Reserved1 : 6;
  } SerialAtaFeaturesSupported;
  struct {
    USHORT Reserved0 : 1;
    USHORT NonZeroOffsets : 1;
    USHORT DmaSetupAutoActivate : 1;
    USHORT DIPM : 1;
    USHORT InOrderData : 1;
    USHORT HardwareFeatureControl : 1;
    USHORT SoftwareSettingsPreservation : 1;
    USHORT DeviceAutoPS : 1;
    USHORT DEVSLP : 1;
    USHORT HybridInformation : 1;
    USHORT Reserved1 : 6;
  } SerialAtaFeaturesEnabled;
  USHORT MajorRevision;
  USHORT MinorRevision;
  struct {
    USHORT SmartCommands : 1;
    USHORT SecurityMode : 1;
    USHORT RemovableMediaFeature : 1;
    USHORT PowerManagement : 1;
    USHORT Reserved1 : 1;
    USHORT WriteCache : 1;
    USHORT LookAhead : 1;
    USHORT ReleaseInterrupt : 1;
    USHORT ServiceInterrupt : 1;
    USHORT DeviceReset : 1;
    USHORT HostProtectedArea : 1;
    USHORT Obsolete1 : 1;
    USHORT WriteBuffer : 1;
    USHORT ReadBuffer : 1;
    USHORT Nop : 1;
    USHORT Obsolete2 : 1;
    USHORT DownloadMicrocode : 1;
    USHORT DmaQueued : 1;
    USHORT Cfa : 1;
    USHORT AdvancedPm : 1;
    USHORT Msn : 1;
    USHORT PowerUpInStandby : 1;
    USHORT ManualPowerUp : 1;
    USHORT Reserved2 : 1;
    USHORT SetMax : 1;
    USHORT Acoustics : 1;
    USHORT BigLba : 1;
    USHORT DeviceConfigOverlay : 1;
    USHORT FlushCache : 1;
    USHORT FlushCacheExt : 1;
    USHORT WordValid83 : 2;
    USHORT SmartErrorLog : 1;
    USHORT SmartSelfTest : 1;
    USHORT MediaSerialNumber : 1;
    USHORT MediaCardPassThrough : 1;
    USHORT StreamingFeature : 1;
    USHORT GpLogging : 1;
    USHORT WriteFua : 1;
    USHORT WriteQueuedFua : 1;
    USHORT WWN64Bit : 1;
    USHORT URGReadStream : 1;
    USHORT URGWriteStream : 1;
    USHORT ReservedForTechReport : 2;
    USHORT IdleWithUnloadFeature : 1;
    USHORT WordValid : 2;
  } CommandSetSupport;
  struct {
    USHORT SmartCommands : 1;
    USHORT SecurityMode : 1;
    USHORT RemovableMediaFeature : 1;
    USHORT PowerManagement : 1;
    USHORT Reserved1 : 1;
    USHORT WriteCache : 1;
    USHORT LookAhead : 1;
    USHORT ReleaseInterrupt : 1;
    USHORT ServiceInterrupt : 1;
    USHORT DeviceReset : 1;
    USHORT HostProtectedArea : 1;
    USHORT Obsolete1 : 1;
    USHORT WriteBuffer : 1;
    USHORT ReadBuffer : 1;
    USHORT Nop : 1;
    USHORT Obsolete2 : 1;
    USHORT DownloadMicrocode : 1;
    USHORT DmaQueued : 1;
    USHORT Cfa : 1;
    USHORT AdvancedPm : 1;
    USHORT Msn : 1;
    USHORT PowerUpInStandby : 1;
    USHORT ManualPowerUp : 1;
    USHORT Reserved2 : 1;
    USHORT SetMax : 1;
    USHORT Acoustics : 1;
    USHORT BigLba : 1;
    USHORT DeviceConfigOverlay : 1;
    USHORT FlushCache : 1;
    USHORT FlushCacheExt : 1;
    USHORT Resrved3 : 1;
    USHORT Words119_120Valid : 1;
    USHORT SmartErrorLog : 1;
    USHORT SmartSelfTest : 1;
    USHORT MediaSerialNumber : 1;
    USHORT MediaCardPassThrough : 1;
    USHORT StreamingFeature : 1;
    USHORT GpLogging : 1;
    USHORT WriteFua : 1;
    USHORT WriteQueuedFua : 1;
    USHORT WWN64Bit : 1;
    USHORT URGReadStream : 1;
    USHORT URGWriteStream : 1;
    USHORT ReservedForTechReport : 2;
    USHORT IdleWithUnloadFeature : 1;
    USHORT Reserved4 : 2;
  } CommandSetActive;
  USHORT UltraDMASupport : 8;
  USHORT UltraDMAActive : 8;
  struct {
    USHORT TimeRequired : 15;
    USHORT ExtendedTimeReported : 1;
  } NormalSecurityEraseUnit;
  struct {
    USHORT TimeRequired : 15;
    USHORT ExtendedTimeReported : 1;
  } EnhancedSecurityEraseUnit;
  USHORT CurrentAPMLevel : 8;
  USHORT ReservedWord91 : 8;
  USHORT MasterPasswordID;
  USHORT HardwareResetResult;
  USHORT CurrentAcousticValue : 8;
  USHORT RecommendedAcousticValue : 8;
  USHORT StreamMinRequestSize;
  USHORT StreamingTransferTimeDMA;
  USHORT StreamingAccessLatencyDMAPIO;
  ULONG StreamingPerfGranularity;
  ULONG Max48BitLBA[2];
  USHORT StreamingTransferTime;
  USHORT DsmCap;
  struct {
    USHORT LogicalSectorsPerPhysicalSector : 4;
    USHORT Reserved0 : 8;
    USHORT LogicalSectorLongerThan256Words : 1;
    USHORT MultipleLogicalSectorsPerPhysicalSector : 1;
    USHORT Reserved1 : 2;
  } PhysicalLogicalSectorSize;
  USHORT InterSeekDelay;
  USHORT WorldWideName[4];
  USHORT ReservedForWorldWideName128[4];
  USHORT ReservedForTlcTechnicalReport;
  USHORT WordsPerLogicalSector[2];
  struct {
    USHORT ReservedForDrqTechnicalReport : 1;
    USHORT WriteReadVerify : 1;
    USHORT WriteUncorrectableExt : 1;
    USHORT ReadWriteLogDmaExt : 1;
    USHORT DownloadMicrocodeMode3 : 1;
    USHORT FreefallControl : 1;
    USHORT SenseDataReporting : 1;
    USHORT ExtendedPowerConditions : 1;
    USHORT Reserved0 : 6;
    USHORT WordValid : 2;
  } CommandSetSupportExt;
  struct {
    USHORT ReservedForDrqTechnicalReport : 1;
    USHORT WriteReadVerify : 1;
    USHORT WriteUncorrectableExt : 1;
    USHORT ReadWriteLogDmaExt : 1;
    USHORT DownloadMicrocodeMode3 : 1;
    USHORT FreefallControl : 1;
    USHORT SenseDataReporting : 1;
    USHORT ExtendedPowerConditions : 1;
    USHORT Reserved0 : 6;
    USHORT Reserved1 : 2;
  } CommandSetActiveExt;
  USHORT ReservedForExpandedSupportandActive[6];
  USHORT MsnSupport : 2;
  USHORT ReservedWord127 : 14;
  struct {
    USHORT SecuritySupported : 1;
    USHORT SecurityEnabled : 1;
    USHORT SecurityLocked : 1;
    USHORT SecurityFrozen : 1;
    USHORT SecurityCountExpired : 1;
    USHORT EnhancedSecurityEraseSupported : 1;
    USHORT Reserved0 : 2;
    USHORT SecurityLevel : 1;
    USHORT Reserved1 : 7;
  } SecurityStatus;
  USHORT ReservedWord129[31];
  struct {
    USHORT MaximumCurrentInMA : 12;
    USHORT CfaPowerMode1Disabled : 1;
    USHORT CfaPowerMode1Required : 1;
    USHORT Reserved0 : 1;
    USHORT Word160Supported : 1;
  } CfaPowerMode1;
  USHORT ReservedForCfaWord161[7];
  USHORT NominalFormFactor : 4;
  USHORT ReservedWord168 : 12;
  struct {
    USHORT SupportsTrim : 1;
    USHORT Reserved0 : 15;
  } DataSetManagementFeature;
  USHORT AdditionalProductID[4];
  USHORT ReservedForCfaWord174[2];
  USHORT CurrentMediaSerialNumber[30];
  struct {
    USHORT Supported : 1;
    USHORT Reserved0 : 1;
    USHORT WriteSameSuported : 1;
    USHORT ErrorRecoveryControlSupported : 1;
    USHORT FeatureControlSuported : 1;
    USHORT DataTablesSuported : 1;
    USHORT Reserved1 : 6;
    USHORT VendorSpecific : 4;
  } SCTCommandTransport;
  USHORT ReservedWord207[2];
  struct {
    USHORT AlignmentOfLogicalWithinPhysical : 14;
    USHORT Word209Supported : 1;
    USHORT Reserved0 : 1;
  } BlockAlignment;
  USHORT WriteReadVerifySectorCountMode3Only[2];
  USHORT WriteReadVerifySectorCountMode2Only[2];
  struct {
    USHORT NVCachePowerModeEnabled : 1;
    USHORT Reserved0 : 3;
    USHORT NVCacheFeatureSetEnabled : 1;
    USHORT Reserved1 : 3;
    USHORT NVCachePowerModeVersion : 4;
    USHORT NVCacheFeatureSetVersion : 4;
  } NVCacheCapabilities;
  USHORT NVCacheSizeLSW;
  USHORT NVCacheSizeMSW;
  USHORT NominalMediaRotationRate;
  USHORT ReservedWord218;
  struct {
    UCHAR NVCacheEstimatedTimeToSpinUpInSeconds;
    UCHAR Reserved;
  } NVCacheOptions;
  USHORT WriteReadVerifySectorCountMode : 8;
  USHORT ReservedWord220 : 8;
  USHORT ReservedWord221;
  struct {
    USHORT MajorVersion : 12;
    USHORT TransportType : 4;
  } TransportMajorVersion;
  USHORT TransportMinorVersion;
  USHORT ReservedWord224[6];
  ULONG ExtendedNumberOfUserAddressableSectors[2];
  USHORT MinBlocksPerDownloadMicrocodeMode03;
  USHORT MaxBlocksPerDownloadMicrocodeMode03;
  USHORT ReservedWord236[19];
  USHORT Signature : 8;
  USHORT CheckSum : 8;
} IDENTIFY_DEVICE_DATA, *PIDENTIFY_DEVICE_DATA;
#define CHECK_BIT(value, bit) (((value) >> (bit)) & 0x1)

struct StateByte {

uint8_t Byte;
bool DeviceFault;
bool StreamError;
operator uint8_t() const {
    return Byte;
}
StateByte(uint8_t byte) {
  this->Byte = byte;
  StreamError = CHECK_BIT(byte,0);
  DeviceFault = CHECK_BIT(byte,5);
}


std::string GetDeviceState() const {
    unsigned char v = Byte & 0x0F;
    switch (v)
    {
        case 0x0: return "Unknown";
        case 0x1: return "Sleep";
        case 0x2: return "Standby";
        case 0x3: return "Active/Idle";
        case 0x4: return "Executing SMART off-line or self-test";
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0xA:
            return "Reserved";

        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0xF:
            return "Vendor specific";
    }

    return "Unknown";
}
};

struct ErrorCommand {
  uint8_t spvalue;
  uint8_t feature;
  uint8_t sector_count;
  uint8_t LBA[3];
  uint8_t device;
  uint8_t command;
  DWORD timestamp;
};

struct ErrorLogData {
  ErrorCommand error_commands[5];
  uint8_t reserved;
  uint8_t cerror;
  uint8_t sector_count;
  uint8_t LBA[3];
  uint8_t device;
  uint8_t written_status;
  uint8_t venspecific[19];
  uint8_t StatByte;
  uint16_t life_timestamp;
};

struct ErrorLog {
uint8_t log_version;
uint8_t log_index;
ErrorLogData errors[5];
uint16_t error_count;
uint8_t reserved[57];
uint8_t checksum;
};

#pragma pack(pop)

class SmartReader {
private:
  HANDLE hDevice;
  std::string drivePath;
  SmartValues smartData;
  bool isValid;
  std::vector<SmartAttribute> validAttributes;

  inline bool EnableSmart() {
    ATA_PASS_THROUGH_DIRECT aptd = {};
    aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
    aptd.TimeOutValue = 10000;
    aptd.DataTransferLength = 0;
    aptd.DataBuffer = nullptr;
    aptd.AtaFlags = ATA_FLAGS_DRDY_REQUIRED;

    // SMART ENABLE command
    aptd.CurrentTaskFile[0] = 0xD8; // Features = SMART ENABLE
    aptd.CurrentTaskFile[1] = 0x00; // Sector Count
    aptd.CurrentTaskFile[2] = 0x00; // LBA Low
    aptd.CurrentTaskFile[3] = 0x4F; // LBA Mid (SMART signature)
    aptd.CurrentTaskFile[4] = 0xC2; // LBA High (SMART signature)
    aptd.CurrentTaskFile[5] = 0x00; // Device/Head
    aptd.CurrentTaskFile[6] = 0xB0; // Command = SMART

    DWORD returned = 0;
    return DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH_DIRECT, &aptd,
                           sizeof(aptd), &aptd, sizeof(aptd), &returned,
                           nullptr);
  }

  inline bool ReadSmartData(UCHAR SMARTCMD = 0xD0, uint8_t *RAWMODE = nullptr,UCHAR SECC = 0x00,UCHAR LBAL = 0x0) {
    if (!RAWMODE)
      ZeroMemory(&smartData, sizeof(smartData));

    ATA_PASS_THROUGH_DIRECT aptd = {};
    aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
    aptd.TimeOutValue =
        10000; // 10 seconds timeout This time is usually enough.
    aptd.DataTransferLength = sizeof(SmartValues);

    if (!RAWMODE)
      aptd.DataBuffer = &smartData;
    else
       aptd.DataBuffer = RAWMODE;
    aptd.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

    // SMART READ DATA command
    aptd.CurrentTaskFile[0] = SMARTCMD; // Features = READ DATA
    aptd.CurrentTaskFile[1] = SECC;     // Sector Count
    aptd.CurrentTaskFile[2] = LBAL;     // LBA Low
    aptd.CurrentTaskFile[3] = 0x4F;     // LBA Mid (SMART signature)
    aptd.CurrentTaskFile[4] = 0xC2;     // LBA High (SMART signature)
    aptd.CurrentTaskFile[5] = 0x00;     // Device/Head
    aptd.CurrentTaskFile[6] = 0xB0;     // Command = SMART

    DWORD returned = 0;
    bool result =
        DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH_DIRECT, &aptd,
                        sizeof(aptd), &aptd, sizeof(aptd), &returned, nullptr);
    if (RAWMODE) {
      return result && returned > 0;
    }
    if (result && returned > 0) {
      // Populate valid attributes vector
      validAttributes.clear();
      for (int i = 0; i < 30; i++) {
        if (smartData.Attributes[i].Id != 0) {
          validAttributes.push_back(smartData.Attributes[i]);
        }
      }
    }

    return result && returned > 0;
  }


public:
  // Constructor - opens drive and reads SMART data
  inline explicit SmartReader(int driveNumber)
      : hDevice(INVALID_HANDLE_VALUE), isValid(false) {

    drivePath = "\\\\.\\PhysicalDrive" + std::to_string(driveNumber);

    hDevice = CreateFileA(drivePath.c_str(), GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                          OPEN_EXISTING, 0, nullptr);

    if (hDevice == INVALID_HANDLE_VALUE) {
      throw std::runtime_error("Failed to open drive " + drivePath +
                               ". Error: " + std::to_string(GetLastError()));
    }

    if (!EnableSmart()) {
      CloseHandle(hDevice);
      hDevice = INVALID_HANDLE_VALUE;
      throw std::runtime_error("Failed to enable SMART for " + drivePath +
                               ". Error: " + std::to_string(GetLastError()));
    }

    if (!ReadSmartData()) {
      CloseHandle(hDevice);
      hDevice = INVALID_HANDLE_VALUE;
      throw std::runtime_error("Failed to read SMART data for " + drivePath +
                               ". Error: " + std::to_string(GetLastError()));
    }

    isValid = true;
  }

  // Constructor with drive path
  inline explicit SmartReader(const std::string &path)
      : hDevice(INVALID_HANDLE_VALUE), drivePath(path), isValid(false) {

    hDevice = CreateFileA(drivePath.c_str(), GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                          OPEN_EXISTING, 0, nullptr);

    if (hDevice == INVALID_HANDLE_VALUE) {
      throw std::runtime_error("Failed to open drive " + drivePath +
                               ". Error: " + std::to_string(GetLastError()));
    }

    if (!EnableSmart()) {
      CloseHandle(hDevice);
      hDevice = INVALID_HANDLE_VALUE;
      throw std::runtime_error("Failed to enable SMART for " + drivePath +
                               ". Error: " + std::to_string(GetLastError()));
    }

    if (!ReadSmartData()) {
      CloseHandle(hDevice);
      hDevice = INVALID_HANDLE_VALUE;
      throw std::runtime_error("Failed to read SMART data for " + drivePath +
                               ". Error: " + std::to_string(GetLastError()));
    }

    isValid = true;
  }

  // Destructor - RAII cleanup
  inline ~SmartReader() {
    if (hDevice != INVALID_HANDLE_VALUE) {
      CloseHandle(hDevice);
    }
  }

  // Move constructor
  inline SmartReader(SmartReader &&other) noexcept
      : hDevice(other.hDevice), drivePath(std::move(other.drivePath)),
        smartData(other.smartData), isValid(other.isValid),
        validAttributes(std::move(other.validAttributes)) {
    other.hDevice = INVALID_HANDLE_VALUE;
    other.isValid = false;
  }

  // Move assignment
  inline SmartReader &operator=(SmartReader &&other) noexcept {
    if (this != &other) {
      if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
      }

      hDevice = other.hDevice;
      drivePath = std::move(other.drivePath);
      smartData = other.smartData;
      isValid = other.isValid;
      validAttributes = std::move(other.validAttributes);

      other.hDevice = INVALID_HANDLE_VALUE;
      other.isValid = false;
    }
    return *this;
  }

  // Delete copy constructor and assignment
  SmartReader(const SmartReader &) = delete;
  SmartReader &operator=(const SmartReader &) = delete;

  // Getters
  inline bool IsValid() const { return isValid; }
  inline const std::string &GetDrivePath() const { return drivePath; }
  inline WORD GetRevisionNumber() const { return smartData.RevisionNumber; }
  inline const std::vector<SmartAttribute> &GetValidAttributes() const {
    return validAttributes;
  }
  inline const SmartValues &GetRawData() const { return smartData; }

  // Refresh SMART data
  inline bool Refresh() {
    if (hDevice == INVALID_HANDLE_VALUE) {
      return false;
    }

    return ReadSmartData();
  }

  // Find specific attribute by ID
  inline const SmartAttribute *FindAttribute(BYTE attributeId) const {
    for (const auto &attr : validAttributes) {
      if (attr.Id == attributeId) {
        return &attr;
      }
    }
    return nullptr;
  }

  // Get temperature (if available)
  inline int GetTemperature() const {
    const SmartAttribute *tempAttr = FindAttribute(0xC2);
    if (tempAttr) {
      return tempAttr->RawValue[0]; // Temperature is usually in the first byte
    }
    return -1; // Not available
  }

  // Get power-on hours (if available)
  inline ULONGLONG GetPowerOnHours() const {
    const SmartAttribute *pohAttr = FindAttribute(0x09);
    if (pohAttr) {
      return pohAttr->GetRawValue();
    }
    return 0; // Not available
  }

  // Get power cycle count (if available)
  inline ULONGLONG GetPowerCycleCount() const {
    const SmartAttribute *pccAttr = FindAttribute(0x0C);
    if (pccAttr) {
      return pccAttr->GetRawValue();
    }
    return 0; // Not available
  }

  // Get reallocated sectors count (critical for drive health)
  inline ULONGLONG GetReallocatedSectorsCount() const {
    const SmartAttribute *rscAttr = FindAttribute(0x05);
    if (rscAttr) {
      return rscAttr->GetRawValue();
    }
    return 0; // Not available
  }

  // SSD specific functions

  // Get SSD life remaining (percentage)
  inline int GetSsdLifeLeft() const {
    // Try different SSD life attributes
    const SmartAttribute *lifeAttr = FindAttribute(0xE7); // Most common
    if (!lifeAttr)
      lifeAttr = FindAttribute(0xA7);
    if (!lifeAttr)
      lifeAttr = FindAttribute(0xA9);

    if (lifeAttr) {
      return lifeAttr->Current; // Usually in Current value for SSDs
    }
    return -1; // Not available
  }

  // Get total bytes written (SSD)
  inline ULONGLONG GetTotalBytesWritten() const {
    const SmartAttribute *attr = FindAttribute(0xF1); // Total LBAs Written
    if (!attr)
      attr = FindAttribute(0xA3); // Alternative

    if (attr) {
      // Usually in 32MB units for 0xF1, or raw sectors for 0xA3
      ULONGLONG value = attr->GetRawValue();
      if (attr->Id == 0xF1) {
        return value * 32 * 1024 * 1024; // Convert 32MB units to bytes
      } else {
        return value *
               512; // Convert sectors to bytes (assuming 512-byte sectors)
      }
    }
    return 0; // Not available
  }

  // Get total bytes read (SSD)
  inline ULONGLONG GetTotalBytesRead() const {
    const SmartAttribute *attr = FindAttribute(0xF2); // Total LBAs Read
    if (attr) {
      // Usually in 32MB units
      return attr->GetRawValue() * 32 * 1024 * 1024;
    }
    return 0; // Not available
  }

  // Get wear leveling count (SSD)
  inline ULONGLONG GetWearLevelingCount() const {
    const SmartAttribute *attr = FindAttribute(0xAD);
    if (attr) {
      return attr->GetRawValue();
    }
    return 0; // Not available
  }

  // Check if drive is likely an SSD
  inline bool IsProbablySsd() const {
    // Check for SSD-specific attributes
    return FindAttribute(0xAD) != nullptr || // Wear Leveling Count
           FindAttribute(0xE7) != nullptr || // SSD Life Left
           FindAttribute(0xF1) != nullptr || // Total LBAs Written
           FindAttribute(0xA7) != nullptr;   // SSD Life Left (Alt)
  }

  // Check if drive is likely an HDD
  inline bool IsProbablyHdd() const {
    // Check for HDD-specific attributes
    return FindAttribute(0x03) != nullptr || // Spin Up Time
           FindAttribute(0x0A) != nullptr || // Spin Retry Count
           FindAttribute(0xC0) != nullptr;   // Power-off Retract Count
  }

  // Get drive type as string
  inline std::string GetDriveType() const {
    if (IsProbablySsd())
      return "SSD";
    if (IsProbablyHdd())
      return "HDD";
    return "Unknown";
  }

  bool FillDiskInfo(IDENTIFY_DEVICE_DATA &Info) {
    ATA_PASS_THROUGH_DIRECT aptd = {};
    aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
    aptd.TimeOutValue =
        10000; // 10 seconds timeout This time is usually enough.
    aptd.DataTransferLength = sizeof(IDENTIFY_DEVICE_DATA);
    aptd.DataBuffer = &Info;
    aptd.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

    aptd.CurrentTaskFile[0] = 0x00; // Features
    aptd.CurrentTaskFile[1] = 0x00; // Sector Count
    aptd.CurrentTaskFile[2] = 0x00; // LBA Low
    aptd.CurrentTaskFile[3] = 0x00; // LBA Mid
    aptd.CurrentTaskFile[4] = 0x00; // LBA High
    aptd.CurrentTaskFile[5] = 0x00; // Device/Head
    aptd.CurrentTaskFile[6] = 0xEC; // Command = IDENTIFY DEVICE

    // Send command
    DWORD bytesReturned = 0;
    BOOL result = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH_DIRECT, &aptd,
                                  sizeof(aptd), &aptd, sizeof(aptd),
                                  &bytesReturned, NULL);
    return result;
  }

  bool GetSmartThresholds(std::vector<SmartThreshold> &Vec) {
    uint8_t RAW[512];
    if (!ReadSmartData(0xD1, RAW)) {
      return false;
    }
    SmartThreshold *Attrs = reinterpret_cast<SmartThreshold *>(RAW + 2);
    for (int i = 0; i < 30; ++i) {
      SmartThreshold &Attr = Attrs[i];
      if (Attr.Id != 0) {
        Vec.push_back(Attr);
      }
    }
    return true;
  }

  bool ReadLog(UCHAR Log,uint8_t* data) {
    if (!ReadSmartData(0xD5, data,1,Log)) {
      return false;
    }
    return true;
  }

  bool ReadErrorLog(ErrorLog& Log) {
    return ReadLog(2,(uint8_t*)&Log);
  }
bool RunTest(UCHAR TestType = 0x01) {
    ATA_PASS_THROUGH_DIRECT aptd = {};
    aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
    aptd.TimeOutValue = 10000; // 10 seconds timeout
    aptd.DataTransferLength = 0;
    aptd.DataBuffer = nullptr;
    aptd.AtaFlags = ATA_FLAGS_DRDY_REQUIRED;

    // SMART EXECUTE OFF-LINE IMMEDIATE command
    aptd.CurrentTaskFile[0] = 0xD4;     // Features = EXECUTE OFF-LINE IMMEDIATE
    aptd.CurrentTaskFile[1] = TestType; // Sector Count = Test Type
    aptd.CurrentTaskFile[2] = 0x00;     // LBA Low
    aptd.CurrentTaskFile[3] = 0x4F;     // LBA Mid (SMART signature)
    aptd.CurrentTaskFile[4] = 0xC2;     // LBA High (SMART signature)
    aptd.CurrentTaskFile[5] = 0x00;     // Device/Head
    aptd.CurrentTaskFile[6] = 0xB0;     // Command = SMART

    DWORD returned = 0;
    bool result = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH_DIRECT, &aptd,
                                  sizeof(aptd), &aptd, sizeof(aptd), &returned, nullptr);

    return result && returned > 0;
}
};
// Utility function to scan all available drives
inline std::vector<std::unique_ptr<SmartReader>>
ScanAllDrives(int maxDrives = 8,
              std::vector<std::pair<int, std::string>> *error = nullptr) {
  std::vector<std::unique_ptr<SmartReader>> readers;

  for (int i = 0; i < maxDrives; i++) {
    try {
      auto reader = std::make_unique<SmartReader>(i);
      readers.push_back(std::move(reader));
    } catch (const std::exception &e) {
      std::string exp = e.what();
      auto s = std::make_pair(i, exp);
      if (error)
        error->push_back(s);
    }
  }

  return readers;
}

// Helper function to swap bytes in strings (for Model, Serial, Firmware)
std::string ByteSwapString(const UCHAR *data, int length) {
  std::string result;
  for (int i = 0; i < length; i += 2) {
    if (i + 1 < length) {
      result += data[i + 1];
      result += data[i];
    }
  }
  // Remove trailing spaces
  while (!result.empty() && result.back() == ' ') {
    result.pop_back();
  }
  return result;
}

std::string trim(const std::string &str) {
  auto start = std::find_if_not(
      str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
               return std::isspace(c);
             }).base();

  if (start >= end)
    return "";
  return std::string(start, end);
}

std::string ToUpper(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

struct SMARTInfoS {
  std::string modelUpper;
  std::vector<SmartAttribute> attributes;
  std::string firmwareRev;
  bool IsSSD;
};

bool IsSsdOld(const std::string &modelUpper);

bool GetDiskInfoS(int dnum, SMARTInfoS &info) {
  SmartReader reader(dnum);
  IDENTIFY_DEVICE_DATA Data;
  if (!reader.FillDiskInfo(Data)) {
    return false;
  }
  info.modelUpper = ToUpper(trim(ByteSwapString(Data.ModelNumber, 40)));
  info.attributes = reader.GetValidAttributes();
  info.firmwareRev = ToUpper(trim(ByteSwapString(Data.FirmwareRevision, 8)));
  info.IsSSD = reader.IsProbablySsd();
  if (!info.IsSSD) {
    info.IsSSD = IsSsdOld(info.modelUpper);
  }
  return true;
}

// SSD Detection Functions were copied from the CrystalDiskInfo source code,
// with modifications to the referenced types and parameters to fit this code,
// but the logic is largely identical.
// https://github.com/hiyohiyo/CrystalDiskInfo/blob/master/AtaSmart.cpp

bool IsSsdOld(const std::string &modelUpper) {
  return modelUpper.find("OCZ") == 0 || modelUpper.find("SPCC") == 0 ||
         modelUpper.find("PATRIOT") == 0 ||
         modelUpper.find("Solid") != std::string::npos ||
         modelUpper.find("SSD") != std::string::npos ||
         modelUpper.find("SiliconHardDisk") != std::string::npos ||
         modelUpper.find("PHOTOFAST") == 0 || modelUpper.find("STT_FTM") == 0 ||
         modelUpper.find("Super Talent") == 0;
}

bool IsSsdMtron(const std::vector<SmartAttribute> &attributes,
                const std::string &modelUpper, int attributeCount) {
  return ((attributes.size() > 0 && attributes[0].Id == 0xBB &&
           attributeCount == 1) ||
          modelUpper.find("MTRON") == 0);
}

bool IsSsdJMicron60x(const std::vector<SmartAttribute> &attributes) {
  if (attributes.size() < 6)
    return false;

  return attributes[0].Id == 0x0C && attributes[1].Id == 0x09 &&
         attributes[2].Id == 0xC2 && attributes[3].Id == 0xE5 &&
         attributes[4].Id == 0xE8 && attributes[5].Id == 0xE9;
}

bool IsSsdJMicron61x(const std::vector<SmartAttribute> &attributes) {
  if (attributes.size() < 13)
    return false;

  return attributes[0].Id == 0x01 && attributes[1].Id == 0x02 &&
         attributes[2].Id == 0x03 && attributes[3].Id == 0x05 &&
         attributes[4].Id == 0x07 && attributes[5].Id == 0x08 &&
         attributes[6].Id == 0x09 && attributes[7].Id == 0x0A &&
         attributes[8].Id == 0x0C && attributes[9].Id == 0xA8 &&
         attributes[10].Id == 0xAF && attributes[11].Id == 0xC0 &&
         attributes[12].Id == 0xC2;
}

bool IsSsdJMicron66x(const std::vector<SmartAttribute> &attributes,
                     const std::string &modelUpper) {
  if (attributes.size() < 15) {
    return modelUpper.find("ADATA SU700") == 0;
  }

  bool flagSmartType = attributes[0].Id == 0x01 && attributes[1].Id == 0x02 &&
                       attributes[2].Id == 0x03 && attributes[3].Id == 0x05 &&
                       attributes[4].Id == 0x07 && attributes[5].Id == 0x08 &&
                       attributes[6].Id == 0x09 && attributes[7].Id == 0x0A &&
                       attributes[8].Id == 0x0C && attributes[9].Id == 0xA7 &&
                       attributes[10].Id == 0xA8 && attributes[11].Id == 0xA9 &&
                       attributes[12].Id == 0xAA && attributes[13].Id == 0xAD &&
                       attributes[14].Id == 0xAF;

  if (!flagSmartType && modelUpper.find("ADATA SU700") == 0) {
    flagSmartType = true;
  }

  return flagSmartType;
}

bool IsSsdIndilinx(const std::vector<SmartAttribute> &attributes) {
  if (attributes.size() < 6)
    return false;

  return attributes[0].Id == 0x01 && attributes[1].Id == 0x09 &&
         attributes[2].Id == 0x0C && attributes[3].Id == 0xB8 &&
         attributes[4].Id == 0xC3 && attributes[5].Id == 0xC4;
}

bool IsSsdIntelDc(const std::string &modelUpper) {
  return modelUpper.find("INTEL SSDSCKHB") != std::string::npos;
}

bool IsSsdIntel(const std::vector<SmartAttribute> &attributes,
                const std::string &modelUpper) {
  bool flagSmartType = false;

  if (attributes.size() >= 5 && attributes[0].Id == 0x03 &&
      attributes[1].Id == 0x04 && attributes[2].Id == 0x05 &&
      attributes[3].Id == 0x09 && attributes[4].Id == 0x0C) {

    if (attributes.size() >= 8 && attributes[5].Id == 0xC0 &&
        attributes[6].Id == 0xE8 && attributes[7].Id == 0xE9) {
      flagSmartType = true;
    } else if (attributes.size() >= 7 && attributes[5].Id == 0xC0 &&
               attributes[6].Id == 0xE1) {
      flagSmartType = true;
    } else if (attributes.size() >= 8 && attributes[5].Id == 0xAA &&
               attributes[6].Id == 0xAB && attributes[7].Id == 0xAC) {
      flagSmartType = true;
    }
  }

  return (modelUpper.find("INTEL") != std::string::npos ||
          modelUpper.find("SOLIDIGM") != std::string::npos || flagSmartType);
}

bool IsSsdSamsung(const std::vector<SmartAttribute> &attributes,
                  const std::string &modelUpper, bool isSsd) {
  bool flagSmartType = false;

  if (attributes.size() >= 10 && attributes[0].Id == 0x05 &&
      attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
      attributes[3].Id == 0xAA && attributes[4].Id == 0xAB &&
      attributes[5].Id == 0xAC && attributes[6].Id == 0xAD &&
      attributes[7].Id == 0xAE && attributes[8].Id == 0xB2 &&
      attributes[9].Id == 0xB4) {
    flagSmartType = true;
  } else if (attributes.size() >= 5 && attributes[0].Id == 0x09 &&
             attributes[1].Id == 0x0C && attributes[2].Id == 0xB2 &&
             attributes[3].Id == 0xB3 && attributes[4].Id == 0xB4) {
    flagSmartType = true;
  } else if (attributes.size() >= 7 && attributes[0].Id == 0x09 &&
             attributes[1].Id == 0x0C && attributes[2].Id == 0xB1 &&
             attributes[3].Id == 0xB2 && attributes[4].Id == 0xB3 &&
             attributes[5].Id == 0xB4 && attributes[6].Id == 0xB7) {
    flagSmartType = true;
  } else if (attributes.size() >= 8 && attributes[0].Id == 0x09 &&
             attributes[1].Id == 0x0C && attributes[2].Id == 0xAF &&
             attributes[3].Id == 0xB0 && attributes[4].Id == 0xB1 &&
             attributes[5].Id == 0xB2 && attributes[6].Id == 0xB3 &&
             attributes[7].Id == 0xB4) {
    flagSmartType = true;
  } else if (attributes.size() >= 7 && attributes[0].Id == 0x05 &&
             attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
             attributes[3].Id == 0xB1 && attributes[4].Id == 0xB3 &&
             attributes[5].Id == 0xB5 && attributes[6].Id == 0xB6) {
    flagSmartType = true;
  }

  return ((modelUpper.find("SAMSUNG") != std::string::npos && isSsd) ||
          (modelUpper.find("MZ-") != std::string::npos && isSsd) ||
          flagSmartType);
}

bool IsSsdSandForce(const std::vector<SmartAttribute> &attributes,
                    const std::string &modelUpper) {
  bool flagSmartType = false;

  if (attributes.size() >= 7 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
      attributes[3].Id == 0x0C && attributes[4].Id == 0x0D &&
      attributes[5].Id == 0x64 && attributes[6].Id == 0xAA) {
    flagSmartType = true;
  }

  if (attributes.size() >= 6 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
      attributes[3].Id == 0x0C && attributes[4].Id == 0xAB &&
      attributes[5].Id == 0xAC) {
    flagSmartType = true;
  }

  if (attributes.size() >= 16 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x02 && attributes[2].Id == 0x03 &&
      attributes[3].Id == 0x05 && attributes[4].Id == 0x07 &&
      attributes[5].Id == 0x08 && attributes[6].Id == 0x09 &&
      attributes[7].Id == 0x0A && attributes[8].Id == 0x0C &&
      attributes[9].Id == 0xA7 && attributes[10].Id == 0xA8 &&
      attributes[11].Id == 0xA9 && attributes[12].Id == 0xAA &&
      attributes[13].Id == 0xAD && attributes[14].Id == 0xAF &&
      attributes[15].Id == 0xB1) {
    flagSmartType = true;
  }

  return (modelUpper.find("SandForce") != std::string::npos || flagSmartType);
}

bool IsSsdMicronMU03(const std::string &modelUpper,
                     const std::string &firmwareRev) {
  if ((modelUpper.find("MICRON_M600") == 0 ||
       modelUpper.find("MICRON M600") == 0 ||
       modelUpper.find("MICRON_M550") == 0 ||
       modelUpper.find("MICRON M550") == 0 ||
       modelUpper.find("MICRON_M510") == 0 ||
       modelUpper.find("MICRON M510") == 0 ||
       modelUpper.find("MICRON_M500") == 0 ||
       modelUpper.find("MICRON M500") == 0 ||
       modelUpper.find("MICRON_1300") == 0 ||
       modelUpper.find("MICRON 1300") == 0 ||
       modelUpper.find("MICRON_1100") == 0 ||
       modelUpper.find("MICRON 1100") == 0 || modelUpper.find("MTFDDA") == 0)) {
    return true;
  } else if ((modelUpper.find("M500SSD") != std::string::npos ||
              modelUpper.find("MX500SSD") != std::string::npos ||
              modelUpper.find("BX500SSD") != std::string::npos ||
              modelUpper.find("MX300SSD") != std::string::npos ||
              modelUpper.find("BX300SSD") != std::string::npos ||
              modelUpper.find("MX200SSD") != std::string::npos ||
              modelUpper.find("BX200SSD") != std::string::npos ||
              modelUpper.find("MX100SSD") != std::string::npos ||
              modelUpper.find("BX100SSD") != std::string::npos ||
              modelUpper.find("MTFD") == 0) &&
             firmwareRev.find("MU01") == std::string::npos) {
    return true;
  }

  return false;
}

bool IsSsdMicron(const std::vector<SmartAttribute> &attributes,
                 const std::string &modelUpper,
                 const std::string &firmwareRev) {
  bool flagSmartType = false;

  if (attributes.size() >= 11 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
      attributes[3].Id == 0x0C && attributes[4].Id == 0xAA &&
      attributes[5].Id == 0xAB && attributes[6].Id == 0xAC &&
      attributes[7].Id == 0xAD && attributes[8].Id == 0xAE &&
      attributes[9].Id == 0xB5 && attributes[10].Id == 0xB7) {
    flagSmartType = true;
  }

  return modelUpper.find("P600") == 0 || modelUpper.find("C600") == 0 ||
         modelUpper.find("M6-") == 0 || modelUpper.find("M600") == 0 ||
         modelUpper.find("P500") == 0 ||
         (modelUpper.find("C500") == 0 && firmwareRev.find("H") != 0) ||
         modelUpper.find("M5-") == 0 || modelUpper.find("M500") == 0 ||
         modelUpper.find("P400") == 0 || modelUpper.find("C400") == 0 ||
         modelUpper.find("M4-") == 0 || modelUpper.find("M400") == 0 ||
         modelUpper.find("P300") == 0 || modelUpper.find("C300") == 0 ||
         modelUpper.find("M3-") == 0 || modelUpper.find("M300") == 0 ||
         (modelUpper.find("CT") == 0 &&
          modelUpper.find("SSD") != std::string::npos) ||
         modelUpper.find("CRUCIAL") == 0 || modelUpper.find("MICRON") == 0 ||
         modelUpper.find("MTFD") == 0 || flagSmartType;
}

bool IsSsdOcz(const std::vector<SmartAttribute> &attributes,
              const std::string &modelUpper) {
  bool flagSmartType = false;

  if (modelUpper.find("OCZ-TRION") == 0) {
    flagSmartType = true;
  }

  if (attributes.size() >= 8 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x03 && attributes[2].Id == 0x04 &&
      attributes[3].Id == 0x05 && attributes[4].Id == 0x09 &&
      attributes[5].Id == 0x0C && attributes[6].Id == 0xE8 &&
      attributes[7].Id == 0xE9) {
    flagSmartType = true;
  }

  return (modelUpper.find("OCZ") == 0 && flagSmartType);
}

bool IsSsdOczVector(const std::vector<SmartAttribute> &attributes,
                    const std::string &modelUpper) {
  bool flagSmartType = false;

  if (modelUpper.find("RADEON R7") == 0) {
    return true;
  }

  if (attributes.size() >= 9 && attributes[0].Id == 0x05 &&
      attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
      attributes[3].Id == 0xAB && attributes[4].Id == 0xAE &&
      attributes[5].Id == 0xC3 && attributes[6].Id == 0xC4 &&
      attributes[7].Id == 0xC5 && attributes[8].Id == 0xC6) {
    flagSmartType = true;
  }

  if (modelUpper.find("PANASONIC RP-SSB") == 0) {
    flagSmartType = true;
  }

  return (modelUpper.find("OCZ") == 0 || flagSmartType);
}

bool IsSsdSsstc(const std::string &modelUpper) {
  return modelUpper.find("CV8-") != std::string::npos ||
         modelUpper.find("CVB-") != std::string::npos ||
         modelUpper.find("ER2-") != std::string::npos;
}

bool IsSsdPlextor(const std::vector<SmartAttribute> &attributes,
                  const std::string &modelUpper) {
  bool flagSmartType = false;

  if (attributes.size() >= 8 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
      attributes[3].Id == 0x0C && attributes[4].Id == 0xB1 &&
      attributes[5].Id == 0xB2 && attributes[6].Id == 0xB5 &&
      attributes[7].Id == 0xB6) {
    flagSmartType = true;
  }

  return modelUpper.find("PLEXTOR") == 0 || modelUpper.find("LITEON") == 0 ||
         modelUpper.find("CV6-CQ") == 0 ||
         modelUpper.find("CSSD-S6T128NM3PQ") == 0 ||
         modelUpper.find("CSSD-S6T256NM3PQ") == 0 || flagSmartType;
}

bool IsSsdSanDisk(const std::string &modelUpper) {
  return modelUpper.find("SanDisk") != std::string::npos ||
         modelUpper.find("SD Ultra") != std::string::npos ||
         modelUpper.find("SDLF1") != std::string::npos;
}

bool IsSsdKingston(const std::string &modelUpper) {
  return modelUpper.find("KINGSTON") != std::string::npos;
}

bool IsSsdCorsair(const std::string &modelUpper) {
  return modelUpper.find("Corsair") == 0;
}

bool IsSsdToshiba(const std::string &modelUpper, bool isSsd) {
  return modelUpper.find("TOSHIBA") != std::string::npos && isSsd;
}

bool IsSsdRealtek(const std::vector<SmartAttribute> &attributes) {
  if (attributes.size() < 10)
    return false;

  return attributes[0].Id == 0x01 && attributes[1].Id == 0x05 &&
         attributes[2].Id == 0x09 && attributes[3].Id == 0x0C &&
         attributes[4].Id == 0xA1 && attributes[5].Id == 0xA2 &&
         attributes[6].Id == 0xA3 && attributes[7].Id == 0xA4 &&
         attributes[8].Id == 0xA6 && attributes[9].Id == 0xA7;
}

bool IsSsdSKhynix(const std::string &modelUpper) {
  return modelUpper.find("SK hynix") != std::string::npos ||
         modelUpper.find("HFS") == 0 || modelUpper.find("SHG") == 0;
}

bool IsSsdKioxia(const std::string &modelUpper) {
  return modelUpper.find("KIOXIA") != std::string::npos;
}

bool IsSsdApacer(const std::string &modelUpper,
                 const std::string &firmwareRev) {
  return modelUpper.find("Apacer") == 0 || modelUpper.find("ZADAK") == 0 ||
         firmwareRev.find("AP") == 0 || firmwareRev.find("SF") == 0 ||
         firmwareRev.find("PN") == 0;
}

bool IsSsdYmtc(const std::string &modelUpper) {
  return modelUpper.find("ZHITAI") != std::string::npos;
}

bool IsSsdScy(const std::string &modelUpper) {
  return modelUpper.find("SCY") == 0;
}

bool IsSsdRecadata(const std::string &modelUpper) {
  return modelUpper.find("RECADATA") == 0;
}

bool IsSsdSiliconMotionCVC(const std::string &modelUpper) {
  return modelUpper.find("CVC-") != std::string::npos;
}

bool IsSsdSiliconMotion(const std::vector<SmartAttribute> &attributes,
                        const std::string &modelUpper,
                        const std::string &firmwareRev,
                        const BYTE *smartReadData) {
  bool flagSmartType = false;

  if (attributes.size() >= 20 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
      attributes[3].Id == 0x0C && attributes[4].Id == 0xA0 &&
      attributes[5].Id == 0xA1 && attributes[6].Id == 0xA3 &&
      attributes[7].Id == 0xA4 && attributes[8].Id == 0xA5 &&
      attributes[9].Id == 0xA6 && attributes[10].Id == 0xA7 &&
      attributes[11].Id == 0xA8 && attributes[12].Id == 0xA9 &&
      attributes[13].Id == 0xAF && attributes[14].Id == 0xB0 &&
      attributes[15].Id == 0xB1 && attributes[16].Id == 0xB2 &&
      attributes[17].Id == 0xB5 && attributes[18].Id == 0xB6 &&
      attributes[19].Id == 0xC0) {
    flagSmartType = true;
  } else if (attributes.size() >= 20 && attributes[0].Id == 0x01 &&
             attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
             attributes[3].Id == 0x0C && attributes[4].Id == 0xA0 &&
             attributes[5].Id == 0xA1 && attributes[6].Id == 0xA3 &&
             attributes[7].Id == 0xA4 && attributes[8].Id == 0xA5 &&
             attributes[9].Id == 0xA6 && attributes[10].Id == 0xA7 &&
             attributes[11].Id == 0x94 && attributes[12].Id == 0x95 &&
             attributes[13].Id == 0x96 && attributes[14].Id == 0x97 &&
             attributes[15].Id == 0xA9 && attributes[16].Id == 0xB1 &&
             attributes[17].Id == 0xB5 && attributes[18].Id == 0xB6 &&
             attributes[19].Id == 0xBB) {
    flagSmartType = true;
  } else if (attributes.size() >= 11 && attributes[0].Id == 0x01 &&
             attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
             attributes[3].Id == 0x0C && attributes[4].Id == 0x94 &&
             attributes[5].Id == 0x95 && attributes[6].Id == 0x96 &&
             attributes[7].Id == 0x97 && attributes[8].Id == 0x9F &&
             attributes[9].Id == 0xA0 && attributes[10].Id == 0xA1) {
    flagSmartType = true;
  } else if (attributes.size() >= 11 && attributes[0].Id == 0x01 &&
             attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
             attributes[3].Id == 0x0C && attributes[4].Id == 0xA0 &&
             attributes[5].Id == 0xA1 && attributes[6].Id == 0xA3 &&
             attributes[7].Id == 0xA4 && attributes[8].Id == 0xA5 &&
             attributes[9].Id == 0xA6 && attributes[10].Id == 0xA7) {
    flagSmartType = true;
  } else if (attributes.size() >= 11 && attributes[0].Id == 0x01 &&
             attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
             attributes[3].Id == 0x0C && attributes[4].Id == 0xA0 &&
             attributes[5].Id == 0xA1 && attributes[6].Id == 0xA3 &&
             attributes[7].Id == 0x94 && attributes[8].Id == 0x95 &&
             attributes[9].Id == 0x96 && attributes[10].Id == 0x97) {
    flagSmartType = true;
  } else if (modelUpper.find("TS") == 0) {
    if (smartReadData &&
        ((smartReadData[400] == 'T' && smartReadData[401] == 'S') ||
         (smartReadData[400] == 'S' && smartReadData[401] == 'M'))) {
      flagSmartType = true;
    }
  } else if (modelUpper.find("ADATA SX950") == 0) {
    flagSmartType = true;
  }

  return flagSmartType;
}

bool IsSsdPhison(const std::vector<SmartAttribute> &attributes,
                 const std::string &modelUpper,
                 const std::string &firmwareRev) {
  bool flagSmartType = false;

  if (attributes.size() >= 11 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
      attributes[3].Id == 0xA8 && attributes[4].Id == 0xAA &&
      attributes[5].Id == 0xAD && attributes[6].Id == 0xC0 &&
      attributes[7].Id == 0xC2 && attributes[8].Id == 0xDA &&
      attributes[9].Id == 0xE7 && attributes[10].Id == 0xF1) {
    flagSmartType = true;
  } else if (attributes.size() >= 10 && attributes[0].Id == 0x01 &&
             attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
             attributes[3].Id == 0xA8 && attributes[4].Id == 0xAA &&
             attributes[5].Id == 0xAD && attributes[6].Id == 0xC0 &&
             attributes[7].Id == 0xDA && attributes[8].Id == 0xE7 &&
             attributes[9].Id == 0xF1) {
    flagSmartType = true;
  }

  return flagSmartType;
}

bool IsSsdWdc(const std::string &modelUpper) {
  return modelUpper.find("WDC ") == 0 || modelUpper.find("WD ") == 0;
}

bool IsSsdSeagate(const std::vector<SmartAttribute> &attributes,
                  const std::string &modelUpper) {
  bool flagSmartType = false;

  if (attributes.size() >= 15 && attributes[0].Id == 0x01 &&
      attributes[1].Id == 0x05 && attributes[2].Id == 0x09 &&
      attributes[3].Id == 0x0C && attributes[4].Id == 0x64 &&
      attributes[5].Id == 0x66 && attributes[6].Id == 0x67 &&
      attributes[7].Id == 0xAA && attributes[8].Id == 0xAB &&
      attributes[9].Id == 0xAC && attributes[10].Id == 0xAD &&
      attributes[11].Id == 0xAE && attributes[12].Id == 0xB1 &&
      attributes[13].Id == 0xB7 && attributes[14].Id == 0xBB) {
    flagSmartType = true;
  } else if (attributes.size() >= 19 && attributes[0].Id == 0x01 &&
             attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
             attributes[3].Id == 0x10 && attributes[4].Id == 0x11 &&
             attributes[5].Id == 0xA8 && attributes[6].Id == 0xAA &&
             attributes[7].Id == 0xAD && attributes[8].Id == 0xAE &&
             attributes[9].Id == 0xB1 && attributes[10].Id == 0xC0 &&
             attributes[11].Id == 0xC2 && attributes[12].Id == 0xDA &&
             attributes[13].Id == 0xE7 && attributes[14].Id == 0xE8 &&
             attributes[15].Id == 0xE9 && attributes[16].Id == 0xEB &&
             attributes[17].Id == 0xF1 && attributes[18].Id == 0xF2) {
    flagSmartType = true;
  } else if (modelUpper.find("Seagate") == 0 ||
             (modelUpper.find("STT") != 0 && modelUpper.find("ST") == 0) ||
             (modelUpper.find("ZA") == 0)) {
    flagSmartType = true;
  }

  return flagSmartType;
}

bool IsSsdMarvell(const std::vector<SmartAttribute> &attributes,
                  const std::string &modelUpper,
                  const std::string &firmwareRev) {
  bool flagSmartType = false;

  if (attributes.size() >= 8 && attributes[0].Id == 0x05 &&
      attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
      attributes[3].Id == 0xA1 && attributes[4].Id == 0xA4 &&
      attributes[5].Id == 0xA5 && attributes[6].Id == 0xA6 &&
      attributes[7].Id == 0xA7) {
    flagSmartType = true;
  } else if (attributes.size() >= 7 && attributes[0].Id == 0x05 &&
             attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
             attributes[3].Id == 0xA4 && attributes[4].Id == 0xA5 &&
             attributes[5].Id == 0xA6 && attributes[6].Id == 0xA7) {
    flagSmartType = true;
  }

  // Exclude HANYE-Q55
  if (modelUpper.find("HANYE-Q55") == 0) {
    return false;
  }

  return flagSmartType;
}

bool IsSsdMaxiotek(const std::vector<SmartAttribute> &attributes,
                   const std::string &modelUpper) {
  bool flagSmartType = false;

  if (modelUpper.find("MAXIO") == 0) {
    flagSmartType = true;
  } else if (modelUpper.find("CUSO C5S-EVO") == 0) {
    flagSmartType = true;
  } else if (modelUpper.find("HANYE-Q55") == 0 && attributes.size() >= 7 &&
             attributes[0].Id == 0x05 && attributes[1].Id == 0x09 &&
             attributes[2].Id == 0x0C && attributes[3].Id == 0xA4 &&
             attributes[4].Id == 0xA5 && attributes[5].Id == 0xA6 &&
             attributes[6].Id == 0xA7) {
    flagSmartType = true;
  } else if (attributes.size() >= 6 && attributes[0].Id == 0x05 &&
             attributes[1].Id == 0x09 && attributes[2].Id == 0x0C &&
             attributes[3].Id == 0xA7 && attributes[4].Id == 0xA8 &&
             attributes[5].Id == 0xA9) {
    flagSmartType = true;
  }

  return flagSmartType;
}

bool IsSsdAdataIndustrial(const std::string &modelUpper) {
  return modelUpper.find("ADATA_IM2S") == 0 ||
         modelUpper.find("ADATA_IMSS") == 0 ||
         modelUpper.find("ADATA_ISSS") == 0 || modelUpper.find("IM2S") == 0 ||
         modelUpper.find("IMSS") == 0 || modelUpper.find("ISSS") == 0;
}

// SSD Type Enumeration
enum class SSDType {
  HDD_GENERAL = 0,
  ADATA_INDUSTRIAL,
  SANDISK,
  WDC,
  SEAGATE,
  MTRON,
  TOSHIBA,
  JMICRON_66X,
  JMICRON_61X,
  JMICRON_60X,
  INDILINX,
  INTEL_DC,
  INTEL,
  SAMSUNG,
  MICRON_MU03,
  MICRON,
  SANDFORCE,
  OCZ,
  OCZ_VECTOR,
  SSSTC,
  PLEXTOR,
  KINGSTON,
  CORSAIR,
  REALTEK,
  SK_HYNIX,
  KIOXIA,
  SILICON_MOTION_CVC,
  SILICON_MOTION,
  PHISON,
  MARVELL,
  MAXIOTEK,
  APACER,
  YMTC,
  SCY,
  RECADATA,
  GENERAL_SSD
};

// Function to detect SSD type
SSDType DetectSSDType(const SMARTInfoS &info,
                      const BYTE *smartReadData = nullptr) {
  const auto &attrs = info.attributes;
  const auto &model = info.modelUpper;
  const auto &firmware = info.firmwareRev;
  bool isSsd = info.IsSSD;
  int attrCount = static_cast<int>(attrs.size());

  // If not SSD, return HDD
  if (!isSsd) {
    return SSDType::HDD_GENERAL;
  }
  // Check in exact CrystalDiskInfo order
  else if (IsSsdAdataIndustrial(model)) {
    return SSDType::ADATA_INDUSTRIAL;
  } else if (IsSsdSanDisk(model)) {
    return SSDType::SANDISK;
  } else if (IsSsdWdc(model)) {
    return SSDType::WDC;
  } else if (IsSsdSeagate(attrs, model)) {
    return SSDType::SEAGATE;
  } else if (IsSsdMtron(attrs, model, attrCount)) {
    return SSDType::MTRON;
  } else if (IsSsdToshiba(model, isSsd)) {
    return SSDType::TOSHIBA;
  } else if (IsSsdJMicron66x(attrs, model)) {
    return SSDType::JMICRON_66X;
  } else if (IsSsdJMicron61x(attrs)) {
    return SSDType::JMICRON_61X;
  } else if (IsSsdJMicron60x(attrs)) {
    return SSDType::JMICRON_60X;
  } else if (IsSsdIndilinx(attrs)) {
    return SSDType::INDILINX;
  } else if (IsSsdIntelDc(model)) {
    return SSDType::INTEL_DC;
  } else if (IsSsdIntel(attrs, model)) {
    return SSDType::INTEL;
  } else if (IsSsdSamsung(attrs, model, isSsd)) {
    return SSDType::SAMSUNG;
  } else if (IsSsdMicronMU03(model, firmware)) {
    return SSDType::MICRON_MU03;
  } else if (IsSsdMicron(attrs, model, firmware)) {
    return SSDType::MICRON;
  } else if (IsSsdSandForce(attrs, model)) {
    return SSDType::SANDFORCE;
  } else if (IsSsdOcz(attrs, model)) {
    return SSDType::OCZ;
  } else if (IsSsdOczVector(attrs, model)) {
    return SSDType::OCZ_VECTOR;
  } else if (IsSsdSsstc(model)) {
    return SSDType::SSSTC;
  } else if (IsSsdPlextor(attrs, model)) {
    return SSDType::PLEXTOR;
  } else if (IsSsdKingston(model)) {
    return SSDType::KINGSTON;
  } else if (IsSsdCorsair(model)) {
    return SSDType::CORSAIR;
  } else if (IsSsdRealtek(attrs)) {
    return SSDType::REALTEK;
  } else if (IsSsdSKhynix(model)) {
    return SSDType::SK_HYNIX;
  } else if (IsSsdKioxia(model)) {
    return SSDType::KIOXIA;
  } else if (IsSsdSiliconMotionCVC(model)) {
    return SSDType::SILICON_MOTION_CVC;
  } else if (IsSsdSiliconMotion(attrs, model, firmware, smartReadData)) {
    return SSDType::SILICON_MOTION;
  } else if (IsSsdPhison(attrs, model, firmware)) {
    return SSDType::PHISON;
  } else if (IsSsdMarvell(attrs, model, firmware)) {
    return SSDType::MARVELL;
  } else if (IsSsdMaxiotek(attrs, model)) {
    return SSDType::MAXIOTEK;
  } else if (IsSsdApacer(model, firmware)) {
    return SSDType::APACER;
  } else if (IsSsdYmtc(model)) {
    return SSDType::YMTC;
  } else if (IsSsdScy(model)) {
    return SSDType::SCY;
  } else if (IsSsdRecadata(model)) {
    return SSDType::RECADATA;
  } else if (isSsd) {
    return SSDType::GENERAL_SSD;
  }

  // Fallback
  return SSDType::GENERAL_SSD;
}

// Helper function to convert SSDType to string
const char *SSDTypeToString(SSDType type) {
  switch (type) {
  case SSDType::HDD_GENERAL:
    return "HDD";
  case SSDType::ADATA_INDUSTRIAL:
    return "ADATA Industrial";
  case SSDType::SANDISK:
    return "SanDisk";
  case SSDType::WDC:
    return "WDC";
  case SSDType::SEAGATE:
    return "Seagate";
  case SSDType::MTRON:
    return "Mtron";
  case SSDType::TOSHIBA:
    return "Toshiba";
  case SSDType::JMICRON_66X:
    return "JMicron 66x";
  case SSDType::JMICRON_61X:
    return "JMicron 61x";
  case SSDType::JMICRON_60X:
    return "JMicron 60x";
  case SSDType::INDILINX:
    return "Indilinx";
  case SSDType::INTEL_DC:
    return "Intel DC";
  case SSDType::INTEL:
    return "Intel";
  case SSDType::SAMSUNG:
    return "Samsung";
  case SSDType::MICRON_MU03:
    return "Micron MU03";
  case SSDType::MICRON:
    return "Micron";
  case SSDType::SANDFORCE:
    return "SandForce";
  case SSDType::OCZ:
    return "OCZ";
  case SSDType::OCZ_VECTOR:
    return "OCZ Vector";
  case SSDType::SSSTC:
    return "SSSTC";
  case SSDType::PLEXTOR:
    return "Plextor";
  case SSDType::KINGSTON:
    return "Kingston";
  case SSDType::CORSAIR:
    return "Corsair";
  case SSDType::REALTEK:
    return "Realtek";
  case SSDType::SK_HYNIX:
    return "SK Hynix";
  case SSDType::KIOXIA:
    return "Kioxia";
  case SSDType::SILICON_MOTION_CVC:
    return "Silicon Motion CVC";
  case SSDType::SILICON_MOTION:
    return "Silicon Motion";
  case SSDType::PHISON:
    return "Phison";
  case SSDType::MARVELL:
    return "Marvell";
  case SSDType::MAXIOTEK:
    return "Maxiotek";
  case SSDType::APACER:
    return "Apacer";
  case SSDType::YMTC:
    return "YMTC";
  case SSDType::SCY:
    return "SCY";
  case SSDType::RECADATA:
    return "Recadata";
  case SSDType::GENERAL_SSD:
    return "General SSD";
  default:
    return "Unknown";
  }
}

// From
// https://github.com/hiyohiyo/CrystalDiskInfo/blob/master/Language/English.lang
const char *GetAttributeNameByIDAndType(SSDType type, unsigned char id) {
  if (type == SSDType::HDD_GENERAL) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x02:
      return "Throughput Performance";
    case 0x03:
      return "Spin-Up Time";
    case 0x04:
      return "Start/Stop Count";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x06:
      return "Read Channel Margin";
    case 0x07:
      return "Seek Error Rate";
    case 0x08:
      return "Seek Time Performance";
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count";
    case 0x0B:
      return "Recalibration Retries";
    case 0x0C:
      return "Power Cycle Count";
    case 0x0D:
      return "Soft Read Error Rate stab";
    case 0x16:
      return "Current Helium Level";
    case 0xB8:
      return "End-to-End Error";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xBC:
      return "Command Timeout";
    case 0xBD:
      return "High Fly Writes";
    case 0xBE:
      return "Airflow Temperature";
    case 0xBF:
      return "G-Sense Error Rate";
    case 0xC0:
      return "Power-off Retract Count";
    case 0xC1:
      return "Load/Unload Cycle Count";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "Hardware ECC recovered";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xC7:
      return "UltraDMA CRC Error Count";
    case 0xC8:
      return "Write Error Rate";
    case 0xC9:
      return "Soft Read Error Rate";
    case 0xCA:
      return "Data Address Mark Error";
    case 0xCB:
      return "Run Out Cancel";
    case 0xCC:
      return "Soft ECC Correction";
    case 0xCD:
      return "Thermal Asperity Rate";
    case 0xCE:
      return "Flying Height";
    case 0xCF:
      return "Spin High Current";
    case 0xD0:
      return "Spin Buzz";
    case 0xD1:
      return "Offline Seek Performance";
    case 0xD3:
      return "Vibration During Write";
    case 0xD4:
      return "Shock During Write";
    case 0xDC:
      return "Disk Shift";
    case 0xDD:
      return "G-Sense Error Rate";
    case 0xDE:
      return "Loaded Hours";
    case 0xDF:
      return "Load/Unload Retry Count";
    case 0xE0:
      return "Load Friction";
    case 0xE1:
      return "Load/Unload Cycle Count";
    case 0xE2:
      return "Load 'In'-time";
    case 0xE3:
      return "Torque Amplification Count";
    case 0xE4:
      return "Power-Off Retract Cycle";
    case 0xE6:
      return "GMR Head Amplitude";
    case 0xE7:
      return "Temperature";
    case 0xF0:
      return "Head Flying Hours";
    case 0xFA:
      return "Read Error Retry Rate";
    case 0xFE:
      return "Free Fall Protection";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::GENERAL_SSD) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x02:
      return "Throughput Performance";
    case 0x03:
      return "Spin-Up Time";
    case 0x04:
      return "Start/Stop Count";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x06:
      return "Read Channel Margin";
    case 0x07:
      return "Seek Error Rate";
    case 0x08:
      return "Seek Time Performance";
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count";
    case 0x0B:
      return "Recalibration Retries";
    case 0x0C:
      return "Power Cycle Count";
    case 0x0D:
      return "Soft Read Error Rate stab";
    case 0xC0:
      return "Unsafe Shutdown Count";
    case 0xC2:
      return "Temperature";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xFF:
      return "Remaining Life";
    case 0xF1:
      return "Total Host Writes";
    case 0xF2:
      return "Total Host Reads";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::ADATA_INDUSTRIAL) {
    switch (id) {
    case 0x09:
      return "Power-On Hours Count";
    case 0x0C:
      return "Drive Power Cycle Count";
    case 0xA7:
      return "SSD Protect Mode";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xA9:
      return "Bad Block Count";
    case 0xAD:
      return "Erase Count";
    case 0xAF:
      return "Bad Cluster Table Count";
    case 0xB4:
      return "Spare Block Count Left";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xE7:
      return "SSD Life Left";
    case 0xE9:
      return "Flash Write Sector Count";
    case 0xEA:
      return "Flash Read Sector Count";
    case 0xF1:
      return "Write Sector Count";
    case 0xF2:
      return "Read Sector Count";
    default:
      return "Unknown Attribute";
    }
  } else if (type == SSDType::SANDISK) {
    switch (id) {
    case 0x05:
      return "Reassigned Block Count";
    case 0x09:
      return "Power On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xAA:
      return "Reserve Block Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Average Write/Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xC2:
      return "Temperature";
    case 0xE6:
      return "Percent of Total Write/Erase Count";
    case 0xE8:
      return "Spare Blocks Remaining";
    case 0xEA:
      return "Percent of Total Write/Erase Count (BC blocks)";
    case 0xF1:
      return "Total Host Writes";
    case 0xF2:
      return "Total Host Reads";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::WDC) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x05:
      return "Reassigned Block Count";
    case 0x09:
      return "Power On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA5:
      return "Block Erase Count (SLC)";
    case 0xA6:
      return "Minimum P/E Cycles";
    case 0xA7:
      return "Maximum Bad Blocks per Die";
    case 0xA8:
      return "Maximum P/E Cycles";
    case 0xA9:
      return "Total Bad Block";
    case 0xAA:
      return "Grown Bad Blocks";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Average P/E Cycles";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xB8:
      return "End-to-End Error Detection/Correction Count";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xBC:
      return "Command Timeout Count";
    case 0xC2:
      return "Temperature";
    case 0xC7:
      return "CRC Error Count";
    case 0xD4:
      return "SATA PHY Error";
    case 0xE6:
      return "Media Wearout Indicator";
    case 0xE8:
      return "Available Reserve Space";
    case 0xE9:
      return "NAND GB Written";
    case 0xEA:
      return "NAND GB Written (SLC)";
    case 0xEE:
      return "Media Wearout Indicator (Cycles Remaining)";
    case 0xF1:
      return "Total GB Written";
    case 0xF2:
      return "Total GB Read";
    case 0xF3:
      return "Total NAND Writes";
    case 0xF4:
      return "Temperature Throttle Status";
    case 0xF9:
      return "NAND GB Written";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SEAGATE) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x05:
      return "Retired Sectors Count";
    case 0x09:
      return "Power On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0x10:
      return "Spare Blocks Available";
    case 0x11:
      return "Remaining Spare Blocks";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xAA:
      return "Bad Block Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xB1:
      return "Wear Range Delta";
    case 0xB5:
      return "Program Fail Count";
    case 0xB6:
      return "Erase Fail Count";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xC9:
      return "Soft Error Rate";
    case 0xCC:
      return "Soft ECC Correction Rate";
    case 0xDA:
      return "CRC Error Count";
    case 0xE7:
      return "SSD Life Left";
    case 0xE8:
      return "Read Failure Block Count";
    case 0xE9:
      return "NAND GiB Written";
    case 0xEA:
      return "Lifetime Writes To Flash";
    case 0xEB:
      return "NAND Sectors Written";
    case 0xF1:
      return "Lifetime Writes From Host";
    case 0xF2:
      return "Lifetime Reads From Host";
    case 0xFA:
      return "Lifetime NAND Read Retries";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::MTRON) {
    switch (id) {
    case 0xBB:
      return "Total Erase Count";
    default:
      return "Unknown Attribute";
    }
  } else if (type == SSDType::TOSHIBA) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x02:
      return "Throughput Performance";
    case 0x03:
      return "Spin-Up Time";
    case 0x04:
      return "Start/Stop Count";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x07:
      return "Seek Error Rate";
    case 0x08:
      return "Seek Time Performance";
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count";
    case 0x0B:
      return "Recalibration Retries";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA7:
      return "SSD Protect Mode";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xA9:
      return "Total Bad Block Count";
    case 0xAD:
      return "Erase Count";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xF1:
      return "Total Host Writes";
    case 0xF2:
      return "Total Host Reads";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::JMICRON_66X) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x02:
      return "Throughput Performance";
    case 0x03:
      return "Spin-Up Time";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x07:
      return "Seek Error Rate";
    case 0x08:
      return "Seek Time Performance";
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA7:
      return "SSD Protect Mode";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xAA:
      return "Bad Block Count";
    case 0xAD:
      return "Erase Count";
    case 0xAF:
      return "Bad Cluster Table Count";
    case 0xB4:
      return "Spare Block Count";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xE7:
      return "Drive Life";
    case 0xE9:
      return "Write Sector Count (NAND)";
    case 0xF0:
      return "Write Head";
    case 0xF1:
      return "Total LBA Written";
    case 0xF2:
      return "Total LBA Read";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::JMICRON_61X) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x02:
      return "Throughput Performance";
    case 0x03:
      return "Spin-Up Time";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x07:
      return "Seek Error Rate";
    case 0x08:
      return "Seek Time Performance";
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xAF:
      return "Bad Cluster Table Count (ECC Fail Count)";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xAA:
      return "Bad Block Count";
    case 0xAD:
      return "Erase Count";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::JMICRON_60X) {
    switch (id) {
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xC2:
      return "Temperature";
    case 0xE5:
      return "Halt System ID, Flash ID";
    case 0xE8:
      return "Firmware Version Information";
    case 0xE9:
      return "ECC Fail Record";
    case 0xEA:
      return "Average Erase Count, Max Erase Count";
    case 0xEB:
      return "Good Block Count, System Block Count";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::INDILINX) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xB8:
      return "Initial Bad Block Count";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "Program Failure Block Count";
    case 0xC4:
      return "Erase Failure Block Count";
    case 0xC5:
      return "Read Failure Block Count";
    case 0xC6:
      return "Total Count of Read Sectors";
    case 0xC7:
      return "Total Count of Write Sectors";
    case 0xC8:
      return "Total Count of Read Commands";
    case 0xC9:
      return "Total Count of Write Commands";
    case 0xCA:
      return "Total Count of Error bits from flash";
    case 0xCB:
      return "Total Count of Read Sectors with Correctable Bit Errors";
    case 0xCC:
      return "Bad Block Full Flag";
    case 0xCD:
      return "Maximum PE Count Specification";
    case 0xCE:
      return "Minimum Erase Count";
    case 0xCF:
      return "Maximum Erase Count";
    case 0xD0:
      return "Average Erase Count";
    case 0xD1:
      return "Remaining Drive Life";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::INTEL_DC) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Count";
    case 0x05:
      return "Reallocated Flash Block Count";
    case 0x09:
      return "Power-On Hours Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0x0E:
      return "Device Capacity";
    case 0x0F:
      return "User Capacity";
    case 0x10:
      return "Spare Blocks Available";
    case 0x11:
      return "Remaining Spare Available";
    case 0xAC:
      return "Total Block Erase Failure";
    case 0xAD:
      return "Per Block Max Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xAF:
      return "Average Erase Count";
    case 0xB0:
      return "Power Loss Protection Error";
    case 0xB5:
      return "Total Block Program Failure";
    case 0xB8:
      return "End-to-End Error Detection Count";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xC2:
      return "Temperature";
    case 0xC5:
      return "Pending Sector Count";
    case 0xC6:
      return "Offline Uncorrectable Error Count";
    case 0xC7:
      return "CRC Error Count";
    case 0xCA:
      return "Percentage of Drive Life Used";
    case 0xE2:
      return "Timed Workload - Media Wear";
    case 0xE3:
      return "Timed Workload - Host Read/Write Ratio";
    case 0xE4:
      return "Timed Workload Timer";
    case 0xEA:
      return "Thermal Throttle Status";
    case 0xEB:
      return "Total Host Bytes/Sectors Written";
    case 0xF1:
      return "Total Nand Bytes/Sectors Written";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::INTEL) {
    switch (id) {
    case 0x03:
      return "Spin Up Time";
    case 0x04:
      return "Start/Stop Count";
    case 0x05:
      return "Re-Allocated Sector Count";
    case 0x09:
      return "Power-On Hours Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xAA:
      return "Available Reserved Space";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAE:
      return "Unexpected Power Loss";
    case 0xAF:
      return "Power Loss Protection Failure";
    case 0xB7:
      return "SATA Downshift Count";
    case 0xB8:
      return "End to End Error Detection Count";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xBE:
      return "Temperature";
    case 0xC0:
      return "Unsafe Shutdown Count";
    case 0xC2:
      return "Temperature";
    case 0xC5:
      return "Pending Sector Count";
    case 0xC7:
      return "CRC Error Count";
    case 0xE1:
      return "Host Writes";
    case 0xE2:
      return "Timed Workload Media Wear";
    case 0xE3:
      return "Timed Workload Host Read/Write Ratio";
    case 0xE4:
      return "Timed Workload Timer";
    case 0xE8:
      return "Available Reserved Space";
    case 0xE9:
      return "Media Wearout Indicator";
    case 0xEA:
      return "Thermal Throttle Status";
    case 0xF1:
      return "Total LBAs Written";
    case 0xF2:
      return "Total LBAs Read";
    case 0xF3:
      return "Total NAND Writes";
    case 0xF9:
      return "Total NAND Writes";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SAMSUNG) {
    switch (id) {
    case 0x05:
      return "Reallocated Sector Count";
    case 0x09:
      return "Power-on Hours";
    case 0x0C:
      return "Power-on Count";
    case 0xAF:
      return "Program Fail Count (Chip)";
    case 0xB0:
      return "Erase Fail Count (Chip)";
    case 0xB1:
      return "Wear Leveling Count";
    case 0xB2:
      return "Used Reserved Block Count (Chip)";
    case 0xB3:
      return "Used Reserved Block Count (Total)";
    case 0xB4:
      return "Unused Reserved Block Count (Total)";
    case 0xB5:
      return "Program Fail Count (Total)";
    case 0xB6:
      return "Erase Fail Count (Total)";
    case 0xB7:
      return "Runtime Bad Block (Total)";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xBE:
      return "Airflow Temperature";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "ECC Error Rate";
    case 0xC6:
      return "Off-Line Uncorrectable Error Count";
    case 0xC7:
      return "CRC Error Count";
    case 0xC9:
      return "Super cap Status";
    case 0xCA:
      return "SSD Mode Status";
    case 0xEB:
      return "POR Recovery Count";
    case 0xF1:
      return "Total LBAs Written";
    case 0xF2:
      return "Total LBAs Read";
    case 0xAA:
      return "Unused Reserved Block Count (Chip)";
    case 0xAB:
      return "Program Fail Count (Chip)";
    case 0xAC:
      return "Erase Fail Count (Chip)";
    case 0xAD:
      return "Wear Leveling Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xB8:
      return "Error Detection";
    case 0xE9:
      return "Normalized Media Wear-out";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::MICRON_MU03) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Rate";
    case 0x05:
      return "Reallocated NAND Block Count";
    case 0x09:
      return "Power On Hours Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Average Block Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xB4:
      return "Unused Spare NAND Blocks";
    case 0xB5:
      return "Non-4K Aligned Access";
    case 0xB7:
      return "SATA interface Downshift";
    case 0xB8:
      return "Error Correction Count";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xC2:
      return "Temperature";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending ECC Count";
    case 0xC6:
      return "SMART Offline Scan Uncorrectable Error Count";
    case 0xC7:
      return "UDMA CRC Error Count";
    case 0xCA:
      return "Lifetime Remaining";
    case 0xCE:
      return "Write Error Rate";
    case 0xD2:
      return "Successful RAIN Recovery Count";
    case 0xF0:
      return "Host Program Page Count";
    case 0xF1:
      return "Total LBAs Written";
    case 0xF2:
      return "Total LBAs Read";
    case 0xF3:
      return "Total Program Page Count";
    case 0xF6:
      return "Cumulative Host Sectors Written";
    case 0xF7:
      return "Host Program NAND Pages Count";
    case 0xF8:
      return "FTL Program NAND Pages Count";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::MICRON) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Rate";
    case 0x05:
      return "Reallocated NAND Blocks";
    case 0x09:
      return "Power On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0x0D:
      return "Soft Error Rate";
    case 0x0E:
      return "Device Capacity (NAND)";
    case 0x0F:
      return "User Capacity";
    case 0x10:
      return "Spare Blocks Available";
    case 0x11:
      return "Remaining Spare Blocks";
    case 0x64:
      return "Total Erase Count";
    case 0xAA:
      return "Reserved Block Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Average Block-Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xB4:
      return "Unused Reserve NAND Blocks";
    case 0xB5:
      return "Unaligned Access Count";
    case 0xB7:
      return "SATA Interface Downshift";
    case 0xB8:
      return "Error Correction Count";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xBC:
      return "Command Timeout Count";
    case 0xBD:
      return "Factory Bad Block Count";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "Cumulative ECC Bit Correction Count";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC6:
      return "Smart Off-line Scan Uncorrectable Error Count";
    case 0xC7:
      return "Ultra DMA CRC Error Rate";
    case 0xCA:
      return "Percent Lifetime Used";
    case 0xCE:
      return "Write Error Rate";
    case 0xD2:
      return "Successful RAIN Recovery Count";
    case 0xEA:
      return "Total Bytes Read";
    case 0xF2:
      return "Write Protect Progress";
    case 0xF3:
      return "ECC Bits Corrected";
    case 0xF4:
      return "ECC Cumulative Threshold Events";
    case 0xF5:
      return "Cumulative Program NAND Pages";
    case 0xF6:
      return "Total Host Sector Writes";
    case 0xF7:
      return "Host Program Page Count";
    case 0xF8:
      return "Background Program Page Count";
    case 0xF9:
      return "Total Refresh ISP Count";
    case 0xFA:
      return "Total Do RefCal Count";
    case 0xFB:
      return "Total NAND Read Plane Count (Low 4Bytes)";
    case 0xFC:
      return "Total NAND Read Plane Count (High 4Bytes)";
    case 0xFD:
      return "Total Block ReMap Pass Count";
    case 0xFE:
      return "Total Background Scan Over Limit Count";
    case 0xDF:
      return "Total Background Scan";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SANDFORCE) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Rate";
    case 0x05:
      return "Retired Block Count";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0x0D:
      return "Soft Read Error Rate";
    case 0x64:
      return "Gigabytes Erased";
    case 0xAA:
      return "Reserve Block Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xB1:
      return "Wear Range Delta";
    case 0xB5:
      return "Program Fail Count";
    case 0xB6:
      return "Erase Fail Count";
    case 0xB8:
      return "Reported I/O Error Detection Code Errors";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "On-the-Fly ECC Uncorrectable Error Count";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xC7:
      return "SATA R-Errors Error Count";
    case 0xC9:
      return "Uncorrectable Soft Read Error Rate";
    case 0xCC:
      return "Soft ECC Correction Rate";
    case 0xE6:
      return "Drive Life Protection Status";
    case 0xE7:
      return "SSD Life Left";
    case 0xE8:
      return "Available Reserved Space";
    case 0xEB:
      return "Power Fail Backup Health";
    case 0xF1:
      return "Lifetime Writes from Host";
    case 0xF2:
      return "Lifetime Reads from Host";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::OCZ) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Rate";
    case 0x03:
      return "Spin Up Time";
    case 0x04:
      return "Start Stop Count";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xC2:
      return "Temperature";
    case 0xE8:
      return "Total Count of Write Sectors";
    case 0xE9:
      return "Remaining Life";
    case 0x64:
      return "Total Blocks Erased";
    case 0xA7:
      return "SSD Protect Mode";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xA9:
      return "Bad Block Count";
    case 0xAD:
      return "Erase Count";
    case 0xB8:
      return "Factory Bad Block Count";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xCA:
      return "Total Number of Corrected Bits";
    case 0xCD:
      return "Max Rated PE Counts";
    case 0xCE:
      return "Minimum Erase Counts";
    case 0xCF:
      return "Maximum Erase Counts";
    case 0xD3:
      return "SATA Uncorrectable Error Count";
    case 0xD4:
      return "NAND Page Reads During Retry";
    case 0xD5:
      return "Simple Read Retry Attempts";
    case 0xD6:
      return "Adaptive Read Retry Attempts";
    case 0xDD:
      return "Internal Data Path Uncorrectable Errors";
    case 0xDE:
      return "RAID Recovery Count";
    case 0xE6:
      return "Power Loss Protection";
    case 0xFB:
      return "NAND Read Count";
    case 0xF1:
      return "Total Host Writes";
    case 0xF2:
      return "Total Host Reads";
    case 0xF9:
      return "Total NAND Writes";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::OCZ_VECTOR) {
    switch (id) {
    case 0x05:
      return "Accumulated Runtime Bad Blocks";
    case 0x09:
      return "Power-On Hours Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xAB:
      return "Available Over-Provisioned Block Count";
    case 0xAE:
      return "Power Cycle Count (Unplanned)";
    case 0xBB:
      return "Total Uncorrectable NAND Reads";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "Total Programming Failures";
    case 0xC4:
      return "Total Erase Failures";
    case 0xC5:
      return "Total Read Failures (Uncorrectable)";
    case 0xC6:
      return "Host Reads";
    case 0xC7:
      return "Host Writes";
    case 0xD0:
      return "Average Erase Count";
    case 0xD2:
      return "SATA CRC Error Count";
    case 0xE9:
      return "Remaining Life";
    case 0xF9:
      return "Total NAND Programming Count";
    case 0xF1:
      return "Total Host Writes";
    case 0xF2:
      return "Total Host Reads";
    case 0x64:
      return "Total Blocks Erased";
    case 0xB8:
      return "Factory Bad Block Count Total";
    case 0xCA:
      return "Total Number Of Read Bits Corrected";
    case 0xCD:
      return "Max Rated PE Count";
    case 0xCE:
      return "Min Erase Count";
    case 0xCF:
      return "Max Erase Count";
    case 0xD4:
      return "Total Count NAND Pages Reads Requiring Read Retry";
    case 0xD5:
      return "Total Count of Simple Read Retry Attempts";
    case 0xD6:
      return "Total Count of Adaptive Read Retry Attempts";
    case 0xDD:
      return "Internal Data Path Protection Uncorrectable Errors";
    case 0xDE:
      return "RAID Recovery Count";
    case 0xE0:
      return "In Warranty";
    case 0xE1:
      return "DAS Polarity";
    case 0xE2:
      return "Partial Pfail";
    case 0xE6:
      return "Write Throttling Activation Flag";
    case 0xFB:
      return "Total NAND Read Count";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SSSTC) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Rate";
    case 0x05:
      return "Reallocated Block Count";
    case 0x09:
      return "Power On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xAA:
      return "Available Reserved Space";
    case 0xAB:
      return "Program Fail Count (Total)";
    case 0xAC:
      return "Erase Fail Count (Total)";
    case 0xAD:
      return "Average Program/Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xAF:
      return "Program Fail count (Worst case)";
    case 0xB0:
      return "Erase Fail Count (Worst case)";
    case 0xB1:
      return "Wear Leveling Count";
    case 0xB2:
      return "Used Reserved Block Count (Worst case)";
    case 0xB3:
      return "Used Reserved Block Count (Total)";
    case 0xB4:
      return "Unused Reserved Block Count (Total)";
    case 0xB7:
      return "SATA Interface Down shift";
    case 0xB8:
      return "End-to-End Data Errors Count";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xBC:
      return "Command Time out";
    case 0xC2:
      return "Drive Temperature";
    case 0xC3:
      return "ECC rate";
    case 0xC6:
      return "OffLine Uncorrectable Error Count";
    case 0xC7:
      return "SATA CRC Error Count";
    case 0xE5:
      return "Power Loss Protection Failure";
    case 0xE7:
      return "Percentage Lifetime Remaining";
    case 0xE8:
      return "Available Reserved Space";
    case 0xE9:
      return "NAND GB Written";
    case 0xF1:
      return "Total Host Write";
    case 0xF2:
      return "Total Host Read";
    case 0xF3:
      return "NAND GB Written";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::PLEXTOR) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xAA:
      return "Grown Bad Blocks";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Average Program/Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xAF:
      return "Program Fail Count (Worst Case)";
    case 0xB0:
      return "Erase Fail Count (Worst Case)";
    case 0xB1:
      return "Wear Leveling Count";
    case 0xB2:
      return "Used Reserved Block Count (Worst Case)";
    case 0xB3:
      return "Used Reserved Block Count";
    case 0xB4:
      return "UnUsed Reserved Block Count (Total)";
    case 0xB5:
      return "Program Fail Count";
    case 0xB6:
      return "Erase Fail Count";
    case 0xB7:
      return "SATA Interface Down Shift";
    case 0xB8:
      return "End-to-End Data Errors Corrected";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xBC:
      return "Command Time out";
    case 0xC0:
      return "Unsafe Shutdown Count";
    case 0xC3:
      return "ECC rate";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xC7:
      return "Ultra CRC Error Count";
    case 0xE8:
      return "Available Reserved Space";
    case 0xE9:
      return "NAND GB written";
    case 0xF1:
      return "Total Host Writes";
    case 0xF2:
      return "Total Host Reads";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::KINGSTON) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x02:
      return "Throughput Performance";
    case 0x03:
      return "Spin Up Time";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x07:
      return "Seek Error Rate";
    case 0x08:
      return "Seek Time Performance";
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xAA:
      return "Bad Block Count";
    case 0xAD:
      return "Erase count";
    case 0xAF:
      return "Bad Cluster Table Count";
    case 0xBB:
      return "Uncorrectable Errors";
    case 0xC0:
      return "Unsafe Shutdown Count";
    case 0xC2:
      return "Temperature";
    case 0xC4:
      return "Later Bad Block Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC7:
      return "CRC Error Count";
    case 0xDA:
      return "CRC Error Count";
    case 0xE7:
      return "SSD Life Left";
    case 0xE9:
      return "Lifetime Writes to Flash";
    case 0xF0:
      return "Write Head";
    case 0xF1:
      return "Host Writes";
    case 0xF2:
      return "Host Reads";
    case 0xF4:
      return "Average Erase Count";
    case 0xF5:
      return "Max Erase Count";
    case 0xF6:
      return "Total Erase Count";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::CORSAIR) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Rate";
    case 0x05:
      return "Retired Block Count";
    case 0x09:
      return "Power On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xAA:
      return "Reserve Block Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xB5:
      return "Program Fail Count";
    case 0xBB:
      return "Reported Uncorrectable Errors";
    case 0xC2:
      return "Temperature";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xE7:
      return "SSD Life Left";
    case 0xF1:
      return "Lifetime Written";
    case 0xF2:
      return "Lifetime Read";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::REALTEK) {
    switch (id) {
    case 0x01:
      return "Raw data error rate";
    case 0x05:
      return "Reallocated sector count";
    case 0x09:
      return "Power on hour count";
    case 0x0C:
      return "Power cycle count";
    case 0xA1:
      return "GDN";
    case 0xA2:
      return "Total erase count";
    case 0xA3:
      return "Max PE cycle";
    case 0xA4:
      return "Average erase count";
    case 0xA5:
      return "Initial bad block count";
    case 0xA6:
      return "Total bad block count";
    case 0xA7:
      return "SSD protect mode";
    case 0xA8:
      return "SATA Phy error count";
    case 0xA9:
      return "Health";
    case 0xAB:
      return "Program fail count";
    case 0xAC:
      return "Erase fail count";
    case 0xAE:
      return "Unexpected power loss count";
    case 0xAF:
      return "ECC fail count";
    case 0xB5:
      return "Unaligned access count";
    case 0xBB:
      return "Reported uncorrectable error";
    case 0xC2:
      return "Enclosure temperature";
    case 0xC3:
      return "Cumulative corrected ecc";
    case 0xC4:
      return "Reallocation event count";
    case 0xC7:
      return "Ultra DMA CRC error count";
    case 0xCE:
      return "Min. erase count";
    case 0xCF:
      return "Max erase count";
    case 0xE7:
      return "Lifetime left";
    case 0xE8:
      return "Available reserved space";
    case 0xF1:
      return "Write life time";
    case 0xF2:
      return "Read life time";
    case 0xF9:
      return "Total GB written to NAND";
    case 0xFA:
      return "Total GB written to NAND (SLC)";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SK_HYNIX) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x05:
      return "Re-Assigned Sector Count";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0x64:
      return "Total Erase Count";
    case 0xA8:
      return "Min. Erase Count";
    case 0xA9:
      return "Max. Erase Count";
    case 0xAA:
      return "Reserve Block Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xAF:
      return "Program Fail Count (Worst Case)";
    case 0xB0:
      return "Erase Fail Count (Worst Case)";
    case 0xB1:
      return "Wear Leveling Count";
    case 0xB2:
      return "Used Reserved Block Count (Worst Case)";
    case 0xB3:
      return "Used Reserved Block Count (SSD Total)";
    case 0xB4:
      return "UnUsed Reserved Block Count (SSD Total)";
    case 0xB5:
      return "Program Fail Count (SSD Total)";
    case 0xB6:
      return "Erase Fail Count (SSD Total)";
    case 0xB7:
      return "SATA Downshift Count";
    case 0xB8:
      return "End-to-End Error Detection Count";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xBC:
      return "Command Timeout Count";
    case 0xBF:
      return "Op Shock Event Count";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "ECC Rate";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xC7:
      return "CRC Error Count";
    case 0xC9:
      return "Uncorrectable Soft Read Error Rate";
    case 0xCC:
      return "Soft ECC Correction Rate";
    case 0xD4:
      return "Phy Error Count";
    case 0xE7:
      return "SSD Life Left";
    case 0xE9:
      return "SSD Life Left";
    case 0xEA:
      return "NAND Written";
    case 0xF1:
      return "Total LBA Written";
    case 0xF2:
      return "Total LBA Read";
    case 0xFA:
      return "Total Number of NAND Read Retries";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::KIOXIA) {
    switch (id) {
    case 0x09:
      return "Power-On Hours Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA7:
      return "SSD Protect Mode";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xA9:
      return "Bad Block Count";
    case 0xAD:
      return "Erase Count (User Data)";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xF1:
      return "Host Writes";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SILICON_MOTION_CVC) {
    switch (id) {
    case 0x05:
      return "Re-Assigned Count";
    case 0x09:
      return "Power-On Hours Count";
    case 0x0C:
      return "Drive Power Cycle Count";
    case 0x93:
      return "Soft Retry Event Count";
    case 0x94:
      return "RAID Event Count";
    case 0x95:
      return "RAID Uncorrectable Count";
    case 0xA5:
      return "SLC Cache";
    case 0xA7:
      return "SSD Protect Mode";
    case 0xAA:
      return "Grown Bad Block Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Avg Erase Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xAF:
      return "Program Fail Count (Worst case)";
    case 0xB7:
      return "Total SATA Link Speed Downshift";
    case 0xB8:
      return "End-to-End Data Errors corrected";
    case 0xBB:
      return "Uncorrectable Errors";
    case 0xBD:
      return "Max Erase Count";
    case 0xBE:
      return "Temperature";
    case 0xBF:
      return "Min Erase Count";
    case 0xC3:
      return "Read Error Rate";
    case 0xC6:
      return "Off-Line Scan Uncorrectable Sector Count";
    case 0xC7:
      return "SATA CRC Error Count";
    case 0xCA:
      return "SSD Life Left";
    case 0xE8:
      return "Total Spare Counts";
    case 0xE9:
      return "TLC NAND Write";
    case 0xEA:
      return "SLC NAND Write";
    case 0xF1:
      return "Host Write";
    case 0xF2:
      return "Host Read";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SILICON_MOTION) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0x94:
      return "SLC Total Erase Count";
    case 0x95:
      return "SLC Maximum Erase Count";
    case 0x96:
      return "SLC Minimum Erase Count";
    case 0x97:
      return "SLC Average Erase Count";
    case 0x9F:
      return "DRAM 1 bit Error Count";
    case 0xA0:
      return "Uncorrectable sectors count when read/write";
    case 0xA1:
      return "Number of Valid Spare Blocks";
    case 0xA2:
      return "Number of Cache Data Block";
    case 0xA3:
      return "Number of Initial Invalid Blocks";
    case 0xA4:
      return "Total Erase Count";
    case 0xA5:
      return "Maximum Erase Count";
    case 0xA6:
      return "Minimum Erase Count";
    case 0xA7:
      return "Average Erase Count";
    case 0xA8:
      return "Max Erase Count of Spec";
    case 0xA9:
      return "Remain Life";
    case 0xAF:
      return "Program fail count in worst die";
    case 0xB0:
      return "Erase fail count in worst die";
    case 0xB1:
      return "Total Wear Level Count";
    case 0xB2:
      return "Runtime Invalid Block Count";
    case 0xB5:
      return "Total Program Fail Count";
    case 0xB6:
      return "Total Erase Fail Count";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xC0:
      return "Power-Off Retract Count";
    case 0xC2:
      return "Temperature";
    case 0xC3:
      return "Hardware ECC Recovered";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC6:
      return "Uncorrectable Error Count Off-line";
    case 0xC7:
      return "Ultra DMA CRC Error Count";
    case 0xE8:
      return "Available Reserved Space";
    case 0xF1:
      return "Total LBA Written";
    case 0xF2:
      return "Total LBA Read";
    case 0xF5:
      return "Flash Write Sector Count";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::PHISON) {
    switch (id) {
    case 0x01:
      return "Read Error Rate";
    case 0x09:
      return "Power on Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xAA:
      return "Bad Block Count";
    case 0xAD:
      return "Erase Count";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xDA:
      return "Number of CRC Error";
    case 0xE7:
      return "SSD Life Left";
    case 0xF1:
      return "Host Writes";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::MARVELL) {
    switch (id) {
    case 0x05:
      return "Number of New Bad Block";
    case 0x09:
      return "Power On Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA4:
      return "Total Erase Count";
    case 0xA5:
      return "Maximum Erase Count";
    case 0xA6:
      return "Minimum Erase Count";
    case 0xA7:
      return "Average Erase Count";
    case 0xC0:
      return "Power off Retract Count";
    case 0xC2:
      return "Temperature";
    case 0xC7:
      return "SATA CRC Error Count";
    case 0xF1:
      return "Total Written";
    case 0xF2:
      return "Total Read";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::APACER) {
    switch (id) {
    case 0x09:
      return "Power on Hours";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA3:
      return "Maximum Erase Count";
    case 0xA4:
      return "Average Erase Count";
    case 0xA6:
      return "Total Later Bad Block Count";
    case 0xA7:
      return "SSD Protect Mode";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAF:
      return "Bad Cluster Table Count";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Temperature";
    case 0xE7:
      return "Lifetime Left";
    case 0xF1:
      return "Total Sectors Write";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::YMTC) {
    switch (id) {
    case 0x05:
      return "New Bad Block Count";
    case 0x09:
      return "Power-On Hours Count";
    case 0x0C:
      return "Power Cycle Count";
    case 0xA8:
      return "SATA PHY Error Count";
    case 0xA9:
      return "Bad Block Count";
    case 0xAA:
      return "Max Bad Block Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Erase Count";
    case 0xAF:
      return "Bad Cluster Count";
    case 0xB7:
      return "SATA Interface Downshift";
    case 0xB8:
      return "End to End Error Detection/Correction Count";
    case 0xBB:
      return "Uncorrectable Error Count";
    case 0xBE:
      return "Temperature";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC4:
      return "Re-allocated Event Count";
    case 0xC7:
      return "CRC Error Count";
    case 0xCE:
      return "Minimum Erase Count";
    case 0xCF:
      return "Maximum Erase Count";
    case 0xD0:
      return "Average Erase Count";
    case 0xD1:
      return "SLC Minimum Erase Count";
    case 0xD2:
      return "SLC Maximum Erase Count";
    case 0xD3:
      return "SLC Average Erase Count";
    case 0xE7:
      return "SSD Life Left";
    case 0xE9:
      return "Write sector count to NAND";
    case 0xEA:
      return "Read sector count to NAND";
    case 0xF1:
      return "Total LBA written";
    case 0xF2:
      return "Total LBA Read";
    case 0xF3:
      return "NAND Temperature";
    case 0xF5:
      return "Bit Error Count";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::SCY) {
    switch (id) {
    case 0x01:
      return "Raw Read Error Rate";
    case 0x05:
      return "Reallocated Sector Count";
    case 0x07:
      return "Seek Error Rate";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Drive Power Cycle Count";
    case 0xA7:
      return "Average Erase Count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Erase Count";
    case 0xB1:
      return "Read Retry Count";
    case 0xB4:
      return "Spare Block Count Left";
    case 0xB7:
      return "Runtime Bad Block";
    case 0xBB:
      return "Reported UNC Errors";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Controlled Temperature";
    case 0xC3:
      return "Cumulative ECC Bit Correction Count";
    case 0xC4:
      return "Reallocated Event Count";
    case 0xC5:
      return "Current Pending Sector";
    case 0xC7:
      return "UDMA CRC Error Count";
    case 0xE7:
      return "SSD Life Left";
    case 0xF1:
      return "Total LBAs Written";
    case 0xF2:
      return "Total LBAs Read";
    case 0xF5:
      return "Total Nand Flash Written";
    default:
      return "Unknown Attribute";
    }

  } else if (type == SSDType::RECADATA) {
    switch (id) {
    case 0x07:
      return "Seek Error Rate";
    case 0x09:
      return "Power-On Hours";
    case 0x0C:
      return "Drive Power Cycle Count";
    case 0xA7:
      return "Average erase count";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Erase Count";
    case 0xB1:
      return "Read Retry Count";
    case 0xB4:
      return "Spare Block Count Left";
    case 0xB7:
      return "Runtime Bad Block";
    case 0xBB:
      return "Reported UNC Errors";
    case 0xC0:
      return "Unexpected Power Loss Count";
    case 0xC2:
      return "Controlled temperature";
    case 0xC3:
      return "Cumulative ECC Bit Correction Count";
    case 0xC4:
      return "Reallocated Event Count";
    case 0xC5:
      return "Current Pending Sector";
    case 0xC7:
      return "UDMA CRC Error Count";
    case 0xE7:
      return "SSD Life Left";
    case 0xF1:
      return "Total LBAs written";
    case 0xF2:
      return "Total LBAs read";
    case 0xF5:
      return "Total Nand Flash Written";
    default:
      return "Unknown Attribute";
    }
  }
  return "Unknown Attribute";
}

//NVMe -  logic is mostly derived from CrystalDiskInfo, but I haven't tested it on an actual device.


namespace StorageQuery {
	typedef enum {
		StorageDeviceProperty = 0,
		StorageAdapterProperty,
		StorageDeviceIdProperty,
		StorageDeviceUniqueIdProperty,
		StorageDeviceWriteCacheProperty,
		StorageMiniportProperty,
		StorageAccessAlignmentProperty,
		StorageDeviceSeekPenaltyProperty,
		StorageDeviceTrimProperty,
		StorageDeviceWriteAggregationProperty,
		StorageDeviceDeviceTelemetryProperty,
		StorageDeviceLBProvisioningProperty,
		StorageDevicePowerProperty,
		StorageDeviceCopyOffloadProperty,
		StorageDeviceResiliencyProperty,
		StorageDeviceMediumProductType,
		StorageDeviceRpmbProperty,
		StorageDeviceIoCapabilityProperty = 48,
		StorageAdapterProtocolSpecificProperty,
		StorageDeviceProtocolSpecificProperty,
		StorageAdapterTemperatureProperty,
		StorageDeviceTemperatureProperty,
		StorageAdapterPhysicalTopologyProperty,
		StorageDevicePhysicalTopologyProperty,
		StorageDeviceAttributesProperty,
	} TStoragePropertyId;

	typedef enum {
		PropertyStandardQuery = 0,
		PropertyExistsQuery,
		PropertyMaskQuery,
		PropertyQueryMaxDefined
	} TStorageQueryType;

	typedef struct {
		TStoragePropertyId PropertyId;
		TStorageQueryType QueryType;
	} TStoragePropertyQuery;

	typedef enum {
		ProtocolTypeUnknown = 0x00,
		ProtocolTypeScsi,
		ProtocolTypeAta,
		ProtocolTypeNvme,
		ProtocolTypeSd,
		ProtocolTypeProprietary = 0x7E,
		ProtocolTypeMaxReserved = 0x7F
	} TStroageProtocolType;

	typedef struct {
		TStroageProtocolType ProtocolType;
		DWORD   DataType;
		DWORD   ProtocolDataRequestValue;
		DWORD   ProtocolDataRequestSubValue;
		DWORD   ProtocolDataOffset;
		DWORD   ProtocolDataLength;
		DWORD   FixedProtocolReturnData;
		DWORD   Reserved[3];
	} TStorageProtocolSpecificData;

	typedef enum {
		NVMeDataTypeUnknown = 0,
		NVMeDataTypeIdentify,
		NVMeDataTypeLogPage,
		NVMeDataTypeFeature,
	} TStorageProtocolNVMeDataType;

	typedef struct {
		TStoragePropertyQuery Query;
		TStorageProtocolSpecificData ProtocolSpecific;
		BYTE Buffer[4096];
	} TStorageQueryWithBuffer;
}


//From DDK

#define	FILE_DEVICE_SCSI							0x0000001b
#define	IOCTL_SCSI_MINIPORT_IDENTIFY				((FILE_DEVICE_SCSI << 16) + 0x0501)
#define	IOCTL_SCSI_MINIPORT_READ_SMART_ATTRIBS		((FILE_DEVICE_SCSI << 16) + 0x0502)
#define IOCTL_SCSI_MINIPORT_READ_SMART_THRESHOLDS	((FILE_DEVICE_SCSI << 16) + 0x0503)
#define IOCTL_SCSI_MINIPORT_ENABLE_SMART			((FILE_DEVICE_SCSI << 16) + 0x0504)
#define IOCTL_SCSI_MINIPORT_DISABLE_SMART			((FILE_DEVICE_SCSI << 16) + 0x0505)

#define IOCTL_SCSI_BASE                 FILE_DEVICE_CONTROLLER
#define IOCTL_SCSI_PASS_THROUGH         CTL_CODE(IOCTL_SCSI_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//
// Define values for pass-through DataIn field.
//
#define SCSI_IOCTL_DATA_OUT          0
#define SCSI_IOCTL_DATA_IN           1
#define SCSI_IOCTL_DATA_UNSPECIFIED  2

//
// Define the SCSI pass through structure.
//
typedef struct _SCSI_PASS_THROUGH {
	USHORT Length;
	UCHAR ScsiStatus;
	UCHAR PathId;
	UCHAR TargetId;
	UCHAR Lun;
	UCHAR CdbLength;
	UCHAR SenseInfoLength;
	UCHAR DataIn;
	ULONG DataTransferLength;
	ULONG TimeOutValue;
	ULONG_PTR DataBufferOffset;
	ULONG SenseInfoOffset;
	UCHAR Cdb[16];
}SCSI_PASS_THROUGH, *PSCSI_PASS_THROUGH;

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS {
SCSI_PASS_THROUGH Spt;
ULONG             Filler;      // realign buffers to double word boundary
UCHAR             SenseBuf[32];
UCHAR             DataBuf[4096];
} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS24 {
	SCSI_PASS_THROUGH Spt;
	UCHAR             SenseBuf[24];
	UCHAR             DataBuf[4096];
} SCSI_PASS_THROUGH_WITH_BUFFERS24, *PSCSI_PASS_THROUGH_WITH_BUFFERS24;

#define NVME_SIG_STR "NvmeMini"
#define NVME_SIG_STR_LEN 8
#define NVME_FROM_DEV_TO_HOST 2
#define NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE 6
#define NVME_IOCTL_CMD_DW_SIZE 16
#define NVME_IOCTL_COMPLETE_DW_SIZE 4
#define NVME_PT_TIMEOUT 40


	struct NVME_PASS_THROUGH_IOCTL {
		SRB_IO_CONTROL SrbIoCtrl;
		DWORD          VendorSpecific[NVME_IOCTL_VENDOR_SPECIFIC_DW_SIZE];
		DWORD          NVMeCmd[NVME_IOCTL_CMD_DW_SIZE];
		DWORD          CplEntry[NVME_IOCTL_COMPLETE_DW_SIZE];
		DWORD          Direction;
		DWORD          QueueId;
		DWORD          DataBufferLen;
		DWORD          MetaDataLen;
		DWORD          ReturnBufferLen;
		UCHAR          DataBuffer[4096];
	};
  #define NVME_STORPORT_DRIVER 0xE000

  #define NVME_PASS_THROUGH_SRB_IO_CODE CTL_CODE( NVME_STORPORT_DRIVER, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#pragma pack(push, 1)
#pragma pack(push, 1)

typedef union
{
	struct
	{
		ULONG Opcode : 8;
		ULONG FUSE : 2;
		ULONG _Rsvd : 4;
		ULONG PSDT : 2;
		ULONG CID : 16;
	} DUMMYSTRUCTNAME;
	ULONG AsDWord;
} NVME_CDW0, * PNVME_CDW0;

// NVMe Command Format
// See NVMe specification 1.3c Section 4.2, Figure 10
typedef union
{
	struct
	{
		ULONG   CNS : 2;
		ULONG   _Rsvd : 30;
	} DUMMYSTRUCTNAME;
	ULONG AsDWord;
} NVME_IDENTIFY_CDW10, * PNVME_IDENTIFY_CDW10;

// NVMe Specification < 1.3
typedef union
{
	struct
	{
		ULONG   LID : 8;
		ULONG   _Rsvd1 : 8;
		ULONG   NUMD : 12;
		ULONG   _Rsvd2 : 4;
	} DUMMYSTRUCTNAME;
	ULONG   AsDWord;
} NVME_GET_LOG_PAGE_CDW10, * PNVME_GET_LOG_PAGE_CDW10;

// NVMe Specification >= 1.3
typedef union
{
	struct
	{
		ULONG   LID : 8;
		ULONG   LSP : 4;
		ULONG   Reserved0 : 3;
		ULONG   RAE : 1;
		ULONG   NUMDL : 16;
	} DUMMYSTRUCTNAME;
	ULONG   AsDWord;
} NVME_GET_LOG_PAGE_CDW10_V13, * PNVME_GET_LOG_PAGE_CDW10_V13;

typedef struct
{
	// Common fields for all commands
	NVME_CDW0           CDW0;

	ULONG               NSID;
	ULONG               _Rsvd[2];
	ULONGLONG           MPTR;
	ULONGLONG           PRP1;
	ULONGLONG           PRP2;

	// Command independent fields from CDW10 to CDW15
	union
	{
		// Admin Command: Identify (6)
		struct
		{
			NVME_IDENTIFY_CDW10 CDW10;
			ULONG   CDW11;
			ULONG   CDW12;
			ULONG   CDW13;
			ULONG   CDW14;
			ULONG   CDW15;
		} IDENTIFY;

		// Admin Command: Get Log Page (2)
		struct
		{
			NVME_GET_LOG_PAGE_CDW10 CDW10;
			//NVME_GET_LOG_PAGE_CDW10_V13 CDW10;
			ULONG   CDW11;
			ULONG   CDW12;
			ULONG   CDW13;
			ULONG   CDW14;
			ULONG   CDW15;
		} GET_LOG_PAGE;
	} u;
} NVME_CMD, * PNVME_CMD;

typedef struct _INTEL_NVME_PAYLOAD
{
	BYTE    Version;        // 0x001C
	BYTE    PathId;         // 0x001D
	BYTE    TargetID;       // 0x001E
	BYTE    Lun;            // 0x001F
	NVME_CMD Cmd;           // 0x0020 ~ 0x005F
	DWORD   CplEntry[4];    // 0x0060 ~ 0x006F
	DWORD   QueueId;        // 0x0070 ~ 0x0073
	DWORD   ParamBufLen;    // 0x0074
	DWORD   ReturnBufferLen;// 0x0078
	BYTE    __rsvd2[0x28];  // 0x007C ~ 0xA3
} INTEL_NVME_PAYLOAD, * PINTEL_NVME_PAYLOAD;

typedef struct _INTEL_NVME_PASS_THROUGH
{
	SRB_IO_CONTROL SRB;     // 0x0000 ~ 0x001B
	INTEL_NVME_PAYLOAD Payload;
	BYTE DataBuffer[0x1000];
} INTEL_NVME_PASS_THROUGH, * PINTEL_NVME_PASS_THROUGH;
#pragma pack(pop)

#define IOCTL_INTEL_NVME_PASS_THROUGH CTL_CODE(0xf000, 0xA02, METHOD_BUFFERED, FILE_ANY_ACCESS);


HANDLE GetIoCtrlHandle(int index)
{
    // Prepare device path
    std::ostringstream oss;
    oss << "\\\\.\\PhysicalDrive" << index;
    std::string strDevice = oss.str();

    // Open handle
    return CreateFileA(
        strDevice.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
}

std::string GetScsiPath(const std::string& path)
{
    HANDLE hIoCtrl = CreateFileA(
        path.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hIoCtrl == INVALID_HANDLE_VALUE)
        return "";

    SCSI_ADDRESS sadr = {};
    DWORD dwReturned = 0;

    BOOL bRet = DeviceIoControl(
        hIoCtrl,
        IOCTL_SCSI_GET_ADDRESS,
        NULL,
        0,
        &sadr,
        sizeof(sadr),
        &dwReturned,
        NULL
    );

    CloseHandle(hIoCtrl);

    if (!bRet)
        return "";

    std::ostringstream oss;
    oss << "\\\\.\\SCSI" << sadr.PortNumber << ":";

    return oss.str();
}

bool GetSmartAttributeNVMeIntel(int physicalDriveId,UCHAR* OutBuffer)
{

    std::ostringstream oss;
    oss << "\\\\.\\PhysicalDrive" << physicalDriveId;
    std::string path = oss.str();

    HANDLE hIoCtrl = CreateFileA(
        GetScsiPath(path).c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hIoCtrl == INVALID_HANDLE_VALUE)
        return false;

    NVME_PASS_THROUGH_IOCTL nptwb = {};
    DWORD length = sizeof(nptwb);
    DWORD dwReturned = 0;

    nptwb.SrbIoCtrl.ControlCode = NVME_PASS_THROUGH_SRB_IO_CODE;
    nptwb.SrbIoCtrl.HeaderLength = sizeof(SRB_IO_CONTROL);
    memcpy((UCHAR*)(&nptwb.SrbIoCtrl.Signature[0]), NVME_SIG_STR, NVME_SIG_STR_LEN);
    nptwb.SrbIoCtrl.Timeout = NVME_PT_TIMEOUT;
    nptwb.SrbIoCtrl.Length = length - sizeof(SRB_IO_CONTROL);

    nptwb.DataBufferLen = sizeof(nptwb.DataBuffer);
    nptwb.ReturnBufferLen = sizeof(nptwb);
    nptwb.Direction = NVME_FROM_DEV_TO_HOST;

    nptwb.NVMeCmd[0] = 2;                // GetLogPage
    nptwb.NVMeCmd[1] = 0xFFFFFFFF;       // GetLogPage
    nptwb.NVMeCmd[10] = 0x007f0002;

    BOOL bRet = DeviceIoControl(
        hIoCtrl,
        IOCTL_SCSI_MINIPORT,
        &nptwb,
        length,
        &nptwb,
        length,
        &dwReturned,
        NULL
    );

    if (bRet == FALSE)
    {
        CloseHandle(hIoCtrl);
        return false;
    }

    DWORD count = 0;
    for (int i = 0; i < 512; i++)
        count += nptwb.DataBuffer[i];

    if (count == 0)
    {
        CloseHandle(hIoCtrl);
        return false;
    }

    memcpy_s(OutBuffer, 512, nptwb.DataBuffer, 512);

    CloseHandle(hIoCtrl);
    return true;
}


bool GetSmartAttributeNVMeSamsung(int physicalDriveId,UCHAR* OutBuffer)
{
    BOOL bRet = FALSE;
    HANDLE hIoCtrl = NULL;
    DWORD dwReturned = 0;
    DWORD length = 0;

    SCSI_PASS_THROUGH_WITH_BUFFERS24 sptwb = {};

    hIoCtrl = GetIoCtrlHandle(physicalDriveId);

    if (!hIoCtrl || hIoCtrl == INVALID_HANDLE_VALUE)
        return FALSE;

    // Setup first command (SECURITY PROTOCOL OUT)
    sptwb.Spt.Length = sizeof(SCSI_PASS_THROUGH);
    sptwb.Spt.PathId = 0;
    sptwb.Spt.TargetId = 0;
    sptwb.Spt.Lun = 0;
    sptwb.Spt.SenseInfoLength = 24;
    sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
    sptwb.Spt.DataTransferLength = IDENTIFY_BUFFER_SIZE;
    sptwb.Spt.TimeOutValue = 2;
    sptwb.Spt.DataBufferOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, DataBuf);
    sptwb.Spt.SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, SenseBuf);

    sptwb.Spt.CdbLength = 16;
    sptwb.Spt.Cdb[0] = 0xB5;   // SECURITY PROTOCOL OUT
    sptwb.Spt.Cdb[1] = 0xFE;   // SAMSUNG PROTOCOL
    sptwb.Spt.Cdb[9] = 0x40;

    sptwb.Spt.DataIn = SCSI_IOCTL_DATA_OUT;
    sptwb.DataBuf[0] = 2;
    sptwb.DataBuf[4] = 0xFF;
    sptwb.DataBuf[5] = 0xFF;
    sptwb.DataBuf[6] = 0xFF;
    sptwb.DataBuf[7] = 0xFF;

    length = offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS24, DataBuf)
           + sptwb.Spt.DataTransferLength;

    // Send first DeviceIoControl
    bRet = DeviceIoControl(
        hIoCtrl,
        IOCTL_SCSI_PASS_THROUGH,
        &sptwb,
        length,
        &sptwb,
        length,
        &dwReturned,
        NULL
    );

    if (bRet == FALSE)
    {
        CloseHandle(hIoCtrl);
        return FALSE;
    }

    // Setup second command (SECURITY PROTOCOL IN)
    sptwb.Spt.CdbLength = 16;
    sptwb.Spt.Cdb[0] = 0xA2;   // SECURITY PROTOCOL IN
    sptwb.Spt.Cdb[1] = 0xFE;   // SAMSUNG PROTOCOL
    sptwb.Spt.Cdb[8] = 1;

    sptwb.Spt.DataIn = SCSI_IOCTL_DATA_IN;
    sptwb.DataBuf[0] = 0;

    // Send second DeviceIoControl
    bRet = DeviceIoControl(
        hIoCtrl,
        IOCTL_SCSI_PASS_THROUGH,
        &sptwb,
        length,
        &sptwb,
        length,
        &dwReturned,
        NULL
    );

    if (bRet == FALSE)
    {
        CloseHandle(hIoCtrl);
        return FALSE;
    }

    // Verify that data is not zero
    DWORD count = 0;
    for (int i = 0; i < 512; i++)
        count += sptwb.DataBuf[i];

    if (count == 0)
    {
        CloseHandle(hIoCtrl);
        return FALSE;
    }

    // Copy SMART data
    memcpy_s(OutBuffer, 512, sptwb.DataBuf, 512);

    CloseHandle(hIoCtrl);
    return TRUE;
}

bool GetSmartAttributeNVMeStorageQuery(int physicalDriveId, UCHAR* OutBuffer)
{
    // Build "\\\\.\\PhysicalDriveX"
    std::ostringstream oss;
    oss << "\\\\.\\PhysicalDrive" << physicalDriveId;
    std::string path = oss.str();

    HANDLE hIoCtrl = CreateFileA(
        path.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hIoCtrl == INVALID_HANDLE_VALUE)
        return FALSE;

    BOOL bRet = FALSE;
    DWORD dwReturned = 0;

    StorageQuery::TStorageQueryWithBuffer nptwb = {};

    nptwb.ProtocolSpecific.ProtocolType = StorageQuery::ProtocolTypeNvme;
    nptwb.ProtocolSpecific.DataType = StorageQuery::NVMeDataTypeLogPage;
    nptwb.ProtocolSpecific.ProtocolDataRequestValue = 2;   // SMART Health Information
    nptwb.ProtocolSpecific.ProtocolDataRequestSubValue = 0x00000000;
    nptwb.ProtocolSpecific.ProtocolDataOffset = sizeof(StorageQuery::TStorageProtocolSpecificData);
    nptwb.ProtocolSpecific.ProtocolDataLength = 4096;
    nptwb.Query.PropertyId = StorageQuery::StorageAdapterProtocolSpecificProperty;
    nptwb.Query.QueryType = StorageQuery::PropertyStandardQuery;

    // First query
    bRet = DeviceIoControl(
        hIoCtrl,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &nptwb,
        sizeof(nptwb),
        &nptwb,
        sizeof(nptwb),
        &dwReturned,
        NULL
    );

    // Retry if failed with fallback sub-value
    if (!bRet)
    {
        nptwb.ProtocolSpecific.ProtocolDataRequestSubValue = 0xFFFFFFFF;

        bRet = DeviceIoControl(
            hIoCtrl,
            IOCTL_STORAGE_QUERY_PROPERTY,
            &nptwb,
            sizeof(nptwb),
            &nptwb,
            sizeof(nptwb),
            &dwReturned,
            NULL
        );
    }

    CloseHandle(hIoCtrl);

    // Copy SMART data
    memcpy_s(OutBuffer, 512, nptwb.Buffer, 512);
    return bRet;
  }

BOOL GetScsiAddress(const char* path, BYTE* portNumber, BYTE* pathId, BYTE* targetId, BYTE* lun)
{
    HANDLE hDevice = CreateFileA(
        path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    DWORD dwReturned = 0;
    SCSI_ADDRESS scsiAddr = {};

    BOOL bRet = DeviceIoControl(
        hDevice,
        IOCTL_SCSI_GET_ADDRESS,
        nullptr,
        0,
        &scsiAddr,
        sizeof(scsiAddr),
        &dwReturned,
        nullptr
    );

    CloseHandle(hDevice);

    *portNumber = scsiAddr.PortNumber;
    *pathId     = scsiAddr.PathId;
    *targetId   = scsiAddr.TargetId;
    *lun        = scsiAddr.Lun;

    return bRet == TRUE;
}


bool GetSmartAttributeNVMeIntelRst(int physicalDriveId, int scsiPort, int scsiTargetId, UCHAR* OutBuffer)
{
    std::string path;
    BYTE portNumber = 0, pathId = 0, targetId = 0, lun = 0;
    std::string drive;

    if (physicalDriveId == -1)
    {
        portNumber = (BYTE)scsiPort;
        pathId = (BYTE)scsiTargetId;
    }
    else
    {
        path = "\\\\.\\PhysicalDrive" + std::to_string(physicalDriveId);
        GetScsiAddress(path.c_str(), &portNumber, &pathId, &targetId, &lun);
    }

    drive = "\\\\.\\Scsi" + std::to_string(portNumber) + ":";

    HANDLE hIoCtrl = CreateFileA(
        drive.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (hIoCtrl != INVALID_HANDLE_VALUE)
    {
        INTEL_NVME_PASS_THROUGH NVMeData = {};

        NVMeData.SRB.HeaderLength = sizeof(SRB_IO_CONTROL);
        memcpy(NVMeData.SRB.Signature, "IntelNvm", 8);
        NVMeData.SRB.Timeout = 10;
        NVMeData.SRB.ControlCode = IOCTL_INTEL_NVME_PASS_THROUGH;
        NVMeData.SRB.Length = sizeof(INTEL_NVME_PASS_THROUGH) - sizeof(SRB_IO_CONTROL);

        NVMeData.Payload.Version = 1;
        NVMeData.Payload.PathId = pathId;
        NVMeData.Payload.Cmd.CDW0.Opcode = 0x02;
        NVMeData.Payload.Cmd.NSID = 0xFFFFFFFF;
        NVMeData.Payload.Cmd.u.GET_LOG_PAGE.CDW10.LID = 2;
        NVMeData.Payload.Cmd.u.GET_LOG_PAGE.CDW10.NUMD = 0x7F;
        NVMeData.Payload.ParamBufLen = sizeof(INTEL_NVME_PAYLOAD) + sizeof(SRB_IO_CONTROL);
        NVMeData.Payload.ReturnBufferLen = 0x1000;
        NVMeData.Payload.CplEntry[0] = 0;

        DWORD dummy;
        if (DeviceIoControl(
                hIoCtrl,
                IOCTL_SCSI_MINIPORT,
                &NVMeData,
                sizeof(NVMeData),
                &NVMeData,
                sizeof(NVMeData),
                &dummy,
                nullptr))
        {
            memcpy_s(OutBuffer, 512, NVMeData.DataBuffer, 512);
            CloseHandle(hIoCtrl);
            return TRUE;
        }

        CloseHandle(hIoCtrl);
    }

    return FALSE;
}

bool GetSmartAttributeNVMeIntelVroc(int physicalDriveId, int scsiPort, int scsiTargetId,UCHAR* OutBuffer)
{
    std::string path;
    std::string drive;
    BYTE portNumber = 0, pathId = 0, targetId = 0, lun = 0;

    if (physicalDriveId >= 0)
    {
        path = "\\\\.\\PhysicalDrive" + std::to_string(physicalDriveId);
        GetScsiAddress(path.c_str(), &portNumber, &pathId, &targetId, &lun);
    }
    else
    {
        portNumber = (BYTE)scsiPort;
        targetId = (BYTE)scsiTargetId;
    }

    drive = "\\\\.\\Scsi" + std::to_string(portNumber) + ":";

    HANDLE hIoCtrl = CreateFileA(drive.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hIoCtrl == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bRet = FALSE;
    NVME_PASS_THROUGH_IOCTL nptwb = {};
    DWORD length = sizeof(nptwb);
    DWORD dwReturned;

    memcpy(nptwb.SrbIoCtrl.Signature, "NvmeRAID", NVME_SIG_STR_LEN);
    nptwb.SrbIoCtrl.ControlCode = NVME_PASS_THROUGH_SRB_IO_CODE;
    nptwb.SrbIoCtrl.Timeout = NVME_PT_TIMEOUT;
    nptwb.SrbIoCtrl.HeaderLength = sizeof(SRB_IO_CONTROL);
    nptwb.SrbIoCtrl.Length = length - sizeof(SRB_IO_CONTROL);

    nptwb.SrbIoCtrl.ReturnCode = 0x86000000 + (pathId << 16) + (targetId << 8) + lun;

    nptwb.Direction = NVME_FROM_DEV_TO_HOST;
    nptwb.QueueId = 0;
    nptwb.MetaDataLen = 0;
    nptwb.DataBufferLen = sizeof(nptwb.DataBuffer);
    nptwb.ReturnBufferLen = sizeof(nptwb);

    nptwb.NVMeCmd[0] = 0x02;  // Log Page
    nptwb.NVMeCmd[1] = 0xFFFFFFFF;  // Namespace Identifier (CDW1.NSID)
    nptwb.NVMeCmd[10] = 0x7f0002; // Controller or Namespace Structure (CNS)

    nptwb.DataBuffer[0] = TRUE;

    bRet = DeviceIoControl(hIoCtrl, IOCTL_SCSI_MINIPORT,
        &nptwb, length, &nptwb, length, &dwReturned, NULL);

    if (bRet == FALSE)
    {
        CloseHandle(hIoCtrl);
        return FALSE;
    }

    DWORD count = 0;
    for (int i = 0; i < 512; i++)
    {
        count += nptwb.DataBuffer[i];
    }
    if (count == 0)
    {
        CloseHandle(hIoCtrl);
        return FALSE;
    }

    memcpy(OutBuffer, nptwb.DataBuffer, 512);

    CloseHandle(hIoCtrl);
    return bRet;
}



} // namespace smart_reader

// new alias
namespace HardView {
namespace SMART = smart_reader;
}
