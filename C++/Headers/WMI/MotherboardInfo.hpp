#pragma once

#include "win_helpers.h"
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

class MotherboardInfo {
  std::string Name;                // Name
  std::string Tag;                 // Tag
  std::string Description;         // Description
  std::string Caption;             // Caption
  std::string Model;               // Model
  std::string Manufacturer;        // Manufacturer
  std::string SerialNumber;        // SerialNumber
  std::string PartNumber;          // PartNumber
  std::string Version;             // Version
  std::string Product;             // Product
  std::string SKU;                 // SKU (Stock Keeping Unit)
  std::string SlotLayout;          // SlotLayout
  bool HostingBoard = false;       // HostingBoard
  bool HotSwappable = false;       // HotSwappable
  bool Removable = false;          // Removable
  bool Replaceable = false;        // Replaceable
  std::string Status;              // Status
  std::string ConfigOptions;       // ConfigOptions
  std::string CreationClassName;   // CreationClassName
  float Depth = 0.0f;              // Depth (in inches)
  float Height = 0.0f;             // Height (in inches)
  float Width = 0.0f;              // Width (in inches)
  float Weight = 0.0f;             // Weight (in pounds)
  std::string InstallDate;         // InstallDate
  std::string PoweredOn;           // PoweredOn
  std::vector<uint16_t> ConfigManagerErrorCode; // ConfigManagerErrorCode
  uint32_t ConfigManagerUserConfig = 0; // ConfigManagerUserConfig
  bool RequiresDaughterBoard = false;   // RequiresDaughterBoard
  bool SpecialRequirements = false;     // SpecialRequirements

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
  static std::string extract_product(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Product");
  }
  static std::string extract_sku(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"SKU");
  }
  static std::string extract_slot_layout(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"SlotLayout");
  }
  static bool extract_hosting_board(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"HostingBoard");
  }
  static bool extract_hot_swappable(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"HotSwappable");
  }
  static bool extract_removable(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"Removable");
  }
  static bool extract_replaceable(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"Replaceable");
  }
  static std::string extract_status(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Status");
  }
  static std::string extract_config_options(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"ConfigOptions");
  }
  static std::string extract_creation_class_name(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"CreationClassName");
  }
  static float extract_depth(IWbemClassObject *obj) {
    return win::get_float_property(obj, L"Depth");
  }
  static float extract_height(IWbemClassObject *obj) {
    return win::get_float_property(obj, L"Height");
  }
  static float extract_width(IWbemClassObject *obj) {
    return win::get_float_property(obj, L"Width");
  }
  static float extract_weight(IWbemClassObject *obj) {
    return win::get_float_property(obj, L"Weight");
  }
  static std::string extract_install_date(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"InstallDate");
  }
  static std::string extract_powered_on(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"PoweredOn");
  }
  static std::vector<uint16_t>
  extract_config_manager_error_code(IWbemClassObject *obj) {
    return win::get_uint16_array_property(obj, L"ConfigManagerErrorCode");
  }
  static uint32_t extract_config_manager_user_config(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"ConfigManagerUserConfig");
  }
  static bool extract_requires_daughter_board(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"RequiresDaughterBoard");
  }
  static bool extract_special_requirements(IWbemClassObject *obj) {
    return win::get_bool_property(obj, L"SpecialRequirements");
  }

public:
  MotherboardInfo() = default;
  // Parametrized constructor
  inline MotherboardInfo(
      const std::string &name, const std::string &tag,
      const std::string &description, const std::string &caption,
      const std::string &model, const std::string &manufacturer,
      const std::string &serial_number, const std::string &part_number,
      const std::string &version, const std::string &product,
      const std::string &sku, const std::string &slot_layout,
      bool hosting_board, bool hot_swappable, bool removable, bool replaceable,
      const std::string &status, const std::string &config_options,
      const std::string &creation_class_name, float depth, float height,
      float width, float weight, const std::string &install_date,
      const std::string &powered_on,
      const std::vector<uint16_t> &config_manager_error_code,
      uint32_t config_manager_user_config, bool requires_daughter_board,
      bool special_requirements)
      : Name(name), Tag(tag), Description(description), Caption(caption),
        Model(model), Manufacturer(manufacturer), SerialNumber(serial_number),
        PartNumber(part_number), Version(version), Product(product), SKU(sku),
        SlotLayout(slot_layout), HostingBoard(hosting_board),
        HotSwappable(hot_swappable), Removable(removable),
        Replaceable(replaceable), Status(status),
        ConfigOptions(config_options), CreationClassName(creation_class_name),
        Depth(depth), Height(height), Width(width), Weight(weight),
        InstallDate(install_date), PoweredOn(powered_on),
        ConfigManagerErrorCode(config_manager_error_code),
        ConfigManagerUserConfig(config_manager_user_config),
        RequiresDaughterBoard(requires_daughter_board),
        SpecialRequirements(special_requirements) {}
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
  inline std::string get_product() const { return Product; }
  inline std::string get_sku() const { return SKU; }
  inline std::string get_slot_layout() const { return SlotLayout; }
  inline bool get_hosting_board() const { return HostingBoard; }
  inline bool get_hot_swappable() const { return HotSwappable; }
  inline bool get_removable() const { return Removable; }
  inline bool get_replaceable() const { return Replaceable; }
  inline std::string get_status() const { return Status; }
  inline std::string get_config_options() const { return ConfigOptions; }
  inline std::string get_creation_class_name() const {
    return CreationClassName;
  }
  inline float get_depth() const { return Depth; }
  inline float get_height() const { return Height; }
  inline float get_width() const { return Width; }
  inline float get_weight() const { return Weight; }
  inline std::string get_install_date() const { return InstallDate; }
  inline std::string get_powered_on() const { return PoweredOn; }
  inline std::vector<uint16_t> get_config_manager_error_code() const {
    return ConfigManagerErrorCode;
  }
  inline uint32_t get_config_manager_user_config() const {
    return ConfigManagerUserConfig;
  }
  inline bool get_requires_daughter_board() const {
    return RequiresDaughterBoard;
  }
  inline bool get_special_requirements() const { return SpecialRequirements; }

  // Helper functions
  inline std::string get_dimensions_string() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << Width << "\" x " << Height << "\" x " << Depth << "\"";
    if (Weight > 0) {
      oss << " (" << Weight << " lbs)";
    }
    return oss.str();
  }
  inline std::vector<std::string>
  get_config_manager_error_code_string() const {
    std::vector<std::string> error_strings;
    for (uint16_t error_code : ConfigManagerErrorCode) {
      switch (error_code) {
      case 0:
        error_strings.push_back("Device is working properly");
        break;
      case 1:
        error_strings.push_back("Device is not configured correctly");
        break;
      case 2:
        error_strings.push_back(
            "Windows cannot load the driver for this device");
        break;
      case 3:
        error_strings.push_back("Driver for this device might be corrupted");
        break;
      case 4:
        error_strings.push_back("Device is not working properly");
        break;
      case 5:
        error_strings.push_back(
            "Windows cannot manage the resources for this device");
        break;
      case 6:
        error_strings.push_back(
            "Boot configuration for this device conflicts with other devices");
        break;
      case 7:
        error_strings.push_back("Cannot filter resources for this device");
        break;
      case 8:
        error_strings.push_back("Driver loader for the device is missing");
        break;
      case 9:
        error_strings.push_back("Device is not working properly because the "
                                "controlling firmware is reporting the "
                                "resources for the device incorrectly");
        break;
      case 10:
        error_strings.push_back("Device cannot start");
        break;
      case 11:
        error_strings.push_back("Device failed");
        break;
      case 12:
        error_strings.push_back(
            "Device cannot find enough free resources that it can use");
        break;
      case 13:
        error_strings.push_back("Windows cannot verify the device's resources");
        break;
      case 14:
        error_strings.push_back(
            "Device cannot work properly until you restart the computer");
        break;
      case 15:
        error_strings.push_back("Device is not working properly because there "
                                "is probably a re-enumeration problem");
        break;
      case 16:
        error_strings.push_back(
            "Windows cannot identify all the resources this device uses");
        break;
      case 17:
        error_strings.push_back("Device is asking for an unknown resource type");
        break;
      case 18:
        error_strings.push_back("Reinstall the drivers for this device");
        break;
      case 19:
        error_strings.push_back("Failure using the VxD loader");
        break;
      case 20:
        error_strings.push_back("Registry might be corrupted");
        break;
      case 21:
        error_strings.push_back("System failure: Try changing the driver for this device");
        break;
      case 22:
        error_strings.push_back("Device is disabled");
        break;
      case 23:
        error_strings.push_back(
            "System failure: Try changing the driver for this device");
        break;
      case 24:
        error_strings.push_back(
            "Device is not present, is not working properly, or does not have "
            "all its drivers installed");
        break;
      case 25:
        error_strings.push_back("Windows is still setting up this device");
        break;
      case 26:
        error_strings.push_back("Windows is still setting up this device");
        break;
      case 27:
        error_strings.push_back(
            "Device does not have valid log configuration");
        break;
      case 28:
        error_strings.push_back("Drivers for this device are not installed");
        break;
      case 29:
        error_strings.push_back(
            "Device is disabled because the firmware of the device did not "
            "give it the required resources");
        break;
      case 30:
        error_strings.push_back(
            "Device is using an Interrupt Request (IRQ) resource that another "
            "device is using");
        break;
      case 31:
        error_strings.push_back(
            "Device is not working properly because Windows cannot load the "
            "drivers required for this device");
        break;
      default:
        error_strings.push_back("Unknown error code (" +
                                std::to_string(error_code) + ")");
        break;
      }
    }
    return error_strings;
  }
  inline float get_area_square_inches() const {
    if (Width > 0 && Height > 0) {
      return Width * Height;
    }
    return 0.0f;
  }
  //---------------------------------------------------------------------------
  // Query function
  static inline std::vector<MotherboardInfo> query() {
    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;
    IEnumWbemClassObject *pEnumerator = nullptr;
    IWbemClassObject *pclsObj = nullptr;
    std::vector<MotherboardInfo> info_list;
    win::initialize_wmi(&pLoc, &pSvc);

    BSTR wql = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM Win32_BaseBoard");

    HRESULT hres = pSvc->ExecQuery(
        wql, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr, &pEnumerator);

    SysFreeString(wql);
    SysFreeString(query);

    if (FAILED(hres)) {
      throw std::runtime_error(
          "Failed to execute WMI query for motherboard.");
    }
    ULONG uReturn = 0;
    while (SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) &&
           uReturn) {
      info_list.emplace_back(
          extract_name(pclsObj), extract_tag(pclsObj),
          extract_description(pclsObj), extract_caption(pclsObj),
          extract_model(pclsObj), extract_manufacturer(pclsObj),
          extract_serial_number(pclsObj), extract_part_number(pclsObj),
          extract_version(pclsObj), extract_product(pclsObj),
          extract_sku(pclsObj), extract_slot_layout(pclsObj),
          extract_hosting_board(pclsObj), extract_hot_swappable(pclsObj),
          extract_removable(pclsObj), extract_replaceable(pclsObj),
          extract_status(pclsObj), extract_config_options(pclsObj),
          extract_creation_class_name(pclsObj), extract_depth(pclsObj),
          extract_height(pclsObj), extract_width(pclsObj),
          extract_weight(pclsObj), extract_install_date(pclsObj),
          extract_powered_on(pclsObj),
          extract_config_manager_error_code(pclsObj),
          extract_config_manager_user_config(pclsObj),
          extract_requires_daughter_board(pclsObj),
          extract_special_requirements(pclsObj));

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
