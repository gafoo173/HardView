/*
================================================================================
MIT License

Copyright (c) 2025 gafoo

This file is part of the HardView project:
https://github.com/gafoo173/HardView

Licensed under the MIT License
See the LICENSE file in the project root for more details.
================================================================================
*/
/**
 * @version 2.0
 */
#pragma once

#include <windows.h>
#include <winioctl.h>
#include <iomanip>
#include <memory>
#include <ntddscsi.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


namespace smart_reader {

#pragma pack(push, 1) // No Padding
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
    // Common attributes (HDD & SSD)
    case 0x01:
      return "Raw Read Error Rate";
    case 0x03:
      return "Spin Up Time"; // HDD only
    case 0x04:
      return "Start/Stop Count"; // HDD only
    case 0x05:
      return "Reallocated Sectors Count";
    case 0x07:
      return "Seek Error Rate"; // HDD only
    case 0x09:
      return "Power-On Hours";
    case 0x0A:
      return "Spin Retry Count"; // HDD only
    case 0x0C:
      return "Power Cycle Count";
    case 0xC0:
      return "Power-off Retract Count"; // HDD only
    case 0xC2:
      return "Temperature";
    case 0xC4:
      return "Reallocation Event Count";
    case 0xC5:
      return "Current Pending Sector Count";
    case 0xC6:
      return "Uncorrectable Sector Count";
    case 0xC7:
      return "UltraDMA CRC Error Count";

    // SSD specific attributes
    case 0xA3:
      return "Total LBAs Written";
    case 0xA7:
      return "SSD Life Left";
    case 0xA9:
      return "SSD Life Left (Alternative)";
    case 0xAB:
      return "Program Fail Count";
    case 0xAC:
      return "Erase Fail Count";
    case 0xAD:
      return "Wear Leveling Count";
    case 0xAE:
      return "Unexpected Power Loss Count";
    case 0xAF:
      return "Power Loss Protection Failure";
    case 0xB0:
      return "Erase Fail Count (Alternative)";
    case 0xB1:
      return "Wear Range Delta";
    case 0xB3:
      return "Used Reserved Block Count Total";
    case 0xB4:
      return "Unused Reserved Block Count Total";
    case 0xB5:
      return "Program Fail Count Total";
    case 0xB6:
      return "Erase Fail Count Total";
    case 0xB7:
      return "Runtime Bad Block Total";
    case 0xB8:
      return "End-to-End Error";
    case 0xBE:
      return "Airflow Temperature or Temperature Difference from 100 ";
    case 0xBF:
      return "G-Sense Error Rate";
    case 0xE1:
      return "Host Writes";
    case 0xE2:
      return "Workload Timer";
    case 0xE3:
      return "High Priority Media Type";
    case 0xE4:
      return "LBA of First Error";
    case 0xE6:
      return "GMR Head Amplitude";
    case 0xE7:
      return "SSD Life Left (Percentage)";
    case 0xE8:
      return "Available Reserved Space";
    case 0xE9:
      return "Media Wearout Indicator";
    case 0xEA:
      return "Average Erase Count";
    case 0xEB:
      return "Good Block Count";
    case 0xF0:
      return "Head Flying Hours";
    case 0xF1:
      return "Total LBAs Written (32MB units)";
    case 0xF2:
      return "Total LBAs Read (32MB units)";
    case 0xF9:
      return "NAND Writes";
    case 0xFA:
      return "Read Error Retry Rate";

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
//this struct From https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ata/ns-ata-_identify_device_data
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
  UCHAR  SerialNumber[20];
  USHORT Retired2[2];
  USHORT Obsolete1;
  UCHAR  FirmwareRevision[8];
  UCHAR  ModelNumber[40];
  UCHAR  MaximumBlockTransfer;
  UCHAR  VendorUnique2;
  struct {
    USHORT FeatureSupported : 1;
    USHORT Reserved : 15;
  } TrustedComputing;
  struct {
    UCHAR  CurrentLongPhysicalSectorAlignment : 2;
    UCHAR  ReservedByte49 : 6;
    UCHAR  DmaSupported : 1;
    UCHAR  LbaSupported : 1;
    UCHAR  IordyDisable : 1;
    UCHAR  IordySupported : 1;
    UCHAR  Reserved1 : 1;
    UCHAR  StandybyTimerSupport : 1;
    UCHAR  Reserved2 : 2;
    USHORT ReservedWord50;
  } Capabilities;
  USHORT ObsoleteWords51[2];
  USHORT TranslationFieldsValid : 3;
  USHORT Reserved3 : 5;
  USHORT FreeFallControlSensitivity : 8;
  USHORT NumberOfCurrentCylinders;
  USHORT NumberOfCurrentHeads;
  USHORT CurrentSectorsPerTrack;
  ULONG  CurrentSectorCapacity;
  UCHAR  CurrentMultiSectorSetting;
  UCHAR  MultiSectorSettingValid : 1;
  UCHAR  ReservedByte59 : 3;
  UCHAR  SanitizeFeatureSupported : 1;
  UCHAR  CryptoScrambleExtCommandSupported : 1;
  UCHAR  OverwriteExtCommandSupported : 1;
  UCHAR  BlockEraseExtCommandSupported : 1;
  ULONG  UserAddressableSectors;
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
  ULONG  StreamingPerfGranularity;
  ULONG  Max48BitLBA[2];
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
  ULONG  ExtendedNumberOfUserAddressableSectors[2];
  USHORT MinBlocksPerDownloadMicrocodeMode03;
  USHORT MaxBlocksPerDownloadMicrocodeMode03;
  USHORT ReservedWord236[19];
  USHORT Signature : 8;
  USHORT CheckSum : 8;
} IDENTIFY_DEVICE_DATA, *PIDENTIFY_DEVICE_DATA;

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

  inline bool ReadSmartData() {
    ZeroMemory(&smartData, sizeof(smartData));

    ATA_PASS_THROUGH_DIRECT aptd = {};
    aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
    aptd.TimeOutValue =
        10000; // 10 seconds timeout This time is usually enough.
    aptd.DataTransferLength = sizeof(SmartValues);
    aptd.DataBuffer = &smartData;
    aptd.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

    // SMART READ DATA command
    aptd.CurrentTaskFile[0] = 0xD0; // Features = READ DATA
    aptd.CurrentTaskFile[1] = 0x00; // Sector Count
    aptd.CurrentTaskFile[2] = 0x00; // LBA Low
    aptd.CurrentTaskFile[3] = 0x4F; // LBA Mid (SMART signature)
    aptd.CurrentTaskFile[4] = 0xC2; // LBA High (SMART signature)
    aptd.CurrentTaskFile[5] = 0x00; // Device/Head
    aptd.CurrentTaskFile[6] = 0xB0; // Command = SMART

    DWORD returned = 0;
    bool result =
        DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH_DIRECT, &aptd,
                        sizeof(aptd), &aptd, sizeof(aptd), &returned, nullptr);

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
    aptd.TimeOutValue = 10000; // 10 seconds timeout This time is usually enough.
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

} // namespace smart_reader

//new alias
namespace HV {
  namespace SMART = smart_reader;
}
