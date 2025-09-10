#pragma once

#include "win_helpers.h"
#include <cstdint>
#include <string>
#include <vector>

class MemoryInfo {
  std::string Name;                // Name
  std::string DeviceLocator;       // DeviceLocator
  std::string BankLabel;           // BankLabel
  std::string Manufacturer;        // Manufacturer
  std::string SerialNumber;        // SerialNumber
  std::string PartNumber;          // PartNumber
  std::string Tag;                 // Tag
  uint64_t Capacity = 0;           // Capacity (in bytes)
  uint32_t Speed = 0;              // Speed (in MHz)
  uint32_t ConfiguredClockSpeed = 0; // ConfiguredClockSpeed (in MHz)
  uint32_t ConfiguredVoltage = 0; // ConfiguredVoltage (in millivolts)
  uint32_t MinVoltage = 0;        // MinVoltage (in millivolts)
  uint32_t MaxVoltage = 0;        // MaxVoltage (in millivolts)
  uint16_t FormFactor = 0;        // FormFactor
  uint16_t MemoryType = 0;        // MemoryType
  uint16_t TypeDetail = 0;        // TypeDetail
  uint32_t DataWidth = 0;         // DataWidth (in bits)
  uint32_t TotalWidth = 0;        // TotalWidth (in bits)
  std::string Status;             // Status
  uint16_t PositionInRow = 0;     // PositionInRow
  uint16_t InterleavePosition = 0; // InterleavePosition

  // Extraction functions
  static std::string extract_name(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Name");
  }
  static std::string extract_device_locator(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"DeviceLocator");
  }
  static std::string extract_bank_label(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"BankLabel");
  }
  static std::string extract_manufacturer(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Manufacturer");
  }
  static std::string extract_serial_number(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"SerialNumber");
  }
  static std::string extract_part_number(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"PartNumber");
  }
  static std::string extract_tag(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Tag");
  }
  static uint64_t extract_capacity(IWbemClassObject *obj) {
    return win::get_uint64_property(obj, L"Capacity");
  }
  static uint32_t extract_speed(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"Speed");
  }
  static uint32_t extract_configured_clock_speed(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"ConfiguredClockSpeed");
  }
  static uint32_t extract_configured_voltage(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"ConfiguredVoltage");
  }
  static uint32_t extract_min_voltage(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"MinVoltage");
  }
  static uint32_t extract_max_voltage(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"MaxVoltage");
  }
  static uint16_t extract_form_factor(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"FormFactor");
  }
  static uint16_t extract_memory_type(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"MemoryType");
  }
  static uint16_t extract_type_detail(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"TypeDetail");
  }
  static uint32_t extract_data_width(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"DataWidth");
  }
  static uint32_t extract_total_width(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"TotalWidth");
  }
  static std::string extract_status(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Status");
  }
  static uint16_t extract_position_in_row(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"PositionInRow");
  }
  static uint16_t extract_interleave_position(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"InterleavePosition");
  }

public:
  MemoryInfo() = default;
  // Parametrized constructor
  inline MemoryInfo(const std::string &name, const std::string &device_locator,
                    const std::string &bank_label,
                    const std::string &manufacturer,
                    const std::string &serial_number,
                    const std::string &part_number, const std::string &tag,
                    uint64_t capacity, uint32_t speed,
                    uint32_t configured_clock_speed,
                    uint32_t configured_voltage, uint32_t min_voltage,
                    uint32_t max_voltage, uint16_t form_factor,
                    uint16_t memory_type, uint16_t type_detail,
                    uint32_t data_width, uint32_t total_width,
                    const std::string &status, uint16_t position_in_row,
                    uint16_t interleave_position)
      : Name(name), DeviceLocator(device_locator), BankLabel(bank_label),
        Manufacturer(manufacturer), SerialNumber(serial_number),
        PartNumber(part_number), Tag(tag), Capacity(capacity), Speed(speed),
        ConfiguredClockSpeed(configured_clock_speed),
        ConfiguredVoltage(configured_voltage), MinVoltage(min_voltage),
        MaxVoltage(max_voltage), FormFactor(form_factor),
        MemoryType(memory_type), TypeDetail(type_detail),
        DataWidth(data_width), TotalWidth(total_width), Status(status),
        PositionInRow(position_in_row),
        InterleavePosition(interleave_position) {}
  //---------------------------------------------------------------------------
  // Getters
  inline std::string get_name() const { return Name; }
  inline std::string get_device_locator() const { return DeviceLocator; }
  inline std::string get_bank_label() const { return BankLabel; }
  inline std::string get_manufacturer() const { return Manufacturer; }
  inline std::string get_serial_number() const { return SerialNumber; }
  inline std::string get_part_number() const { return PartNumber; }
  inline std::string get_tag() const { return Tag; }
  inline uint64_t get_capacity() const { return Capacity; }
  inline uint32_t get_speed() const { return Speed; }
  inline uint32_t get_configured_clock_speed() const {
    return ConfiguredClockSpeed;
  }
  inline uint32_t get_configured_voltage() const { return ConfiguredVoltage; }
  inline uint32_t get_min_voltage() const { return MinVoltage; }
  inline uint32_t get_max_voltage() const { return MaxVoltage; }
  inline uint16_t get_form_factor() const { return FormFactor; }
  inline uint16_t get_memory_type() const { return MemoryType; }
  inline uint16_t get_type_detail() const { return TypeDetail; }
  inline uint32_t get_data_width() const { return DataWidth; }
  inline uint32_t get_total_width() const { return TotalWidth; }
  inline std::string get_status() const { return Status; }
  inline uint16_t get_position_in_row() const { return PositionInRow; }
  inline uint16_t get_interleave_position() const { return InterleavePosition; }

  // Helper function to get capacity in GB
  inline double get_capacity_gb() const {
    return static_cast<double>(Capacity) / (1024.0 * 1024.0 * 1024.0);
  }

  // Helper function to get memory type as string
  inline std::string get_memory_type_string() const {
    switch (MemoryType) {
    case 1:
      return "Other";
    case 2:
      return "DRAM";
    case 3:
      return "Synchronous DRAM";
    case 4:
      return "Cache DRAM";
    case 5:
      return "EDO";
    case 6:
      return "EDRAM";
    case 7:
      return "VRAM";
    case 8:
      return "SRAM";
    case 9:
      return "RAM";
    case 10:
      return "ROM";
    case 11:
      return "Flash";
    case 12:
      return "EEPROM";
    case 13:
      return "FEPROM";
    case 14:
      return "EPROM";
    case 15:
      return "CDRAM";
    case 16:
      return "3DRAM";
    case 17:
      return "SDRAM";
    case 18:
      return "SGRAM";
    case 19:
      return "RDRAM";
    case 20:
      return "DDR";
    case 21:
      return "DDR2";
    case 22:
      return "DDR2 FB-DIMM";
    case 24:
      return "DDR3";
    case 25:
      return "FBD2";
    case 26:
      return "DDR4";
    case 27:
      return "LPDDR";
    case 28:
      return "LPDDR2";
    case 29:
      return "LPDDR3";
    case 30:
      return "LPDDR4";
    case 31:
      return "Logical non-volatile device";
    case 32:
      return "HBM";
    case 33:
      return "HBM2";
    case 34:
      return "DDR5";
    case 35:
      return "LPDDR5";
    default:
      return "Unknown";
    }
  }

  // Helper function to get form factor as string
  inline std::string get_form_factor_string() const {
    switch (FormFactor) {
    case 1:
      return "Other";
    case 2:
      return "SIP";
    case 3:
      return "DIP";
    case 4:
      return "ZIP";
    case 5:
      return "SOJ";
    case 6:
      return "Proprietary";
    case 7:
      return "SIMM";
    case 8:
      return "DIMM";
    case 9:
      return "TSOP";
    case 10:
      return "PGA";
    case 11:
      return "RIMM";
    case 12:
      return "SODIMM";
    case 13:
      return "SRIMM";
    case 14:
      return "SMD";
    case 15:
      return "SSMP";
    case 16:
      return "QFP";
    case 17:
      return "TQFP";
    case 18:
      return "SOIC";
    case 19:
      return "LCC";
    case 20:
      return "PLCC";
    case 21:
      return "BGA";
    case 22:
      return "FPBGA";
    case 23:
      return "LGA";
    default:
      return "Unknown";
    }
  }
  //---------------------------------------------------------------------------
  // Query function
  static inline std::vector<MemoryInfo> query() {
    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;
    IEnumWbemClassObject *pEnumerator = nullptr;
    IWbemClassObject *pclsObj = nullptr;
    std::vector<MemoryInfo> info_list;
    win::initialize_wmi(&pLoc, &pSvc);

    BSTR wql = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM Win32_PhysicalMemory");

    HRESULT hres = pSvc->ExecQuery(
        wql, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr, &pEnumerator);

    SysFreeString(wql);
    SysFreeString(query);

    if (FAILED(hres)) {
      throw std::runtime_error(
          "Failed to execute WMI query for physical memory.");
    }
    ULONG uReturn = 0;
    while (SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) &&
           uReturn) {
      info_list.emplace_back(
          extract_name(pclsObj), extract_device_locator(pclsObj),
          extract_bank_label(pclsObj), extract_manufacturer(pclsObj),
          extract_serial_number(pclsObj), extract_part_number(pclsObj),
          extract_tag(pclsObj), extract_capacity(pclsObj),
          extract_speed(pclsObj), extract_configured_clock_speed(pclsObj),
          extract_configured_voltage(pclsObj), extract_min_voltage(pclsObj),
          extract_max_voltage(pclsObj), extract_form_factor(pclsObj),
          extract_memory_type(pclsObj), extract_type_detail(pclsObj),
          extract_data_width(pclsObj), extract_total_width(pclsObj),
          extract_status(pclsObj), extract_position_in_row(pclsObj),
          extract_interleave_position(pclsObj));

      if (pclsObj) {
        pclsObj->Release();
        pclsObj = nullptr;
      }
    }
    win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return info_list;
  }
  //---------------------------------------------------------------------------
};
