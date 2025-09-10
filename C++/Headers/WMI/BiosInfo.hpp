#pragma once

#include "win_helpers.h"
#include <string>
#include <vector>


class BiosInfo {
  std::string name;
  std::string manufacturer;
  std::string version;
  std::string smbios_version;
  std::string release_date;
  std::string serial_number;
  bool primary_bios;
  std::vector<std::string> bios_versions;
  std::string caption;
  std::string current_language;
  std::string description;
  std::string status;
  std::string software_element_id;
  std::string language_edition;

  uint16_t software_element_state;
  uint16_t installable_languages;
  uint16_t target_os;

  std::vector<std::string> list_of_languages;

  static inline std::string extract_name(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"Name");
  }
  
  static inline std::string extract_manufacturer(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"Manufacturer");
  }
  
  static inline std::string extract_version(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"Version");
  }
  
  static inline std::string extract_smbios_version(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"SMBIOSBIOSVersion");
  }
  
  static inline std::string extract_release_date(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"ReleaseDate");
  }
  
  static inline std::string extract_serial_number(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"SerialNumber");
  }
  
  static inline bool extract_primary_bios(IWbemClassObject *obj) {
      return win::get_bool_property(obj, L"PrimaryBIOS");
  }
  
  static inline std::vector<std::string> extract_bios_versions(IWbemClassObject *obj) {
      return win::get_string_array(obj, L"BIOSVersion");
  }
  
  static inline std::string extract_caption(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"Caption");
  }
  
  static inline std::string extract_current_language(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"CurrentLanguage");
  }
  
  static inline std::string extract_description(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"Description");
  }
  
  static inline std::string extract_status(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"Status");
  }
  
  static inline std::string extract_software_element_id(IWbemClassObject *obj) {
      return win::get_string_property(obj, L"SoftwareElementID");
  }
  
  static inline uint16_t extract_software_element_state(IWbemClassObject *obj) {
      return static_cast<uint16_t>(win::get_uint32_property(obj, L"SoftwareElementState"));
  }
  
  static inline uint16_t extract_installable_languages(IWbemClassObject *obj) {
      return static_cast<uint16_t>(win::get_uint32_property(obj, L"InstallableLanguages"));
  }
  
  static inline uint16_t extract_target_os(IWbemClassObject *obj) {
      return static_cast<uint16_t>(win::get_uint32_property(obj, L"TargetOperatingSystem"));
  }

public:
    BiosInfo() = default;

    // Constructor
    inline BiosInfo(const std::string &name, const std::string &manufacturer,
               const std::string &version, const std::string &smbios_version,
               const std::string &release_date, const std::string &serial_number,
               bool primary_bios, const std::vector<std::string> &bios_versions,
               const std::string &caption, const std::string &current_language,
               const std::string &description, const std::string &status,
               const std::string &software_element_id,
               uint16_t software_element_state, uint16_t installable_languages,
               uint16_t target_os)
        : name(name), manufacturer(manufacturer), version(version),
          smbios_version(smbios_version), release_date(release_date),
          serial_number(serial_number), primary_bios(primary_bios),
          bios_versions(bios_versions), caption(caption),
          current_language(current_language), description(description),
          status(status), software_element_id(software_element_id),
          software_element_state(software_element_state),
          installable_languages(installable_languages), target_os(target_os) {}

    // Getters
    inline std::string get_name() const { return name; }
    inline std::string get_manufacturer() const { return manufacturer; }
    inline std::string get_version() const { return version; }
    inline std::string get_smbios_version() const { return smbios_version; }
    inline std::string get_release_date() const { return release_date; }
    inline std::string get_serial_number() const { return serial_number; }
    inline bool get_primary_bios() const { return primary_bios; }
    inline std::vector<std::string> get_bios_versions() const { return bios_versions; }
    inline std::string get_caption() const { return caption; }
    inline std::string get_current_language() const { return current_language; }
    inline std::string get_description() const { return description; }
    inline std::string get_status() const { return status; }
    inline std::string get_software_element_id() const { return software_element_id; }
    inline uint16_t get_software_element_state() const { return software_element_state; }
    inline uint16_t get_installable_languages() const { return installable_languages; }
    inline uint16_t get_target_os() const { return target_os; }

    // Static query function
    static inline BiosInfo query() {
        IWbemLocator* pLoc = nullptr;
        IWbemServices* pSvc = nullptr;
        IEnumWbemClassObject* pEnumerator = nullptr;
        IWbemClassObject* pclsObj = nullptr;
        win::initialize_wmi(&pLoc, &pSvc);


        BSTR wql = SysAllocString(L"WQL");
        BSTR query = SysAllocString(L"SELECT * FROM Win32_BIOS");

        HRESULT hres = pSvc->ExecQuery(
            wql,
            query,
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            nullptr,
            &pEnumerator
        );

        SysFreeString(wql);
        SysFreeString(query);

        if (FAILED(hres)) {
            throw std::runtime_error("Failed to execute BIOS query.");
        }

        ULONG uReturn = 0;
        if (!(SUCCEEDED(pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn)) && uReturn)) {
            win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
            throw std::runtime_error("No BIOS data found.");
        }

        BiosInfo Info(
            extract_name(pclsObj),
            extract_manufacturer(pclsObj),
            extract_version(pclsObj),
            extract_smbios_version(pclsObj),
            extract_release_date(pclsObj),
            extract_serial_number(pclsObj),
            extract_primary_bios(pclsObj),
            extract_bios_versions(pclsObj),
            extract_caption(pclsObj),
            extract_current_language(pclsObj),
            extract_description(pclsObj),
            extract_status(pclsObj),
            extract_software_element_id(pclsObj),
            extract_software_element_state(pclsObj),
            extract_installable_languages(pclsObj),
            extract_target_os(pclsObj)
        );
        
        win::cleanup_wmi(pLoc, pSvc, pEnumerator, pclsObj);
        return Info;
    }
};
