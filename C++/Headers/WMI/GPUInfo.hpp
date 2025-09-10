#pragma once

#include "win_helpers.h"
#include <cstdint>
#include <string>
#include <vector>
#ifdef _WIN32
#include <Wbemidl.h>
#endif

class GPUInfo {
  std::string name;
  std::string manufacturer;
  std::string device_id;
  std::string video_processor;
  std::string adapter_ram;
  std::string adapter_dac_type;
  std::string video_mode_description;
  std::string current_horizontal_resolution;
  std::string current_vertical_resolution;
  std::string current_refresh_rate;
  std::string current_bits_per_pixel;
  std::string driver_version;
  std::string driver_date;
  std::string inf_filename;
  std::string inf_section;
  std::string pnp_device_id;
  std::string system_name;
  std::string adapter_type;
  std::string video_architecture;
  std::string video_memory_type;
  std::string max_refresh_rate;
  std::string min_refresh_rate;
  std::string max_memory_supported;
  std::string availability;
  std::string status;
  std::string power_management_supported;
  std::string current_number_of_colors;
  std::string current_scan_mode;
  std::string acceleration_caps;
  std::string caption;
  std::string description;
  std::string install_date;
  std::string last_error_code;
  std::string dither_type;
  std::string color_table_entries;
  std::string device_specific_pens;
  std::string monochrome;
  std::string reserved_system_palette_entries;
  std::string system_palette_entries;
  std::string time_of_last_reset;
  std::string number_of_color_planes;
  std::string current_number_of_columns;
  std::string current_number_of_rows;

  // Extraction functions
  static std::string extract_name(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Name");
  }

  static std::string extract_manufacturer(IWbemClassObject *obj) {
    std::string manufacturer = win::get_string_property(obj, L"Manufacturer");
    if (manufacturer == "N/A" || manufacturer.empty()) {
      manufacturer = win::get_string_property(obj, L"AdapterCompatibility");
    }
    return manufacturer;
  }

  static std::string extract_device_id(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"DeviceID");
  }

  static std::string extract_video_processor(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"VideoProcessor");
  }

  static std::string extract_adapter_ram(IWbemClassObject *obj) {
    uint64_t ram = win::get_int64_property(obj, L"AdapterRAM");
    if (ram > 0) {
      double gb = static_cast<double>(ram) / (1024 * 1024 * 1024);
      return std::to_string(gb) + " GB";
    }
    return "N/A";
  }

  static std::string extract_adapter_dac_type(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"AdapterDACType");
  }

  static std::string extract_video_mode_description(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"VideoModeDescription");
  }

  static std::string
  extract_current_horizontal_resolution(IWbemClassObject *obj) {
    uint32_t resolution =
        win::get_uint32_property(obj, L"CurrentHorizontalResolution");
    return resolution > 0 ? std::to_string(resolution) : "N/A";
  }

  static std::string
  extract_current_vertical_resolution(IWbemClassObject *obj) {
    uint32_t resolution =
        win::get_uint32_property(obj, L"CurrentVerticalResolution");
    return resolution > 0 ? std::to_string(resolution) : "N/A";
  }

  static std::string extract_current_refresh_rate(IWbemClassObject *obj) {
    uint32_t rate = win::get_uint32_property(obj, L"CurrentRefreshRate");
    return rate > 0 ? std::to_string(rate) + " Hz" : "N/A";
  }

  static std::string extract_current_bits_per_pixel(IWbemClassObject *obj) {
    uint32_t bits = win::get_uint32_property(obj, L"CurrentBitsPerPixel");
    return bits > 0 ? std::to_string(bits) : "N/A";
  }

  static std::string extract_driver_version(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"DriverVersion");
  }

  static std::string extract_driver_date(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"DriverDate");
  }

  static std::string extract_inf_filename(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"InfFilename");
  }

  static std::string extract_inf_section(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"InfSection");
  }

  static std::string extract_pnp_device_id(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"PNPDeviceID");
  }

  static std::string extract_system_name(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"SystemName");
  }

  static std::string extract_adapter_type(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"AdapterType");
  }

  static std::string extract_video_architecture(IWbemClassObject *obj) {
    uint32_t arch = win::get_uint32_property(obj, L"VideoArchitecture");
    return arch > 0 ? std::to_string(arch) : "N/A";
  }

  static std::string extract_video_memory_type(IWbemClassObject *obj) {
    uint32_t type = win::get_uint32_property(obj, L"VideoMemoryType");
    return type > 0 ? std::to_string(type) : "N/A";
  }

  static std::string extract_max_refresh_rate(IWbemClassObject *obj) {
    uint32_t rate = win::get_uint32_property(obj, L"MaxRefreshRate");
    return rate > 0 ? std::to_string(rate) + " Hz" : "N/A";
  }

  static std::string extract_min_refresh_rate(IWbemClassObject *obj) {
    uint32_t rate = win::get_uint32_property(obj, L"MinRefreshRate");
    return rate > 0 ? std::to_string(rate) + " Hz" : "N/A";
  }

  static std::string extract_max_memory_supported(IWbemClassObject *obj) {
    uint32_t memory = win::get_uint32_property(obj, L"MaxMemorySupported");
    if (memory > 0) {
      double mb = static_cast<double>(memory) / (1024 * 1024);
      return std::to_string(mb) + " MB";
    }
    return "N/A";
  }

  static std::string extract_availability(IWbemClassObject *obj) {
    uint32_t avail = win::get_uint32_property(obj, L"Availability");
    return avail > 0 ? std::to_string(avail) : "N/A";
  }

  static std::string extract_status(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Status");
  }

  static std::string
  extract_power_management_supported(IWbemClassObject *obj) {
    bool supported = win::get_bool_property(obj, L"PowerManagementSupported");
    return supported ? "True" : "False";
  }

  static std::string extract_current_number_of_colors(IWbemClassObject *obj) {
    uint64_t colors = win::get_int64_property(obj, L"CurrentNumberOfColors");
    return colors > 0 ? std::to_string(colors) : "N/A";
  }

  static std::string extract_current_scan_mode(IWbemClassObject *obj) {
    uint32_t mode = win::get_uint32_property(obj, L"CurrentScanMode");
    if (mode > 0) {
      switch (mode) {
      case 1:
        return "Interlaced";
      case 2:
        return "Noninterlaced";
      case 3:
        return "Interlaced and Noninterlaced";
      case 4:
        return "Display Scan Mode Unknown";
      default:
        return std::to_string(mode);
      }
    }
    return "N/A";
  }

  static std::string extract_acceleration_caps(IWbemClassObject *obj) {
    std::string caps =
        win::get_string_property(obj, L"AcceleratorCapabilities");
    if (caps != "N/A" && !caps.empty()) {
      return caps;
    }
    uint32_t caps_num =
        win::get_uint32_property(obj, L"AccelerationCapabilities");
    return caps_num > 0 ? std::to_string(caps_num) : "N/A";
  }

  static std::string extract_caption(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Caption");
  }

  static std::string extract_description(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Description");
  }

  static std::string extract_install_date(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"InstallDate");
  }

  static std::string extract_last_error_code(IWbemClassObject *obj) {
    std::string error_str = win::get_string_property(obj, L"LastErrorCode");
    if (error_str != "N/A" && !error_str.empty()) {
      return error_str;
    }
    uint32_t error = win::get_uint32_property(obj, L"LastErrorCode");
    return error > 0 ? std::to_string(error) : "No Errors";
  }

  static std::string extract_dither_type(IWbemClassObject *obj) {
    uint32_t type = win::get_uint32_property(obj, L"DitherType");
    switch (type) {
    case 0:
      return "None";
    case 1:
      return "Solid";
    case 2:
      return "Pattern";
    case 3:
      return "Diffusion";
    default:
      return std::to_string(type);
    }
  }

  static std::string extract_color_table_entries(IWbemClassObject *obj) {
    std::string entries_str =
        win::get_string_property(obj, L"ColorTableEntries");
    if (entries_str != "N/A" && !entries_str.empty()) {
      return entries_str;
    }
    uint32_t entries = win::get_uint32_property(obj, L"ColorTableEntries");
    return entries > 0 ? std::to_string(entries) : "Not Available";
  }

  static std::string extract_device_specific_pens(IWbemClassObject *obj) {
    std::string pens_str = win::get_string_property(obj, L"DeviceSpecificPens");
    if (pens_str != "N/A" && !pens_str.empty()) {
      return pens_str;
    }
    uint32_t pens = win::get_uint32_property(obj, L"DeviceSpecificPens");
    return pens > 0 ? std::to_string(pens) : "N/A";
  }

  static std::string extract_monochrome(IWbemClassObject *obj) {
    bool mono = win::get_bool_property(obj, L"Monochrome");
    return mono ? "True" : "False";
  }

  static std::string
  extract_reserved_system_palette_entries(IWbemClassObject *obj) {
    std::string entries_str =
        win::get_string_property(obj, L"ReservedSystemPaletteEntries");
    if (entries_str != "N/A" && !entries_str.empty()) {
      return entries_str;
    }
    uint32_t entries =
        win::get_uint32_property(obj, L"ReservedSystemPaletteEntries");
    return entries > 0 ? std::to_string(entries) : "Not Available";
  }

  static std::string extract_system_palette_entries(IWbemClassObject *obj) {
    std::string entries_str =
        win::get_string_property(obj, L"SystemPaletteEntries");
    if (entries_str != "N/A" && !entries_str.empty()) {
      return entries_str;
    }
    uint32_t entries = win::get_uint32_property(obj, L"SystemPaletteEntries");
    return entries > 0 ? std::to_string(entries) : "N/A";
  }

  static std::string extract_time_of_last_reset(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"TimeOfLastReset");
  }

  static std::string extract_number_of_color_planes(IWbemClassObject *obj) {
    std::string planes_str =
        win::get_string_property(obj, L"NumberOfColorPlanes");
    if (planes_str != "N/A" && !planes_str.empty()) {
      return planes_str;
    }
    uint32_t planes = win::get_uint32_property(obj, L"NumberOfColorPlanes");
    return planes > 0 ? std::to_string(planes) : "N/A";
  }

  static std::string extract_current_number_of_columns(IWbemClassObject *obj) {
    uint32_t columns = win::get_uint32_property(obj, L"CurrentNumberOfColumns");
    return columns > 0 ? std::to_string(columns) : "N/A";
  }

  static std::string extract_current_number_of_rows(IWbemClassObject *obj) {
    uint32_t rows = win::get_uint32_property(obj, L"CurrentNumberOfRows");
    return rows > 0 ? std::to_string(rows) : "N/A";
  }

public:
  // Default constructor
  GPUInfo() = default;

  // Parameterized constructor
  inline GPUInfo(
      const std::string &name, const std::string &manufacturer,
      const std::string &device_id, const std::string &video_processor,
      const std::string &adapter_ram, const std::string &adapter_dac_type,
      const std::string &video_mode_description,
      const std::string &current_horizontal_resolution,
      const std::string &current_vertical_resolution,
      const std::string &current_refresh_rate,
      const std::string &current_bits_per_pixel,
      const std::string &driver_version, const std::string &driver_date,
      const std::string &inf_filename, const std::string &inf_section,
      const std::string &pnp_device_id, const std::string &system_name,
      const std::string &adapter_type, const std::string &video_architecture,
      const std::string &video_memory_type, const std::string &max_refresh_rate,
      const std::string &min_refresh_rate,
      const std::string &max_memory_supported, const std::string &availability,
      const std::string &status, const std::string &power_management_supported,
      const std::string &current_number_of_colors,
      const std::string &current_scan_mode,
      const std::string &acceleration_caps, const std::string &caption,
      const std::string &description, const std::string &install_date,
      const std::string &last_error_code, const std::string &dither_type,
      const std::string &color_table_entries,
      const std::string &device_specific_pens, const std::string &monochrome,
      const std::string &reserved_system_palette_entries,
      const std::string &system_palette_entries,
      const std::string &time_of_last_reset,
      const std::string &number_of_color_planes,
      const std::string &current_number_of_columns,
      const std::string &current_number_of_rows)
      : name(name), manufacturer(manufacturer), device_id(device_id),
        video_processor(video_processor), adapter_ram(adapter_ram),
        adapter_dac_type(adapter_dac_type),
        video_mode_description(video_mode_description),
        current_horizontal_resolution(current_horizontal_resolution),
        current_vertical_resolution(current_vertical_resolution),
        current_refresh_rate(current_refresh_rate),
        current_bits_per_pixel(current_bits_per_pixel),
        driver_version(driver_version), driver_date(driver_date),
        inf_filename(inf_filename), inf_section(inf_section),
        pnp_device_id(pnp_device_id), system_name(system_name),
        adapter_type(adapter_type), video_architecture(video_architecture),
        video_memory_type(video_memory_type),
        max_refresh_rate(max_refresh_rate), min_refresh_rate(min_refresh_rate),
        max_memory_supported(max_memory_supported), availability(availability),
        status(status),
        power_management_supported(power_management_supported),
        current_number_of_colors(current_number_of_colors),
        current_scan_mode(current_scan_mode),
        acceleration_caps(acceleration_caps), caption(caption),
        description(description), install_date(install_date),
        last_error_code(last_error_code), dither_type(dither_type),
        color_table_entries(color_table_entries),
        device_specific_pens(device_specific_pens), monochrome(monochrome),
        reserved_system_palette_entries(reserved_system_palette_entries),
        system_palette_entries(system_palette_entries),
        time_of_last_reset(time_of_last_reset),
        number_of_color_planes(number_of_color_planes),
        current_number_of_columns(current_number_of_columns),
        current_number_of_rows(current_number_of_rows) {}

  // Getters
  inline std::string get_name() const { return name; }
  inline std::string get_manufacturer() const { return manufacturer; }
  inline std::string get_device_id() const { return device_id; }
  inline std::string get_video_processor() const { return video_processor; }
  inline std::string get_adapter_ram() const { return adapter_ram; }
  inline std::string get_adapter_dac_type() const { return adapter_dac_type; }
  inline std::string get_video_mode_description() const {
    return video_mode_description;
  }
  inline std::string get_current_horizontal_resolution() const {
    return current_horizontal_resolution;
  }
  inline std::string get_current_vertical_resolution() const {
    return current_vertical_resolution;
  }
  inline std::string get_current_refresh_rate() const {
    return current_refresh_rate;
  }
  inline std::string get_current_bits_per_pixel() const {
    return current_bits_per_pixel;
  }
  inline std::string get_driver_version() const { return driver_version; }
  inline std::string get_driver_date() const { return driver_date; }
  inline std::string get_inf_filename() const { return inf_filename; }
  inline std::string get_inf_section() const { return inf_section; }
  inline std::string get_pnp_device_id() const { return pnp_device_id; }
  inline std::string get_system_name() const { return system_name; }
  inline std::string get_adapter_type() const { return adapter_type; }
  inline std::string get_video_architecture() const {
    return video_architecture;
  }
  inline std::string get_video_memory_type() const {
    return video_memory_type;
  }
  inline std::string get_max_refresh_rate() const { return max_refresh_rate; }
  inline std::string get_min_refresh_rate() const { return min_refresh_rate; }
  inline std::string get_max_memory_supported() const {
    return max_memory_supported;
  }
  inline std::string get_availability() const { return availability; }
  inline std::string get_status() const { return status; }
  inline std::string get_power_management_supported() const {
    return power_management_supported;
  }
  inline std::string get_current_number_of_colors() const {
    return current_number_of_colors;
  }
  inline std::string get_current_scan_mode() const {
    return current_scan_mode;
  }
  inline std::string get_acceleration_caps() const {
    return acceleration_caps;
  }
  inline std::string get_caption() const { return caption; }
  inline std::string get_description() const { return description; }
  inline std::string get_install_date() const { return install_date; }
  inline std::string get_last_error_code() const { return last_error_code; }
  inline std::string get_dither_type() const { return dither_type; }
  inline std::string get_color_table_entries() const {
    return color_table_entries;
  }
  inline std::string get_device_specific_pens() const {
    return device_specific_pens;
  }
  inline std::string get_monochrome() const { return monochrome; }
  inline std::string get_reserved_system_palette_entries() const {
    return reserved_system_palette_entries;
  }
  inline std::string get_system_palette_entries() const {
    return system_palette_entries;
  }
  inline std::string get_time_of_last_reset() const {
    return time_of_last_reset;
  }
  inline std::string get_number_of_color_planes() const {
    return number_of_color_planes;
  }
  inline std::string get_current_number_of_columns() const {
    return current_number_of_columns;
  }
  inline std::string get_current_number_of_rows() const {
    return current_number_of_rows;
  }

  // Static query
  static inline std::vector<GPUInfo> query_all() {
    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;
    IEnumWbemClassObject *pEnumerator = nullptr;
    IWbemClassObject *pclsObj = nullptr;
    std::vector<GPUInfo> gpus;

    try {
      win::initialize_wmi(&pLoc, &pSvc);

      BSTR wql = SysAllocString(L"WQL");
      BSTR query = SysAllocString(L"SELECT * FROM Win32_VideoController");

      HRESULT hres = pSvc->ExecQuery(
          wql, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
          nullptr, &pEnumerator);

      SysFreeString(wql);
      SysFreeString(query);

      if (FAILED(hres)) {
        throw std::runtime_error(
            "WMI query failed for Win32_VideoController.");
      }

      ULONG uReturn = 0;

      while (pEnumerator &&
             SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) &&
             uReturn) {
        try {
          std::string name = extract_name(pclsObj);
          std::string manufacturer = extract_manufacturer(pclsObj);
          std::string device_id = extract_device_id(pclsObj);
          std::string video_processor = extract_video_processor(pclsObj);
          std::string adapter_ram = extract_adapter_ram(pclsObj);
          std::string adapter_dac_type = extract_adapter_dac_type(pclsObj);
          std::string video_mode_description =
              extract_video_mode_description(pclsObj);
          std::string current_horizontal_resolution =
              extract_current_horizontal_resolution(pclsObj);
          std::string current_vertical_resolution =
              extract_current_vertical_resolution(pclsObj);
          std::string current_refresh_rate =
              extract_current_refresh_rate(pclsObj);
          std::string current_bits_per_pixel =
              extract_current_bits_per_pixel(pclsObj);
          std::string driver_version = extract_driver_version(pclsObj);
          std::string driver_date = extract_driver_date(pclsObj);
          std::string inf_filename = extract_inf_filename(pclsObj);
          std::string inf_section = extract_inf_section(pclsObj);
          std::string pnp_device_id = extract_pnp_device_id(pclsObj);
          std::string system_name = extract_system_name(pclsObj);
          std::string adapter_type = extract_adapter_type(pclsObj);
          std::string video_architecture = extract_video_architecture(pclsObj);
          std::string video_memory_type = extract_video_memory_type(pclsObj);
          std::string max_refresh_rate = extract_max_refresh_rate(pclsObj);
          std::string min_refresh_rate = extract_min_refresh_rate(pclsObj);
          std::string max_memory_supported =
              extract_max_memory_supported(pclsObj);
          std::string availability = extract_availability(pclsObj);
          std::string status = extract_status(pclsObj);
          std::string power_management_supported =
              extract_power_management_supported(pclsObj);
          std::string current_number_of_colors =
              extract_current_number_of_colors(pclsObj);
          std::string current_scan_mode = extract_current_scan_mode(pclsObj);
          std::string acceleration_caps = extract_acceleration_caps(pclsObj);
          std::string caption = extract_caption(pclsObj);
          std::string description = extract_description(pclsObj);
          std::string install_date = extract_install_date(pclsObj);
          std::string last_error_code = extract_last_error_code(pclsObj);
          std::string dither_type = extract_dither_type(pclsObj);
          std::string color_table_entries =
              extract_color_table_entries(pclsObj);
          std::string device_specific_pens =
              extract_device_specific_pens(pclsObj);
          std::string monochrome = extract_monochrome(pclsObj);
          std::string reserved_system_palette_entries =
              extract_reserved_system_palette_entries(pclsObj);
          std::string system_palette_entries =
              extract_system_palette_entries(pclsObj);
          std::string time_of_last_reset = extract_time_of_last_reset(pclsObj);
          std::string number_of_color_planes =
              extract_number_of_color_planes(pclsObj);
          std::string current_number_of_columns =
              extract_current_number_of_columns(pclsObj);
          std::string current_number_of_rows =
              extract_current_number_of_rows(pclsObj);

          gpus.emplace_back(
              name, manufacturer, device_id, video_processor, adapter_ram,
              adapter_dac_type, video_mode_description,
              current_horizontal_resolution, current_vertical_resolution,
              current_refresh_rate, current_bits_per_pixel, driver_version,
              driver_date, inf_filename, inf_section, pnp_device_id,
              system_name, adapter_type, video_architecture,
              video_memory_type, max_refresh_rate, min_refresh_rate,
              max_memory_supported, availability, status,
              power_management_supported, current_number_of_colors,
              current_scan_mode, acceleration_caps, caption, description,
              install_date, last_error_code, dither_type, color_table_entries,
              device_specific_pens, monochrome, reserved_system_palette_entries,
              system_palette_entries, time_of_last_reset,
              number_of_color_planes, current_number_of_columns,
              current_number_of_rows);
        } catch (...) {
          throw std::runtime_error("Failed to get GPU info");
        }

        if (pclsObj) {
          pclsObj->Release();
          pclsObj = nullptr;
        }
      }

      win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
      return gpus;

    } catch (...) {
      win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
      throw std::runtime_error("Failed to get GPU info");
    }
  }
};
