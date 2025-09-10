#pragma once
#include "win_helpers.h"
#include <cstdint>
#include <string>
#include <vector>
class EnclosureInfo {
  std::string Name;                    // Name
  std::string Tag;                     // Tag
  std::string Description;             // Description
  std::string Caption;                 // Caption
  std::string Model;                   // Model
  std::string Manufacturer;            // Manufacturer
  std::string SerialNumber;            // SerialNumber
  std::string PartNumber;              // PartNumber
  std::string Version;                 // Version
  std::string SKU;                     // SKU (Stock Keeping Unit)
  std::vector<uint16_t> ChassisTypes;  // ChassisTypes array
  uint16_t NumberOfPowerCords = 0;     // NumberOfPowerCords
  bool PoweredOn = false;              // PoweredOn
  bool Removable = false;              // Removable
  bool Replaceable = false;            // Replaceable
  bool HotSwappable = false;           // HotSwappable
  uint16_t TypeDescriptions = 0;       // TypeDescriptions
  std::string Status;                  // Status
  uint16_t CurrentRequiredOrProduced = 0; // CurrentRequiredOrProduced
  uint16_t SecurityBreach = 0;         // SecurityBreach
  uint16_t SecurityStatus = 0;         // SecurityStatus
  uint16_t ServicePhilosophy = 0;      // ServicePhilosophy
  std::string ServiceDescriptions;     // ServiceDescriptions
  bool LockPresent = false;            // LockPresent
  bool AudibleAlarm = false;           // AudibleAlarm
  bool VisibleAlarm = false;           // VisibleAlarm

  // Extraction functions
  static std::string extract_name(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Name");
  }
  static std::string extract_tag(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Tag");
  }
  static std::string extract_description(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Description");
  }
  static std::string extract_caption(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Caption");
  }
  static std::string extract_model(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Model");
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
  static std::string extract_version(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Version");
  }
  static std::string extract_sku(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"SKU");
  }
  static std::vector<uint16_t> extract_chassis_types(IWbemClassObject *obj) {
    return win::get_uint16_array_property(obj, L"ChassisTypes");
  }
  static uint16_t extract_number_of_power_cords(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"NumberOfPowerCords");
  }
  static bool extract_powered_on(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"PoweredOn");
  }
  static bool extract_removable(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"Removable");
  }
  static bool extract_replaceable(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"Replaceable");
  }
  static bool extract_hot_swappable(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"HotSwappable");
  }
  static uint16_t extract_type_descriptions(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"TypeDescriptions");
  }
  static std::string extract_status(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Status");
  }
  static uint16_t
  extract_current_required_or_produced(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"CurrentRequiredOrProduced");
  }
  static uint16_t extract_security_breach(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"SecurityBreach");
  }
  static uint16_t extract_security_status(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"SecurityStatus");
  }
  static uint16_t extract_service_philosophy(IWbemClassObject *obj) {
    return win::get_uint16_property(obj, L"ServicePhilosophy");
  }
  static std::string extract_service_descriptions(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"ServiceDescriptions");
  }
  static bool extract_lock_present(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"LockPresent");
  }
  static bool extract_audible_alarm(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"AudibleAlarm");
  }
  static bool extract_visible_alarm(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"VisibleAlarm");
  }

public:
  EnclosureInfo() = default;
  inline EnclosureInfo(
      const std::string &name, const std::string &tag,
      const std::string &description, const std::string &caption,
      const std::string &model, const std::string &manufacturer,
      const std::string &serial_number, const std::string &part_number,
      const std::string &version, const std::string &sku,
      const std::vector<uint16_t> &chassis_types,
      uint16_t number_of_power_cords, bool powered_on, bool removable,
      bool replaceable, bool hot_swappable, uint16_t type_descriptions,
      const std::string &status, uint16_t current_required_or_produced,
      uint16_t security_breach, uint16_t security_status,
      uint16_t service_philosophy, const std::string &service_descriptions,
      bool lock_present, bool audible_alarm, bool visible_alarm)
      : Name(name), Tag(tag), Description(description), Caption(caption),
        Model(model), Manufacturer(manufacturer), SerialNumber(serial_number),
        PartNumber(part_number), Version(version), SKU(sku),
        ChassisTypes(chassis_types), NumberOfPowerCords(number_of_power_cords),
        PoweredOn(powered_on), Removable(removable), Replaceable(replaceable),
        HotSwappable(hot_swappable), TypeDescriptions(type_descriptions),
        Status(status),
        CurrentRequiredOrProduced(current_required_or_produced),
        SecurityBreach(security_breach), SecurityStatus(security_status),
        ServicePhilosophy(service_philosophy),
        ServiceDescriptions(service_descriptions), LockPresent(lock_present),
        AudibleAlarm(audible_alarm), VisibleAlarm(visible_alarm) {}
  //---------------------------------------------------------------------------
  // Getters
  inline std::string get_name() const { return Name; }
  inline std::string get_tag() const { return Tag; }
  inline std::string get_description() const { return Description; }
  inline std::string get_caption() const { return Caption; }
  inline std::string get_model() const { return Model; }
  inline std::string get_manufacturer() const { return Manufacturer; }
  inline std::string get_serial_number() const { return SerialNumber; }
  inline std::string get_part_number() const { return PartNumber; }
  inline std::string get_version() const { return Version; }
  inline std::string get_sku() const { return SKU; }
  inline std::vector<uint16_t> get_chassis_types() const {
    return ChassisTypes;
  }
  inline uint16_t get_number_of_power_cords() const {
    return NumberOfPowerCords;
  }
  inline bool get_powered_on() const { return PoweredOn; }
  inline bool get_removable() const { return Removable; }
  inline bool get_replaceable() const { return Replaceable; }
  inline bool get_hot_swappable() const { return HotSwappable; }
  inline uint16_t get_type_descriptions() const { return TypeDescriptions; }
  inline std::string get_status() const { return Status; }
  inline uint16_t get_current_required_or_produced() const {
    return CurrentRequiredOrProduced;
  }
  inline uint16_t get_security_breach() const { return SecurityBreach; }
  inline uint16_t get_security_status() const { return SecurityStatus; }
  inline uint16_t get_service_philosophy() const { return ServicePhilosophy; }
  inline std::string get_service_descriptions() const {
    return ServiceDescriptions;
  }
  inline bool get_lock_present() const { return LockPresent; }
  inline bool get_audible_alarm() const { return AudibleAlarm; }
  inline bool get_visible_alarm() const { return VisibleAlarm; }

  // Helper functions
  inline std::vector<std::string> get_chassis_types_string() const {
    std::vector<std::string> types_string;
    for (uint16_t type : ChassisTypes) {
      switch (type) {
      case 1:
        types_string.push_back("Other");
        break;
      case 2:
        types_string.push_back("Unknown");
        break;
      case 3:
        types_string.push_back("Desktop");
        break;
      case 4:
        types_string.push_back("Low Profile Desktop");
        break;
      case 5:
        types_string.push_back("Pizza Box");
        break;
      case 6:
        types_string.push_back("Mini Tower");
        break;
      case 7:
        types_string.push_back("Tower");
        break;
      case 8:
        types_string.push_back("Portable");
        break;
      case 9:
        types_string.push_back("Laptop");
        break;
      case 10:
        types_string.push_back("Notebook");
        break;
      case 11:
        types_string.push_back("Hand Held");
        break;
      case 12:
        types_string.push_back("Docking Station");
        break;
      case 13:
        types_string.push_back("All in One");
        break;
      case 14:
        types_string.push_back("Sub Notebook");
        break;
      case 15:
        types_string.push_back("Space-saving");
        break;
      case 16:
        types_string.push_back("Lunch Box");
        break;
      case 17:
        types_string.push_back("Main Server Chassis");
        break;
      case 18:
        types_string.push_back("Expansion Chassis");
        break;
      case 19:
        types_string.push_back("SubChassis");
        break;
      case 20:
        types_string.push_back("Bus Expansion Chassis");
        break;
      case 21:
        types_string.push_back("Peripheral Chassis");
        break;
      case 22:
        types_string.push_back("RAID Chassis");
        break;
      case 23:
        types_string.push_back("Rack Mount Chassis");
        break;
      case 24:
        types_string.push_back("Sealed-case PC");
        break;
      case 25:
        types_string.push_back("Multi-system chassis");
        break;
      case 26:
        types_string.push_back("CompactPCI");
        break;
      case 27:
        types_string.push_back("AdvancedTCA");
        break;
      case 28:
        types_string.push_back("Blade");
        break;
      case 29:
        types_string.push_back("Blade Enclosure");
        break;
      case 30:
        types_string.push_back("Tablet");
        break;
      case 31:
        types_string.push_back("Convertible");
        break;
      case 32:
        types_string.push_back("Detachable");
        break;
      case 33:
        types_string.push_back("IoT Gateway");
        break;
      case 34:
        types_string.push_back("Embedded PC");
        break;
      case 35:
        types_string.push_back("Mini PC");
        break;
      case 36:
        types_string.push_back("Stick PC");
        break;
      default:
        types_string.push_back("Unknown Type (" + std::to_string(type) + ")");
        break;
      }
    }
    return types_string;
  }
  inline std::string get_security_breach_string() const {
    switch (SecurityBreach) {
    case 1:
      return "Other";
    case 2:
      return "Unknown";
    case 3:
      return "No Breach";
    case 4:
      return "Breach Attempted";
    case 5:
      return "Breach Successful";
    default:
      return "Unknown (" + std::to_string(SecurityBreach) + ")";
    }
  }
  inline std::string get_security_status_string() const {
    switch (SecurityStatus) {
    case 1:
      return "Other";
    case 2:
      return "Unknown";
    case 3:
      return "None";
    case 4:
      return "External Interface Locked Out";
    case 5:
      return "External Interface Enabled";
    default:
      return "Unknown (" + std::to_string(SecurityStatus) + ")";
    }
  }
  inline std::string get_service_philosophy_string() const {
    switch (ServicePhilosophy) {
    case 0:
      return "Unknown";
    case 1:
      return "Other";
    case 2:
      return "Service From Top";
    case 3:
      return "Service From Front";
    case 4:
      return "Service From Back";
    case 5:
      return "Service From Side";
    case 6:
      return "Sliding Trays";
    case 7:
      return "Removable Sides";
    case 8:
      return "Moveable";
    default:
      return "Unknown (" + std::to_string(ServicePhilosophy) + ")";
    }
  }
  static inline std::vector<EnclosureInfo> query() {
    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;
    IEnumWbemClassObject *pEnumerator = nullptr;
    IWbemClassObject *pclsObj = nullptr;
    std::vector<EnclosureInfo> info_list;
    win::initialize_wmi(&pLoc, &pSvc);

    BSTR wql = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM Win32_SystemEnclosure");

    HRESULT hres = pSvc->ExecQuery(
        wql, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr, &pEnumerator);

    SysFreeString(wql);
    SysFreeString(query);

    if (FAILED(hres)) {
      throw std::runtime_error(
          "Failed to execute WMI query for system enclosure.");
    }
    ULONG uReturn = 0;
    while (SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) &&
           uReturn) {
      info_list.emplace_back(
          extract_name(pclsObj), extract_tag(pclsObj),
          extract_description(pclsObj), extract_caption(pclsObj),
          extract_model(pclsObj), extract_manufacturer(pclsObj),
          extract_serial_number(pclsObj), extract_part_number(pclsObj),
          extract_version(pclsObj), extract_sku(pclsObj),
          extract_chassis_types(pclsObj),
          extract_number_of_power_cords(pclsObj), extract_powered_on(pclsObj),
          extract_removable(pclsObj), extract_replaceable(pclsObj),
          extract_hot_swappable(pclsObj), extract_type_descriptions(pclsObj),
          extract_status(pclsObj),
          extract_current_required_or_produced(pclsObj),
          extract_security_breach(pclsObj), extract_security_status(pclsObj),
          extract_service_philosophy(pclsObj),
          extract_service_descriptions(pclsObj), extract_lock_present(pclsObj),
          extract_audible_alarm(pclsObj), extract_visible_alarm(pclsObj));

      if (pclsObj) {
        pclsObj->Release();
        pclsObj = nullptr;
      }
    }
    win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
    return info_list;
  }
};
