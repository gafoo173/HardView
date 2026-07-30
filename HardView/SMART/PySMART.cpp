/*
================================================================================
 MIT License

 Copyright (c) 2026 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License
 See the LICENSE file in the project root for more details.
================================================================================
*/
/**
 * @version 2.0
 * Python bindings for SMART.hpp using Pybind11
 *
 * Changes vs 1.0:
 *  - Bound the remaining SmartReader methods: FillDiskInfo, GetSmartThresholds,
 *    ReadLog, ReadErrorLog, RunTest.
 *  - Bound SmartThreshold, StateByte, ErrorCommand, ErrorLogData, ErrorLog.
 *  - Bound the SSDType enum plus SMARTInfoS, GetDiskInfoS, DetectSSDType,
 *    SSDTypeToString and GetAttributeNameByIDAndType (the "controller type"
 *    detection + attribute interpretation pipeline).
 *  - Bound every per-vendor IsSsd* heuristic (used internally by
 *    DetectSSDType, exposed here too since they are free functions declared
 *    in the header).
 *  - Bound the low-level raw NVMe / SCSI helpers (GetScsiPath,
 *    GetScsiAddress, GetSmartAttributeNVMe*), each returning the raw 512-byte
 *    log page as `bytes` (or None on failure)
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <cstdio>
#include "../../C++/Headers/SMART/SMART.hpp"

namespace py = pybind11;
using namespace smart_reader;

PYBIND11_MODULE(SMART, m) {
    m.doc() = "Python bindings for SMART drive reader - HardView project";

    // ------------------------------------------------------------------
    // SmartAttribute
    // ------------------------------------------------------------------
    py::class_<SmartAttribute>(m, "SmartAttribute")
        .def(py::init<>())
        .def_readonly("id", &SmartAttribute::Id, "Attribute ID")
        .def_readonly("flags", &SmartAttribute::Flags, "Attribute flags")
        .def_readonly("current", &SmartAttribute::Current, "Current value")
        .def_readonly("worst", &SmartAttribute::Worst, "Worst value")
        .def_property_readonly("raw_value", &SmartAttribute::GetRawValue, 
            "Get raw value as 64-bit integer")
        .def_property_readonly("name", &SmartAttribute::GetAttributeName,
            "Get human-readable attribute name")
        .def("__repr__", [](const SmartAttribute& attr) {
            return "<SmartAttribute id=" + std::to_string(attr.Id) + 
                   " name='" + attr.GetAttributeName() + 
                   "' current=" + std::to_string(attr.Current) +
                   " worst=" + std::to_string(attr.Worst) +
                   " raw=" + std::to_string(attr.GetRawValue()) + ">";
        });

    // SmartThreshold (per-attribute failure thresholds, from SmartReader::GetSmartThresholds)
    py::class_<SmartThreshold>(m, "SmartThreshold")
        .def(py::init<>())
        .def_readonly("id", &SmartThreshold::Id, "Attribute ID this threshold applies to")
        .def_readonly("threshold", &SmartThreshold::Threshold, "Failure threshold value")
        .def("__repr__", [](const SmartThreshold& t) {
            return "<SmartThreshold id=" + std::to_string(t.Id) +
                   " threshold=" + std::to_string(t.Threshold) + ">";
        });

    // StateByte (decodes the device status byte found in the SMART error log)
    py::class_<StateByte>(m, "StateByte")
        .def(py::init<uint8_t>(), py::arg("byte"))
        .def_readonly("byte", &StateByte::Byte)
        .def_readonly("device_fault", &StateByte::DeviceFault)
        .def_readonly("stream_error", &StateByte::StreamError)
        .def("get_device_state", &StateByte::GetDeviceState,
            "Human-readable device state (e.g. 'Active/Idle', 'Standby')")
        .def("__repr__", [](const StateByte& s) {
            return "<StateByte state='" + s.GetDeviceState() +
                   "' device_fault=" + (s.DeviceFault ? "True" : "False") +
                   " stream_error=" + (s.StreamError ? "True" : "False") + ">";
        });

    // ErrorCommand / ErrorLogData / ErrorLog (SMART Summary Error Log, log page 0x01)
    py::class_<ErrorCommand>(m, "ErrorCommand")
        .def(py::init<>())
        .def_readonly("spvalue", &ErrorCommand::spvalue)
        .def_readonly("feature", &ErrorCommand::feature)
        .def_readonly("sector_count", &ErrorCommand::sector_count)
        .def_property_readonly("lba", [](const ErrorCommand& e) {
            return py::make_tuple(e.LBA[0], e.LBA[1], e.LBA[2]);
        }, "LBA bytes as a (low, mid, high) tuple")
        .def_readonly("device", &ErrorCommand::device)
        .def_readonly("command", &ErrorCommand::command)
        .def_readonly("timestamp", &ErrorCommand::timestamp)
        .def("__repr__", [](const ErrorCommand& e) {
            return "<ErrorCommand command=0x" +
                   [](uint8_t v){ char b[8]; snprintf(b, sizeof(b), "%02X", v); return std::string(b); }(e.command) +
                   " timestamp=" + std::to_string(e.timestamp) + ">";
        });

    py::class_<ErrorLogData>(m, "ErrorLogData")
        .def(py::init<>())
        .def_property_readonly("error_commands", [](const ErrorLogData& d) {
            return std::vector<ErrorCommand>(d.error_commands, d.error_commands + 5);
        }, "The 5 commands that preceded this error")
        .def_readonly("cerror", &ErrorLogData::cerror, "Error register at time of error")
        .def_readonly("sector_count", &ErrorLogData::sector_count)
        .def_property_readonly("lba", [](const ErrorLogData& d) {
            return py::make_tuple(d.LBA[0], d.LBA[1], d.LBA[2]);
        })
        .def_readonly("device", &ErrorLogData::device)
        .def_readonly("written_status", &ErrorLogData::written_status)
        .def_property_readonly("state", [](const ErrorLogData& d) {
            return StateByte(d.StatByte);
        }, "Decoded device state at time of error")
        .def_readonly("life_timestamp", &ErrorLogData::life_timestamp,
            "Power-on hours at the time this error occurred");

    py::class_<ErrorLog>(m, "ErrorLog")
        .def(py::init<>())
        .def_readonly("log_version", &ErrorLog::log_version)
        .def_readonly("log_index", &ErrorLog::log_index)
        .def_property_readonly("errors", [](const ErrorLog& l) {
            return std::vector<ErrorLogData>(l.errors, l.errors + 5);
        }, "Up to 5 most recent error log entries")
        .def_readonly("error_count", &ErrorLog::error_count,
            "Total number of errors logged by the device (lifetime)")
        .def_readonly("checksum", &ErrorLog::checksum)
        .def("__repr__", [](const ErrorLog& l) {
            return "<ErrorLog error_count=" + std::to_string(l.error_count) + ">";
        });

    // ------------------------------------------------------------------
    // SmartValues struct
    // ------------------------------------------------------------------
    py::class_<SmartValues>(m, "SmartValues")
        .def(py::init<>())
        .def_readonly("revision_number", &SmartValues::RevisionNumber)
        .def_readonly("offline_data_collection_status", 
            &SmartValues::OfflineDataCollectionStatus)
        .def_readonly("self_test_execution_status", 
            &SmartValues::SelfTestExecutionStatus)
        .def_readonly("total_time_to_complete_offline_data_collection",
            &SmartValues::TotalTimeToCompleteOfflineDataCollection)
        .def("__repr__", [](const SmartValues& sv) {
            return "<SmartValues revision=" + std::to_string(sv.RevisionNumber) + ">";
        });

    // ------------------------------------------------------------------
    // SmartReader class
    // ------------------------------------------------------------------
    py::class_<SmartReader>(m, "SmartReader")
        .def(py::init<int>(), py::arg("drive_number"),
            "Create SMART reader for physical drive number (0, 1, 2, ...)")
        .def(py::init<const std::string&>(), py::arg("drive_path"),
            "Create SMART reader for drive path (e.g., '\\\\.\\PhysicalDrive0')")
        
        // Properties
        .def_property_readonly("is_valid", &SmartReader::IsValid,
            "Check if SMART data was read successfully")
        .def_property_readonly("drive_path", &SmartReader::GetDrivePath,
            "Get the drive path")
        .def_property_readonly("revision_number", &SmartReader::GetRevisionNumber,
            "Get SMART revision number")
        .def_property_readonly("valid_attributes", &SmartReader::GetValidAttributes,
            "Get list of valid SMART attributes")
        .def_property_readonly("raw_data", &SmartReader::GetRawData,
            "Get raw SMART data structure")
        
        // Methods
        .def("refresh", &SmartReader::Refresh,
            "Refresh SMART data from drive")
        .def("find_attribute", &SmartReader::FindAttribute, py::arg("attribute_id"),
            "Find specific attribute by ID", py::return_value_policy::reference_internal)
        
        // Common attributes
        .def("get_temperature", &SmartReader::GetTemperature,
            "Get drive temperature in Celsius (-1 if not available)")
        .def("get_power_on_hours", &SmartReader::GetPowerOnHours,
            "Get power-on hours (0 if not available)")
        .def("get_power_cycle_count", &SmartReader::GetPowerCycleCount,
            "Get power cycle count (0 if not available)")
        .def("get_reallocated_sectors_count", &SmartReader::GetReallocatedSectorsCount,
            "Get reallocated sectors count (0 if not available)")
        
        // SSD specific
        .def("get_ssd_life_left", &SmartReader::GetSsdLifeLeft,
            "Get SSD life remaining percentage (-1 if not available)")
        .def("get_total_bytes_written", &SmartReader::GetTotalBytesWritten,
            "Get total bytes written (SSD only, 0 if not available)")
        .def("get_total_bytes_read", &SmartReader::GetTotalBytesRead,
            "Get total bytes read (SSD only, 0 if not available)")
        .def("get_wear_leveling_count", &SmartReader::GetWearLevelingCount,
            "Get wear leveling count (SSD only, 0 if not available)")
        
        // Drive type detection
        .def("is_probably_ssd", &SmartReader::IsProbablySsd,
            "Check if drive is likely an SSD")
        .def("is_probably_hdd", &SmartReader::IsProbablyHdd,
            "Check if drive is likely an HDD")
        .def("get_drive_type", &SmartReader::GetDriveType,
            "Get drive type as string ('SSD', 'HDD', or 'Unknown')")

        // --- Newly bound members ---
        .def("fill_disk_info", [](SmartReader& r) -> py::object {
            IDENTIFY_DEVICE_DATA data;
            ZeroMemory(&data, sizeof(data));
            if (!r.FillDiskInfo(data)) {
                return py::none();
            }
            py::dict result;
            result["model_number"] = trim(ByteSwapString(data.ModelNumber, 40));
            result["serial_number"] = trim(ByteSwapString(data.SerialNumber, 20));
            result["firmware_revision"] = trim(ByteSwapString(data.FirmwareRevision, 8));
            result["user_addressable_sectors"] = static_cast<unsigned long long>(data.UserAddressableSectors);
            result["nominal_media_rotation_rate"] = static_cast<int>(data.NominalMediaRotationRate);
            return result;
        }, "Send IDENTIFY DEVICE and return a dict with model_number, serial_number,\n"
           "firmware_revision, user_addressable_sectors and nominal_media_rotation_rate.\n"
           "Returns None on failure. (A simplified view of the full ATA IDENTIFY\n"
           "structure, which is not exposed field-by-field.)")

        .def("get_smart_thresholds", [](SmartReader& r) {
            std::vector<SmartThreshold> thresholds;
            r.GetSmartThresholds(thresholds);
            return thresholds;
        }, "Read the SMART attribute thresholds table, returns list[SmartThreshold]")

        .def("read_log", [](SmartReader& r, int logNumber) -> py::object {
            std::vector<uint8_t> buf(512, 0);
            if (!r.ReadLog(static_cast<UCHAR>(logNumber), buf.data())) {
                return py::none();
            }
            return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
        }, py::arg("log_number"),
           "Read a raw SMART log page (e.g. 1 = Summary Error Log) and return\n"
           "512 raw bytes, or None on failure.")

        .def("read_error_log", [](SmartReader& r) -> py::object {
            ErrorLog log{};
            if (!r.ReadErrorLog(log)) {
                return py::none();
            }
            return py::cast(log);
        }, "Read and parse the SMART Summary Error Log (log page 0x01).\n"
           "Returns an ErrorLog, or None on failure.")

        .def("run_test", &SmartReader::RunTest, py::arg("test_type") = 0x01,
            "Start a SMART self-test (SMART EXECUTE OFF-LINE IMMEDIATE).\n"
            "test_type defaults to 0x01 (short off-line test).")

        .def("__repr__", [](const SmartReader& reader) {
            return "<SmartReader path='" + reader.GetDrivePath() + 
                   "' type='" + reader.GetDriveType() + 
                   "' valid=" + (reader.IsValid() ? "True" : "False") + ">";
        });

    m.def("scan_all_drives",  
       [](int max_drives) { 
        std::vector<std::pair<int, std::string>> errors; 
        auto readers = ScanAllDrives(max_drives, &errors); 
         
        // Convert unique_ptr vector to py::list with proper ownership
        py::list result; 
        for (auto& reader : readers) { 
            result.append(py::cast(reader.release(), 
                          py::return_value_policy::take_ownership));
        } 
         
        return py::make_tuple(result, errors); 
    }, 
    py::arg("max_drives") = 8, 
    "Scan all available drives and return tuple of (readers_list, errors_list)\n" 
    "Returns: ([SmartReader, ...], [(drive_num, error_msg), ...])"
    );


    py::enum_<SSDType>(m, "SSDType", "Detected SSD controller/vendor family")
        .value("HDD_GENERAL", SSDType::HDD_GENERAL)
        .value("ADATA_INDUSTRIAL", SSDType::ADATA_INDUSTRIAL)
        .value("SANDISK", SSDType::SANDISK)
        .value("WDC", SSDType::WDC)
        .value("SEAGATE", SSDType::SEAGATE)
        .value("MTRON", SSDType::MTRON)
        .value("TOSHIBA", SSDType::TOSHIBA)
        .value("JMICRON_66X", SSDType::JMICRON_66X)
        .value("JMICRON_61X", SSDType::JMICRON_61X)
        .value("JMICRON_60X", SSDType::JMICRON_60X)
        .value("INDILINX", SSDType::INDILINX)
        .value("INTEL_DC", SSDType::INTEL_DC)
        .value("INTEL", SSDType::INTEL)
        .value("SAMSUNG", SSDType::SAMSUNG)
        .value("MICRON_MU03", SSDType::MICRON_MU03)
        .value("MICRON", SSDType::MICRON)
        .value("SANDFORCE", SSDType::SANDFORCE)
        .value("OCZ", SSDType::OCZ)
        .value("OCZ_VECTOR", SSDType::OCZ_VECTOR)
        .value("SSSTC", SSDType::SSSTC)
        .value("PLEXTOR", SSDType::PLEXTOR)
        .value("KINGSTON", SSDType::KINGSTON)
        .value("CORSAIR", SSDType::CORSAIR)
        .value("REALTEK", SSDType::REALTEK)
        .value("SK_HYNIX", SSDType::SK_HYNIX)
        .value("KIOXIA", SSDType::KIOXIA)
        .value("SILICON_MOTION_CVC", SSDType::SILICON_MOTION_CVC)
        .value("SILICON_MOTION", SSDType::SILICON_MOTION)
        .value("PHISON", SSDType::PHISON)
        .value("MARVELL", SSDType::MARVELL)
        .value("MAXIOTEK", SSDType::MAXIOTEK)
        .value("APACER", SSDType::APACER)
        .value("YMTC", SSDType::YMTC)
        .value("SCY", SSDType::SCY)
        .value("RECADATA", SSDType::RECADATA)
        .value("GENERAL_SSD", SSDType::GENERAL_SSD)
        .export_values();

    py::class_<SMARTInfoS>(m, "SMARTInfoS",
            "Model/firmware/attributes bundle used as input to detect_ssd_type()")
        .def(py::init<>())
        .def_readonly("model_upper", &SMARTInfoS::modelUpper)
        .def_readonly("attributes", &SMARTInfoS::attributes)
        .def_readonly("firmware_rev", &SMARTInfoS::firmwareRev)
        .def_readonly("is_ssd", &SMARTInfoS::IsSSD)
        .def("__repr__", [](const SMARTInfoS& info) {
            return "<SMARTInfoS model='" + info.modelUpper + "' firmware='" +
                   info.firmwareRev + "' is_ssd=" + (info.IsSSD ? "True" : "False") +
                   " attributes=" + std::to_string(info.attributes.size()) + ">";
        });

    m.def("get_disk_info_s", [](int driveNumber) -> py::object {
        try {
            SMARTInfoS info;
            if (!GetDiskInfoS(driveNumber, info)) {
                return py::none();
            }
            return py::cast(info);
        } catch (const std::exception&) {
            return py::none();
        }
    }, py::arg("drive_number"),
       "Open the given physical drive, read SMART + IDENTIFY data and return\n"
       "a SMARTInfoS ready to pass to detect_ssd_type(). Returns None on failure.");

    m.def("detect_ssd_type", [](const SMARTInfoS& info, py::object raw_smart_data) {
        if (raw_smart_data.is_none()) {
            return DetectSSDType(info);
        }
        std::string raw = py::cast<std::string>(raw_smart_data);
        return DetectSSDType(info, reinterpret_cast<const BYTE*>(raw.data()));
    }, py::arg("info"), py::arg("raw_smart_data") = py::none(),
       "Detect the SSD controller/vendor type (or HDD_GENERAL) from a SMARTInfoS.\n"
       "raw_smart_data is an optional 512-byte SMART READ DATA buffer, needed only\n"
       "to disambiguate a small number of Silicon Motion / ADATA models.");

    m.def("ssd_type_to_string", &SSDTypeToString, py::arg("type"),
        "Human-readable name for an SSDType, e.g. 'Phison', 'Samsung', 'HDD'.");

    m.def("get_attribute_name_by_id_and_type", &GetAttributeNameByIDAndType,
        py::arg("type"), py::arg("attribute_id"),
        "Vendor-specific human-readable name for a SMART attribute ID, given the\n"
        "SSDType returned by detect_ssd_type(). Falls back to a generic ATA name\n"
        "when the vendor doesn't define anything special for that ID.");

    // ------------------------------------------------------------------
    // Per-vendor detection heuristics (used internally by detect_ssd_type,
    // exposed individually since they are free functions in the header).
    // Most users only need detect_ssd_type() above.
    // ------------------------------------------------------------------

    m.def("is_ssd_old", &IsSsdOld, py::arg("model_upper"));
    m.def("is_ssd_mtron", &IsSsdMtron, py::arg("attributes"), py::arg("model_upper"), py::arg("attribute_count"));
    m.def("is_ssd_jmicron_60x", &IsSsdJMicron60x, py::arg("attributes"));
    m.def("is_ssd_jmicron_61x", &IsSsdJMicron61x, py::arg("attributes"));
    m.def("is_ssd_jmicron_66x", &IsSsdJMicron66x, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_indilinx", &IsSsdIndilinx, py::arg("attributes"));
    m.def("is_ssd_intel_dc", &IsSsdIntelDc, py::arg("model_upper"));
    m.def("is_ssd_intel", &IsSsdIntel, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_samsung", &IsSsdSamsung, py::arg("attributes"), py::arg("model_upper"), py::arg("is_ssd"));
    m.def("is_ssd_sandforce", &IsSsdSandForce, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_micron_mu03", &IsSsdMicronMU03, py::arg("model_upper"), py::arg("firmware_rev"));
    m.def("is_ssd_micron", &IsSsdMicron, py::arg("attributes"), py::arg("model_upper"), py::arg("firmware_rev"));
    m.def("is_ssd_ocz", &IsSsdOcz, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_ocz_vector", &IsSsdOczVector, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_ssstc", &IsSsdSsstc, py::arg("model_upper"));
    m.def("is_ssd_plextor", &IsSsdPlextor, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_sandisk", &IsSsdSanDisk, py::arg("model_upper"));
    m.def("is_ssd_kingston", &IsSsdKingston, py::arg("model_upper"));
    m.def("is_ssd_corsair", &IsSsdCorsair, py::arg("model_upper"));
    m.def("is_ssd_toshiba", &IsSsdToshiba, py::arg("model_upper"), py::arg("is_ssd"));
    m.def("is_ssd_realtek", &IsSsdRealtek, py::arg("attributes"));
    m.def("is_ssd_skhynix", &IsSsdSKhynix, py::arg("model_upper"));
    m.def("is_ssd_kioxia", &IsSsdKioxia, py::arg("model_upper"));
    m.def("is_ssd_apacer", &IsSsdApacer, py::arg("model_upper"), py::arg("firmware_rev"));
    m.def("is_ssd_ymtc", &IsSsdYmtc, py::arg("model_upper"));
    m.def("is_ssd_scy", &IsSsdScy, py::arg("model_upper"));
    m.def("is_ssd_recadata", &IsSsdRecadata, py::arg("model_upper"));
    m.def("is_ssd_silicon_motion_cvc", &IsSsdSiliconMotionCVC, py::arg("model_upper"));
    m.def("is_ssd_silicon_motion", [](const std::vector<SmartAttribute>& attrs,
                                       const std::string& modelUpper,
                                       const std::string& firmwareRev,
                                       py::object raw_smart_data) {
        if (raw_smart_data.is_none()) {
            return IsSsdSiliconMotion(attrs, modelUpper, firmwareRev, nullptr);
        }
        std::string raw = py::cast<std::string>(raw_smart_data);
        return IsSsdSiliconMotion(attrs, modelUpper, firmwareRev,
                                   reinterpret_cast<const BYTE*>(raw.data()));
    }, py::arg("attributes"), py::arg("model_upper"), py::arg("firmware_rev"),
       py::arg("raw_smart_data") = py::none());
    m.def("is_ssd_phison", &IsSsdPhison, py::arg("attributes"), py::arg("model_upper"), py::arg("firmware_rev"));
    m.def("is_ssd_wdc", &IsSsdWdc, py::arg("model_upper"));
    m.def("is_ssd_seagate", &IsSsdSeagate, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_marvell", &IsSsdMarvell, py::arg("attributes"), py::arg("model_upper"), py::arg("firmware_rev"));
    m.def("is_ssd_maxiotek", &IsSsdMaxiotek, py::arg("attributes"), py::arg("model_upper"));
    m.def("is_ssd_adata_industrial", &IsSsdAdataIndustrial, py::arg("model_upper"));

    m.def("get_scsi_path", &GetScsiPath, py::arg("path"),
        "Resolve a device path (e.g. '\\\\.\\PhysicalDrive0') to its underlying\n"
        "'\\\\.\\SCSIn:' path, or '' on failure.");

    m.def("get_scsi_address", [](const std::string& path) -> py::object {
        BYTE port = 0, pathId = 0, targetId = 0, lun = 0;
        if (!GetScsiAddress(path.c_str(), &port, &pathId, &targetId, &lun)) {
            return py::none();
        }
        return py::make_tuple(port, pathId, targetId, lun);
    }, py::arg("path"),
       "Get the (port, path_id, target_id, lun) SCSI address of a device path,\n"
       "or None on failure.");

    m.def("get_smart_attribute_nvme_intel", [](int driveNumber) -> py::object {
        std::vector<UCHAR> buf(512, 0);
        if (!GetSmartAttributeNVMeIntel(driveNumber, buf.data())) {
            return py::none();
        }
        return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
    }, py::arg("drive_number"),
       "Read the raw NVMe SMART/Health log page via generic Intel NVMe\n"
       "pass-through. Returns 512 bytes, or None on failure.");

    m.def("get_smart_attribute_nvme_samsung", [](int driveNumber) -> py::object {
        std::vector<UCHAR> buf(512, 0);
        if (!GetSmartAttributeNVMeSamsung(driveNumber, buf.data())) {
            return py::none();
        }
        return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
    }, py::arg("drive_number"),
       "Read the raw NVMe SMART/Health log page via Samsung's vendor-specific\n"
       "SCSI security protocol commands. Returns 512 bytes, or None on failure.");

    m.def("get_smart_attribute_nvme_storage_query", [](int driveNumber) -> py::object {
        std::vector<UCHAR> buf(512, 0);
        if (!GetSmartAttributeNVMeStorageQuery(driveNumber, buf.data())) {
            return py::none();
        }
        return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
    }, py::arg("drive_number"),
       "Read the raw NVMe SMART/Health log page via the standard Windows\n"
       "IOCTL_STORAGE_QUERY_PROPERTY protocol-specific query. Returns 512\n"
       "bytes, or None on failure. This is usually the first one to try.");

    m.def("get_smart_attribute_nvme_intel_rst", [](int driveNumber, int scsiPort, int scsiTargetId) -> py::object {
        std::vector<UCHAR> buf(512, 0);
        if (!GetSmartAttributeNVMeIntelRst(driveNumber, scsiPort, scsiTargetId, buf.data())) {
            return py::none();
        }
        return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
    }, py::arg("drive_number") = -1, py::arg("scsi_port") = 0, py::arg("scsi_target_id") = 0,
       "Read the raw NVMe SMART/Health log page through an Intel Rapid Storage\n"
       "Technology (RST) SCSI miniport pass-through. Pass drive_number=-1 with\n"
       "an explicit scsi_port/scsi_target_id if you already know the SCSI\n"
       "address; otherwise pass a physical drive number and it will be resolved\n"
       "automatically. Returns 512 bytes, or None on failure.");

    m.def("get_smart_attribute_nvme_intel_vroc", [](int driveNumber, int scsiPort, int scsiTargetId) -> py::object {
        std::vector<UCHAR> buf(512, 0);
        if (!GetSmartAttributeNVMeIntelVroc(driveNumber, scsiPort, scsiTargetId, buf.data())) {
            return py::none();
        }
        return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
    }, py::arg("drive_number") = -1, py::arg("scsi_port") = 0, py::arg("scsi_target_id") = 0,
       "Read the raw NVMe SMART/Health log page through an Intel Virtual RAID\n"
       "on CPU (VROC) SCSI miniport pass-through. Same drive_number/scsi_port/\n"
       "scsi_target_id conventions as get_smart_attribute_nvme_intel_rst().\n"
       "Returns 512 bytes, or None on failure.");
}