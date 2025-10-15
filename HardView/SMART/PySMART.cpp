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
 * @version 1.0
 * Python bindings for SMART.hpp using Pybind11
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../../C++/Headers/SMART/SMART.hpp"

namespace py = pybind11;
using namespace smart_reader;

PYBIND11_MODULE(SMART, m) {
    m.doc() = "Python bindings for SMART drive reader - HardView project";

    // SmartAttribute class
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

    // SmartValues struct
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

    // SmartReader class
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
        
        .def("__repr__", [](const SmartReader& reader) {
            return "<SmartReader path='" + reader.GetDrivePath() + 
                   "' type='" + reader.GetDriveType() + 
                   "' valid=" + (reader.IsValid() ? "True" : "False") + ">";
        });

    // Utility function
    m.def("scan_all_drives", 
        [](int max_drives) {
            std::vector<std::pair<int, std::string>> errors;
            auto readers = ScanAllDrives(max_drives, &errors);
            
            // Convert unique_ptr vector to regular vector for Python
            std::vector<SmartReader*> result;
            for (auto& reader : readers) {
                result.push_back(reader.release());
            }
            
            py::tuple ret = py::make_tuple(result, errors);
            return ret;
        },
        py::arg("max_drives") = 8,
        "Scan all available drives and return tuple of (readers_list, errors_list)\n"
        "Returns: ([SmartReader, ...], [(drive_num, error_msg), ...])",
        py::return_value_policy::take_ownership
    );
}