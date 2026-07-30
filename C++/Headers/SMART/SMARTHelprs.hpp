#include <windows.h>
#include <winioctl.h>
#include <cstdint>
#include <ntddscsi.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
namespace smart_helpers {
inline bool ReadExtLog(
    HANDLE hDevice,
    uint8_t logAddress,
    uint16_t startPage,
    uint8_t pageCount,
    uint8_t* buffer,
    uint32_t timeout = 10)
{
    if (!buffer || pageCount == 0)
        return false;

    ATA_PASS_THROUGH_DIRECT aptd = {};

    aptd.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
    aptd.TimeOutValue = timeout; // seconds

    aptd.DataTransferLength = pageCount * 512;
    aptd.DataBuffer = buffer;

    aptd.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED | ATA_FLAGS_48BIT_COMMAND;

    // HOB (bits 47:24)
    aptd.PreviousTaskFile[1] = 0x00; // Features high
    aptd.PreviousTaskFile[2] = 0x00; // Sector count high

    // Page number bits 15:8
    aptd.PreviousTaskFile[3] = 0x00; 
    aptd.PreviousTaskFile[4] = 0x00;

    // Current registers

    // Features
    aptd.CurrentTaskFile[0] = 0x00;

    // Sector Count = number of 512 byte pages
    aptd.CurrentTaskFile[1] = pageCount;

    // LBA Low = Log Address
    aptd.CurrentTaskFile[2] = logAddress;

    // LBA Mid = Page Number bits 7:0
    aptd.CurrentTaskFile[3] = startPage & 0xFF;

    aptd.CurrentTaskFile[4] = 0x00;

    aptd.PreviousTaskFile[3] = (startPage >> 8);

    aptd.PreviousTaskFile[4] = 0x00;
    // Device
    aptd.CurrentTaskFile[5] = 0x00;

    // Command
    aptd.CurrentTaskFile[6] = 0x2F;


    DWORD returned = 0;

    return DeviceIoControl(
        hDevice,
        IOCTL_ATA_PASS_THROUGH_DIRECT,
        &aptd,
        sizeof(aptd),
        &aptd,
        sizeof(aptd),
        &returned,
        nullptr
    ) && returned > 0;
}


const std::map<uint8_t, std::string> StatusMeanings = {
    {0x0, "Completed without error"},
    {0x1, "Aborted by host"},
    {0x2, "Interrupted by reset"},
    {0x3, "Fatal or unknown error - unable to complete"},
    {0x4, "Completed - unknown test element failed"},
    {0x5, "Completed - electrical test element failed"},
    {0x6, "Completed - servo/seek test element failed"},
    {0x7, "Completed - read test element failed"},
    {0x8, "Completed - handling damage (obsolete)"},
    {0xF, "Self-test in progress"},
};

inline std::string StatusText(uint8_t status_code) {
  auto it = StatusMeanings.find(status_code);
  if (it != StatusMeanings.end()) return it->second;
  char buf[32];
  std::snprintf(buf, sizeof(buf), "Unknown (0x%X)", status_code);
  return buf;
}

//name for the self-test type nibble/byte. Bit 7 set means
// this is a "captive" variant of the same test.
inline std::string TestTypeText(uint8_t self_test_type) {
  uint8_t base = self_test_type & 0x7F;
  bool captive = (self_test_type & 0x80);
  std::string name;
  switch (base) {
    case 0x00: name = "Off-line data collection"; break;
    case 0x01: name = "Short self-test"; break;
    case 0x02: name = "Extended self-test"; break;
    case 0x03: name = "Conveyance self-test"; break;
    case 0x04: name = "Selective self-test"; break;
    case 0x7F: name = "Abort self-test"; break;
    default: {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "Vendor/type 0x%02X", base);
      name = buf;
    }
  }
  if (captive) name += " (captive)";
  return name;
}

inline uint16_t ReadU16LE(const uint8_t* p) {
  return static_cast<uint16_t>(p[0] | (static_cast<uint16_t>(p[1]) << 8));
}

// failing_lba is 6 bytes, little-endian, 48-bit.
inline uint64_t ReadLba48LE(const uint8_t* p) {
  uint64_t lba = 0;
  for (int i = 5; i >= 0; --i) lba = (lba << 8) | p[i];
  return lba;
}

struct SelfTestEntry {
  int slot_index = 0;             // 0..18, raw position in the buffer
  uint8_t self_test_type = 0;
  uint8_t self_test_status = 0;
  uint8_t status_code = 0;        // top nibble of self_test_status
  uint8_t remaining = 0;        // bottom nibble of self_test_status
  uint16_t power_on_hours = 0;
  uint8_t checkpoint = 0;
  uint64_t failing_lba = 0;
  bool is_most_recent = false;
};

inline bool IsUnusedSlot(const uint8_t* entry) {
  if (entry[0] != 0) return false;
  if (entry[1] != 0) return false;
  if (entry[2] != 0 || entry[3] != 0) return false;
  return true;
}

template<typename T>
inline constexpr T mod(T a, T b)
{
    return ((a % b) + b) % b;
}

// Parses the raw Extended Self-Test Log buffer. Returns entries ordered
// most-recent-first, walking backwards through the circular 19-slot
// buffer starting at log_desc_index.
inline std::vector<SelfTestEntry> ParseExtSelfTestLog(const std::vector<uint8_t>& raw,
                                                uint8_t* out_version,
                                                uint8_t* out_checksum_byte,
                                                bool* out_checksum_ok) {
  std::vector<SelfTestEntry> raw_entries;
  if (raw.size() < 512) throw std::runtime_error("Invalid Log size");
  raw_entries.reserve(19);
  uint8_t version = raw[0];
  // raw[1] is reserved1.
  uint16_t most_recent_index = ReadU16LE(&raw[2]);
  if (most_recent_index > 19) throw std::runtime_error("Invalid Log index");
  if (most_recent_index < 1) throw std::runtime_error("Extended Self-Test Log is Empty");
  most_recent_index--; //Zero based indexing

  if (out_version) *out_version = version;

  // Checksum: the full first 512-byte sector should sum to 0 (mod 256).
  uint8_t sum = 0;
  for (size_t i = 0; i < 512; ++i) sum = static_cast<uint8_t>(sum + raw[i]);
  if (out_checksum_byte) *out_checksum_byte = raw[512 - 1];
  if (out_checksum_ok) *out_checksum_ok = (sum == 0);


  for (size_t i = 0; i < 19; ++i) {
    size_t offset = 4 + i * 26;
    const uint8_t* e = &raw[offset];

    if (IsUnusedSlot(e)) continue;

    SelfTestEntry entry;
    entry.slot_index = static_cast<int>(i);
    entry.self_test_type = e[0];
    entry.self_test_status = e[1];
    entry.status_code = static_cast<uint8_t>((e[1] >> 4) & 0x0F);
    entry.remaining = static_cast<uint8_t>(e[1] & 0x0F);
    entry.power_on_hours = ReadU16LE(&e[2]);
    entry.checkpoint = e[4];
    entry.failing_lba = ReadLba48LE(&e[5]);
    entry.is_most_recent = (i == most_recent_index);
    raw_entries.push_back(entry);
  }

  //Sort Circular buffer
  std::stable_sort(raw_entries.begin(), raw_entries.end(),
                    [most_recent_index](const SelfTestEntry& a, const SelfTestEntry& b) {
                      int da = (mod<int>((static_cast<int>(most_recent_index) - a.slot_index), static_cast<int>(19)));
                      int db = (mod<int>((static_cast<int>(most_recent_index) - b.slot_index), static_cast<int>(19)));
                      return da < db;
                    });


  return raw_entries;
}
}
