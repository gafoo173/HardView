#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>
#include <sstream>

#include "..\..\C++\Headers\Process\Process.hpp"

namespace py = pybind11;

// ----------------------------------------------------------------------------
// Python mirror structs
// ----------------------------------------------------------------------------

struct PyModuleInfo {
    std::string name;
    unsigned long long address = 0;
    unsigned long size = 0;
};

struct PyProcessBasicInfo {
    unsigned long pid = 0;
    unsigned long parent_pid = 0;
    unsigned long thread_count = 0;
    long priority_base = 0;
    std::wstring exe_name;
};

struct PyProcessInfoEx {
    unsigned long number_of_threads = 0;
    std::wstring image_name;
    long base_priority = 0;
    unsigned long priority_class = 0;
    std::string priority_class_name;
    unsigned long long pid = 0;
    unsigned long handle_count = 0;
    unsigned long session_id = 0;
    unsigned long long peak_virtual_size = 0;
    unsigned long long virtual_size = 0;
    unsigned long long peak_working_set_size = 0;
    unsigned long long working_set_size = 0;
    unsigned long long quota_paged_pool_usage = 0;
    unsigned long long quota_nonpaged_pool_usage = 0;
    unsigned long long pagefile_usage = 0;
    unsigned long long peak_pagefile_usage = 0;
    unsigned long long private_page_count = 0;
};

struct PyCpuTimes {
    unsigned long long kernel_time = 0;
    unsigned long long user_time = 0;
    unsigned long long creation_time = 0;
    unsigned long long exit_time = 0;
};

static std::string NarrowForRepr(const std::wstring& w) {
    std::string s;
    s.reserve(w.size());
    for (wchar_t c : w) s.push_back(static_cast<char>(c));
    return s;
}

// ----------------------------------------------------------------------------
// Conversion helpers
// ---------------------------------------------------------------------------

static PyProcessInfoEx ConvertInfoEx(PROCESS_INFO_EX&& info) {
    PyProcessInfoEx out;
    out.number_of_threads = info.NumberOfThreads;
    out.image_name = std::move(info.ImageName);
    out.base_priority = info.BasePriority;
    out.priority_class = info.PriorityClass;
    out.priority_class_name = GetPriorityName(info.PriorityClass);
    out.pid = (uintptr_t)info.UniqueProcessId;
    out.handle_count = info.HandleCount;
    out.session_id = info.SessionId;
    out.peak_virtual_size = info.PeakVirtualSize;
    out.virtual_size = info.VirtualSize;
    out.peak_working_set_size = info.PeakWorkingSetSize;
    out.working_set_size = info.WorkingSetSize;
    out.quota_paged_pool_usage = info.QuotaPagedPoolUsage;
    out.quota_nonpaged_pool_usage = info.QuotaNonPagedPoolUsage;
    out.pagefile_usage = info.PagefileUsage;
    out.peak_pagefile_usage = info.PeakPagefileUsage;
    out.private_page_count = info.PrivatePageCount;
    return out;
}

// ----------------------------------------------------------------------------
// Wrapper functions calling into ProcessControl
// ----------------------------------------------------------------------------

static std::vector<PyModuleInfo> Wrap_GetModules(ProcessControl& self) {
    std::vector<PyModuleInfo> result;
    for (auto& mod : self.GetModules()) {
        PyModuleInfo pm;
        pm.name = mod.name;
        pm.address = (unsigned long long)(uintptr_t)mod.hModule;
        pm.size = mod.size;
        result.push_back(pm);
    }
    return result;
}

static PyProcessBasicInfo Wrap_GetProcessInfo(ProcessControl& self) {
    PROCESSENTRY32W pe = self.GetProcessInfo();
    PyProcessBasicInfo out;
    out.pid = pe.th32ProcessID;
    out.parent_pid = pe.th32ParentProcessID;
    out.thread_count = pe.cntThreads;
    out.priority_base = pe.pcPriClassBase;
    out.exe_name = pe.szExeFile;
    return out;
}

static PyProcessInfoEx Wrap_GetProcessInfoEx(ProcessControl& self) {
    return ConvertInfoEx(self.GetProcessInfoEx());
}

static PyCpuTimes Wrap_GetCPUTimes(ProcessControl& self) {
    size_t kernel = 0, user = 0, create = 0, exit_ = 0;
    if (!self.GetCPUTimes(kernel, user, create, exit_)) {
        throw std::runtime_error("GetProcessTimes failed, GetLastError=" +
                                  std::to_string(GetLastError()));
    }
    PyCpuTimes out;
    out.kernel_time = kernel;
    out.user_time = user;
    out.creation_time = create;
    out.exit_time = exit_;
    return out;
}

static py::bytes Wrap_ReadMemory(ProcessControl& self, unsigned long long address, size_t size) {
    std::vector<unsigned char> buffer(size);
    SIZE_T bytesRead = 0;
    if (!self.ReadMemory(buffer.data(), size, reinterpret_cast<void*>(address), &bytesRead)) {
        throw std::runtime_error("ReadMemory failed, GetLastError=" +
                                  std::to_string(GetLastError()));
    }
    return py::bytes(reinterpret_cast<const char*>(buffer.data()), bytesRead);
}

static bool Wrap_WriteMemory(ProcessControl& self, unsigned long long address, py::bytes data) {
    std::string buf = data; // pybind11 converts py::bytes -> std::string
    SIZE_T bytesWritten = 0;
    bool ok = self.WriteMemory(
        reinterpret_cast<unsigned char*>(const_cast<char*>(buf.data())),
        buf.size(),
        reinterpret_cast<void*>(address),
        &bytesWritten);
    if (!ok) {
        throw std::runtime_error("WriteMemory failed, GetLastError=" +
                                  std::to_string(GetLastError()));
    }
    return true;
}


static std::unordered_map<unsigned long long, PyProcessInfoEx> Wrap_GetProcessInfoMap() {
    std::unordered_map<unsigned long long, PyProcessInfoEx> out;
    for (auto& pr : ProcessControl::GetProcessInfoMap()) {
        out[pr.first] = ConvertInfoEx(std::move(pr.second));
    }
    return out;
}

// ----------------------------------------------------------------------------
// Module definition
// ----------------------------------------------------------------------------

PYBIND11_MODULE(process, m) {
    m.doc() = "Pybind11 bindings for the HardView ProcessControl C++ class "
              "(Windows process management: memory, priority, modules, "
              "suspend/resume, PEB command line, etc.)";

    // ---- Priority class constants ------------------------------------------
    m.attr("IDLE_PRIORITY_CLASS") = (unsigned long)IDLE_PRIORITY_CLASS;
    m.attr("BELOW_NORMAL_PRIORITY_CLASS") = (unsigned long)BELOW_NORMAL_PRIORITY_CLASS;
    m.attr("NORMAL_PRIORITY_CLASS") = (unsigned long)NORMAL_PRIORITY_CLASS;
    m.attr("ABOVE_NORMAL_PRIORITY_CLASS") = (unsigned long)ABOVE_NORMAL_PRIORITY_CLASS;
    m.attr("HIGH_PRIORITY_CLASS") = (unsigned long)HIGH_PRIORITY_CLASS;
    m.attr("REALTIME_PRIORITY_CLASS") = (unsigned long)REALTIME_PRIORITY_CLASS;
    m.attr("PROCESS_ALL_ACCESS") = (unsigned long)PROCESS_ALL_ACCESS;

    // ---- Free functions -----------------------------------------------------
    m.def("get_priority_name", &GetPriorityName, py::arg("priority"),
          "Return the human readable name of a Windows priority class value.");

    m.def(
        "enable_privilege",
        [](const std::string& name) { return EnablePrivilege(name.c_str()); },
        py::arg("privilege_name"),
        "Enable a privilege (e.g. 'SeDebugPrivilege') on the current process token.");

    // ---- ModuleInfo -----------------------------------------------------
    py::class_<PyModuleInfo>(m, "ModuleInfo")
        .def_readonly("name", &PyModuleInfo::name)
        .def_readonly("address", &PyModuleInfo::address)
        .def_readonly("size", &PyModuleInfo::size)
        .def("__repr__", [](const PyModuleInfo& mi) {
            std::ostringstream oss;
            oss << "<ModuleInfo name='" << mi.name << "' address=0x" << std::hex
                << mi.address << std::dec << " size=" << mi.size << ">";
            return oss.str();
        });

    // ---- ProcessBasicInfo -------------------------------------------------
    py::class_<PyProcessBasicInfo>(m, "ProcessBasicInfo")
        .def_readonly("pid", &PyProcessBasicInfo::pid)
        .def_readonly("parent_pid", &PyProcessBasicInfo::parent_pid)
        .def_readonly("thread_count", &PyProcessBasicInfo::thread_count)
        .def_readonly("priority_base", &PyProcessBasicInfo::priority_base)
        .def_readonly("exe_name", &PyProcessBasicInfo::exe_name)
        .def("__repr__", [](const PyProcessBasicInfo& info) {
            std::ostringstream oss;
            oss << "<ProcessBasicInfo pid=" << info.pid
                << " parent_pid=" << info.parent_pid
                << " exe_name='" << NarrowForRepr(info.exe_name) << "'>";
            return oss.str();
        });

    // ---- ProcessInfoEx ------------------------------------------------------
    py::class_<PyProcessInfoEx>(m, "ProcessInfoEx")
        .def_readonly("number_of_threads", &PyProcessInfoEx::number_of_threads)
        .def_readonly("image_name", &PyProcessInfoEx::image_name)
        .def_readonly("base_priority", &PyProcessInfoEx::base_priority)
        .def_readonly("priority_class", &PyProcessInfoEx::priority_class)
        .def_readonly("priority_class_name", &PyProcessInfoEx::priority_class_name)
        .def_readonly("pid", &PyProcessInfoEx::pid)
        .def_readonly("handle_count", &PyProcessInfoEx::handle_count)
        .def_readonly("session_id", &PyProcessInfoEx::session_id)
        .def_readonly("peak_virtual_size", &PyProcessInfoEx::peak_virtual_size)
        .def_readonly("virtual_size", &PyProcessInfoEx::virtual_size)
        .def_readonly("peak_working_set_size", &PyProcessInfoEx::peak_working_set_size)
        .def_readonly("working_set_size", &PyProcessInfoEx::working_set_size)
        .def_readonly("quota_paged_pool_usage", &PyProcessInfoEx::quota_paged_pool_usage)
        .def_readonly("quota_nonpaged_pool_usage", &PyProcessInfoEx::quota_nonpaged_pool_usage)
        .def_readonly("pagefile_usage", &PyProcessInfoEx::pagefile_usage)
        .def_readonly("peak_pagefile_usage", &PyProcessInfoEx::peak_pagefile_usage)
        .def_readonly("private_page_count", &PyProcessInfoEx::private_page_count)
        .def("__repr__", [](const PyProcessInfoEx& info) {
            std::ostringstream oss;
            oss << "<ProcessInfoEx pid=" << info.pid << " image_name='"
                << NarrowForRepr(info.image_name) << "' threads=" << info.number_of_threads
                << " working_set=" << info.working_set_size << ">";
            return oss.str();
        });

    // ---- CpuTimes -----------------------------------------------------------
    py::class_<PyCpuTimes>(m, "CpuTimes")
        .def_readonly("kernel_time", &PyCpuTimes::kernel_time)
        .def_readonly("user_time", &PyCpuTimes::user_time)
        .def_readonly("creation_time", &PyCpuTimes::creation_time)
        .def_readonly("exit_time", &PyCpuTimes::exit_time)
        .def("__repr__", [](const PyCpuTimes& t) {
            std::ostringstream oss;
            oss << "<CpuTimes kernel_time=" << t.kernel_time << " user_time=" << t.user_time
                << " creation_time=" << t.creation_time << " exit_time=" << t.exit_time << ">";
            return oss.str();
        });

    // ---- Process (wraps ProcessControl) --------------------------------
    py::class_<ProcessControl>(m, "Process")
        .def(py::init<DWORD, bool, DWORD>(), py::arg("pid"),
             py::arg("enable_debug_privilege") = false,
             py::arg("access") = (DWORD)PROCESS_ALL_ACCESS,
             "Open a handle to an existing process by PID. Set "
             "enable_debug_privilege=True to attempt enabling SeDebugPrivilege first.")

        .def("get_ram_usage", &ProcessControl::GetRamUsage,
             "Return the process working set size in bytes.")

        .def("get_cpu_times", &Wrap_GetCPUTimes,
             "Return kernel/user/creation/exit times as a CpuTimes object "
             "(raw FILETIME values, 100-ns units).")

        .def("get_modules", &Wrap_GetModules,
             "Return the list of modules (DLLs) loaded in the process as ModuleInfo objects.")

        .def("suspend", &ProcessControl::Suspend, py::arg("one_thread") = false,
             py::arg("thread_id") = 0,
             "Suspend all threads of the process, or a single thread if one_thread=True.")

        .def("resume", &ProcessControl::Resume, py::arg("one_thread") = false,
             py::arg("thread_id") = 0,
             "Resume all threads of the process, or a single thread if one_thread=True.")

        .def("kill", &ProcessControl::Kill, py::arg("exit_code") = 0,
             "Terminate the process.")

        .def("get_process_info", &Wrap_GetProcessInfo,
             "Return basic process info (pid, parent pid, thread count, priority base, exe name) "
             "via a ToolHelp32 snapshot.")

        .def("read_memory", &Wrap_ReadMemory, py::arg("address"), py::arg("size"),
             "Read 'size' bytes from process memory at 'address'. Returns bytes.")

        .def("write_memory", &Wrap_WriteMemory, py::arg("address"), py::arg("data"),
             "Write bytes 'data' to process memory at 'address'.")

        .def("set_priority", &ProcessControl::SetPriority, py::arg("priority_class"),
             "Set the process priority class (see e.g. process.NORMAL_PRIORITY_CLASS).")

        .def("get_priority", &ProcessControl::GetPriority,
             "Return the raw process priority class value.")

        .def("get_process_info_ex", &Wrap_GetProcessInfoEx,
             "Return extended process info (memory counters, handles, session, etc.) "
             "via NtQuerySystemInformation.")

        .def("get_cmdline", &ProcessControl::Getcmdline,
             "Read the process command line via its PEB.")

        .def("get_current_directory", &ProcessControl::GetCD,
             "Read the process current directory via its PEB.")

        .def("enable_privilege", &ProcessControl::EnablePriv, py::arg("privilege_name"),
             "Enable a privilege (e.g. 'SeDebugPrivilege') on this process's token.")


        .def_static("get_process_map", &ProcessControl::GetProcessMap,
                    "Return a dict {process_name: pid} for all running processes "
                    "(via ToolHelp32 snapshot).")

        .def_static("get_process_info_map", &Wrap_GetProcessInfoMap,
                    "Return a dict {pid: ProcessInfoEx} for all running processes "
                    "(via NtQuerySystemInformation).");
}