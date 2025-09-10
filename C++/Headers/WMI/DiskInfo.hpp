#pragma once

#include "win_helpers.h"
#include <cstdint>
#include <string>
#include <vector>

class DiskInfo {
  std::string Name;           // Name
  std::string DeviceID;       // DeviceID
  std::string Model;          // Model
  std::string SerialNumber;   // SerialNumber
  std::string InterfaceType;  // InterfaceType
  std::string MediaType;      // MediaType
  uint32_t Partitions = 0;    // Partitions
  std::string Manufacturer;   // Manufacturer
  std::string PNPDeviceID;    // PNPDeviceID
  std::string Status;         // Status
  uint32_t BytesPerSector = 0; // BytesPerSector
  uint64_t TotalCylinders = 0; // TotalCylinders
  uint32_t TotalHeads = 0;     // TotalHeads
  uint64_t TotalSectors = 0;   // TotalSectors
  uint64_t TotalTracks = 0;    // TotalTracks
  uint32_t SectorsPerTrack = 0; // SectorsPerTrack
  
  // Extraction functions
  static std::string extract_name(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Name");
  }
  static std::string extract_device_id(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"DeviceID");
  }
  static std::string extract_model(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Model");
  }
  static std::string extract_serial_number(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"SerialNumber");
  }
  static std::string extract_interface_type(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"InterfaceType");
  }
  static std::string extract_media_type(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"MediaType");
  }
  static uint32_t extract_partitions(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"Partitions");
  }
  static std::string extract_manufacturer(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Manufacturer");
  }
  static std::string extract_pnp_device_id(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"PNPDeviceID");
  }
  static std::string extract_status(IWbemClassObject *obj) {
    return win::get_string_property(obj, L"Status");
  }
  static uint32_t extract_bytes_per_sector(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"BytesPerSector");
  }
  static uint64_t extract_total_cylinders(IWbemClassObject *obj) {
    return win::get_uint64_property(obj, L"TotalCylinders");
  }
  static uint32_t extract_total_heads(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"TotalHeads");
  }
  static uint64_t extract_total_sectors(IWbemClassObject *obj) {
    return win::get_uint64_property(obj, L"TotalSectors");
  }
  static uint64_t extract_total_tracks(IWbemClassObject *obj) {
    return win::get_uint64_property(obj, L"TotalTracks");
  }
  static uint32_t extract_sectors_per_track(IWbemClassObject *obj) {
    return win::get_uint32_property(obj, L"SectorsPerTrack");
  }

public:
  DiskInfo() = default;
  // Parametrized constructor
  inline DiskInfo(const std::string &name, const std::string &device_id,
                   const std::string &model, const std::string &serial_number,
                   const std::string &interface_type,
                   const std::string &media_type, uint32_t partitions,
                   const std::string &manufacturer,
                   const std::string &pnp_device_id, const std::string &status,
                   uint32_t bytes_per_sector, uint64_t total_cylinders,
                   uint32_t total_heads, uint64_t total_sectors,
                   uint64_t total_tracks, uint32_t sectors_per_track)
      : Name(name), DeviceID(device_id), Model(model),
        SerialNumber(serial_number), InterfaceType(interface_type),
        MediaType(media_type), Partitions(partitions),
        Manufacturer(manufacturer), PNPDeviceID(pnp_device_id), Status(status),
        BytesPerSector(bytes_per_sector), TotalCylinders(total_cylinders),
        TotalHeads(total_heads), TotalSectors(total_sectors),
        TotalTracks(total_tracks), SectorsPerTrack(sectors_per_track) {}
  //---------------------------------------------------------------------------
  // Getters
  inline std::string get_name() const { return Name; }
  inline std::string get_device_id() const { return DeviceID; }
  inline std::string get_model() const { return Model; }
  inline std::string get_serial_number() const { return SerialNumber; }
  inline std::string get_interface_type() const { return InterfaceType; }
  inline std::string get_media_type() const { return MediaType; }
  inline uint64_t get_size() const { return TotalSectors * BytesPerSector; }
  inline uint32_t get_partitions() const { return Partitions; }
  inline std::string get_manufacturer() const { return Manufacturer; }
  inline std::string get_pnp_device_id() const { return PNPDeviceID; }
  inline std::string get_status() const { return Status; }
  inline uint32_t get_bytes_per_sector() const { return BytesPerSector; }
  inline uint64_t get_total_cylinders() const { return TotalCylinders; }
  inline uint32_t get_total_heads() const { return TotalHeads; }
  inline uint64_t get_total_sectors() const { return TotalSectors; }
  inline uint64_t get_total_tracks() const { return TotalTracks; }
  inline uint32_t get_sectors_per_track() const { return SectorsPerTrack; }
  //---------------------------------------------------------------------------
  // Query function
  static inline std::vector<DiskInfo> query() {
    IWbemLocator *pLoc = nullptr;
    IWbemServices *pSvc = nullptr;
    IEnumWbemClassObject *pEnumerator = nullptr;
    IWbemClassObject *pclsObj = nullptr;
    std::vector<DiskInfo> info_list;
    win::initialize_wmi(&pLoc, &pSvc);

    BSTR wql = SysAllocString(L"WQL");
    BSTR query_str = SysAllocString(L"SELECT * FROM Win32_DiskDrive");

    HRESULT hres = pSvc->ExecQuery(
        wql, query_str, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr, &pEnumerator);

    SysFreeString(wql);
    SysFreeString(query_str);

    if (FAILED(hres)) {
      throw std::runtime_error("Failed to execute WMI query for disk drives.");
    }
    ULONG uReturn = 0;
    while (SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) &&
           uReturn) {
      info_list.emplace_back(
          extract_name(pclsObj), extract_device_id(pclsObj),
          extract_model(pclsObj), extract_serial_number(pclsObj),
          extract_interface_type(pclsObj), extract_media_type(pclsObj),
          extract_partitions(pclsObj), extract_manufacturer(pclsObj),
          extract_pnp_device_id(pclsObj), extract_status(pclsObj),
          extract_bytes_per_sector(pclsObj), extract_total_cylinders(pclsObj),
          extract_total_heads(pclsObj), extract_total_sectors(pclsObj),
          extract_total_tracks(pclsObj), extract_sectors_per_track(pclsObj));

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
