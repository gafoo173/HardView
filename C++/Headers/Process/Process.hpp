/*
========================================================================================================
 MIT License

 Copyright (c) 2026 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License
 See the LICENSE file in the project root for more details.
========================================================================================================
Process6 - Lightweight C++ Library for Windows Process Management (MIT Licensed)
Process6 is a lightweight C++ library for Windows (64-bit) that provides deep,
low-level control over system processes. It wraps native Windows APIs (psapi,
tlhelp32, ntdll) to expose process management features such as memory usage,
CPU times, loaded modules, priority control, and thread suspension/resumption.
It also supports direct process memory reading/writing, reading a process's
command line and current directory via its PEB, and marking a process as
critical to the system. A single ProcessControl class encapsulates all
operations per process, along with static helpers to enumerate and query all
running processes on the system.
========================================================================================================
*/
/**
 * @version 0.1Beta
 */
#include <windows.h>
#include <iostream>
#include <psapi.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <iomanip>
#include <map>
#include <Winsvc.h>
#include <processthreadsapi.h>
#include <winternl.h> 
#pragma comment(lib, "ntdll.lib")

/*
* This code is designed for 64-bit Windows and requires modifications to work on 32-bit systems.
*/

//This structs from psutil project  https://github.com/giampaolo/psutil/blob/master/psutil/arch/windows/ntextapi.h - 357-367 474-482
typedef struct {
    BYTE Reserved1[16];
    PVOID Reserved2[5];
    UNICODE_STRING CurrentDirectoryPath;
    PVOID CurrentDirectoryHandle;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    LPCWSTR env;
} RTL_USER_PROCESS_PARAMETERS_, * PRTL_USER_PROCESS_PARAMETERS_;

typedef struct {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[21];
    PVOID LoaderData;
    PRTL_USER_PROCESS_PARAMETERS_ ProcessParameters;
    // more fields...
} PEB_;

//------------------------------------------------------------------------------------------------------


struct PROCESS_INFO_EX{
    ULONG NumberOfThreads;
    std::wstring ImageName;
    KPRIORITY BasePriority;
    DWORD PriorityClass = 0;
    HANDLE UniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
};

bool GetProcessPriority(DWORD pid, DWORD& priorityOut) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProcess) return false;
    int prio = GetPriorityClass(hProcess);
    CloseHandle(hProcess);
    if (!prio) return false;
    priorityOut = prio;
    return true;
}

std::string GetPriorityName(DWORD priority) {
    switch (priority) {
    case ABOVE_NORMAL_PRIORITY_CLASS: return "ABOVE_NORMAL_PRIORITY_CLASS";
    case BELOW_NORMAL_PRIORITY_CLASS: return "BELOW_NORMAL_PRIORITY_CLASS";
    case HIGH_PRIORITY_CLASS: return "HIGH_PRIORITY_CLASS";
    case IDLE_PRIORITY_CLASS: return "IDLE_PRIORITY_CLASS";
    case NORMAL_PRIORITY_CLASS: return "NORMAL_PRIORITY_CLASS";
    case REALTIME_PRIORITY_CLASS: return "REALTIME_PRIORITY_CLASS";
    default: return "UNKNOWN_PRIORITY_CLASS";
    }
}

inline bool EnablePrivilege(LPCSTR privilegeName, HANDLE hProc = nullptr) {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    if (!hProc) hProc = GetCurrentProcess();
    if (!OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }


    if (!LookupPrivilegeValueA(NULL, privilegeName, &luid)) {
        CloseHandle(hToken);
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;


    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        CloseHandle(hToken);
        return false;
    }


    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);
    return true;
}


struct Module {
  HMODULE hModule = nullptr;
  std::string name;
  unsigned long size;
};

class ProcessControl {
  HANDLE hProcess;
  DWORD PID;
public:
  ProcessControl(DWORD pid,bool EnableDebugPrivilege = false,DWORD Access = PROCESS_ALL_ACCESS) : PID(pid) {
    if (EnableDebugPrivilege) {
      if (!EnablePrivilege("SeDebugPrivilege")) {
        throw std::runtime_error("Unable to enable SE_DEBUG_NAME privilege");
    }
    }
    hProcess = OpenProcess(Access, FALSE, PID);
    if (!hProcess) {
      throw std::runtime_error("Failed to open process Error: " +
                               std::to_string(GetLastError()));
    }
  }

  ~ProcessControl() { CloseHandle(hProcess); }

  size_t GetRamUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
      return pmc.WorkingSetSize;
    }
    return 0;
  }

  bool GetCPUTimes(size_t& kernal,size_t& user, size_t& create,size_t& exit) {
    FILETIME ftCreation, ftExit, ftKernel1, ftUser1;
    if (!GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel1,
                         &ftUser1)) {
      return false;
    }
    ULARGE_INTEGER Tmp;
    Tmp.LowPart = ftKernel1.dwLowDateTime;
    Tmp.HighPart = ftKernel1.dwHighDateTime;
    kernal = Tmp.QuadPart;

    Tmp.LowPart = ftUser1.dwLowDateTime;
    Tmp.HighPart = ftUser1.dwHighDateTime;
    user = Tmp.QuadPart;

    Tmp.LowPart = ftCreation.dwLowDateTime;
    Tmp.HighPart = ftCreation.dwHighDateTime;
    create = Tmp.QuadPart;

    Tmp.LowPart = ftExit.dwLowDateTime;
    Tmp.HighPart = ftExit.dwHighDateTime;
    exit = Tmp.QuadPart;

    return true;
  }

std::vector<Module> GetModules() {
    DWORD cbNeeded;
    HMODULE hmods[1024];
    if (EnumProcessModules(hProcess, hmods, sizeof(hmods), &cbNeeded)) {
      std::vector<Module> Modules;
      int modcount = cbNeeded / sizeof(HMODULE);
      for (int i = 0; i < modcount; i++) {
        char modnm[256];
        MODULEINFO mi;
        Module mod;
        mod.hModule = hmods[i];
        if (GetModuleFileNameExA(hProcess, hmods[i], modnm, sizeof(modnm))) {
          mod.name = modnm;
        } else mod.name = "Unknown";
        if (GetModuleInformation(hProcess, hmods[i], &mi, sizeof(mi))) {
          mod.size = mi.SizeOfImage;
        } else mod.size = 0;
        Modules.push_back(mod);
      }
      return Modules;
    }
    throw std::runtime_error("Failed to get modules");
  }



bool Suspend(bool onethread = false, DWORD threadid = 0) {
    bool found = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Failed to create thread snapshot.");

    THREADENTRY32 te;
    ZeroMemory(&te, sizeof(te));
    te.dwSize = sizeof(te);

    if (!Thread32First(hSnapshot, &te)) {
        CloseHandle(hSnapshot);
        throw std::runtime_error("Failed to retrieve first thread.");
    }

    do {
        if (te.th32OwnerProcessID == PID && (!onethread || te.th32ThreadID == threadid)) {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
            if (!hThread) {
                CloseHandle(hSnapshot);
                throw std::runtime_error("Failed to open thread.");
            }

            if (SuspendThread(hThread) == (DWORD)-1) {
                CloseHandle(hThread);
                CloseHandle(hSnapshot);
                throw std::runtime_error("Failed to suspend thread.");
            }

            CloseHandle(hThread);
            found = true;
        }
    } while (Thread32Next(hSnapshot, &te));

    CloseHandle(hSnapshot);
    return found;
}

bool Resume(bool onethread = false, DWORD threadid = 0) {
    bool found = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Failed to create thread snapshot.");

    THREADENTRY32 te;
    ZeroMemory(&te, sizeof(te));
    te.dwSize = sizeof(te);

    if (!Thread32First(hSnapshot, &te)) {
        CloseHandle(hSnapshot);
        throw std::runtime_error("Failed to retrieve first thread.");
    }

    do {
        if (te.th32OwnerProcessID == PID && (!onethread || te.th32ThreadID == threadid)) {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
            if (!hThread) {
                CloseHandle(hSnapshot);
                throw std::runtime_error("Failed to open thread.");
            }

            if (ResumeThread(hThread) == (DWORD)-1) {
                CloseHandle(hThread);
                CloseHandle(hSnapshot);
                throw std::runtime_error("Failed to resume thread.");
            }

            CloseHandle(hThread);
            found = true;
        }
    } while (Thread32Next(hSnapshot, &te));

    CloseHandle(hSnapshot);
    return found;
}

bool Kill(unsigned int exitcode = 0) {
return TerminateProcess(hProcess, exitcode);
}

PROCESSENTRY32W GetProcessInfo() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
      return {0};
    }
 PROCESSENTRY32W pe; pe.dwSize = sizeof(pe);
 if (Process32FirstW(snap, &pe)) {
      do {
        if (pe.th32ProcessID == PID) {
          CloseHandle(snap);
          return pe;
        }
      } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return {0};
}


bool ReadMemory(unsigned char* bytes, size_t size, void* address, SIZE_T* bytesRead = nullptr) {
  return ReadProcessMemory(hProcess, address, bytes, size, bytesRead);
}

bool WriteMemory(unsigned char* bytes, size_t size, void* address, SIZE_T* bytesWritten = nullptr) {
  return WriteProcessMemory(hProcess, address, bytes, size, bytesWritten);
}

bool SetPriority(DWORD priorityClass) {
    return SetPriorityClass(hProcess, priorityClass);
}

DWORD GetPriority() {
    return GetPriorityClass(hProcess);
}



PROCESS_INFO_EX GetProcessInfoEx() {
  std::vector<BYTE> buffer;
  ULONG bytesneed = 0;
  NTSTATUS st = NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &bytesneed);
  if (bytesneed == 0) {
    throw std::runtime_error("NtQuerySystemInformation failed");
  }
  buffer.resize(bytesneed);
  st = NtQuerySystemInformation(SystemProcessInformation, buffer.data(), bytesneed, &bytesneed);
  if (!NT_SUCCESS(st)) {
      throw std::runtime_error("NtQuerySystemInformation failed NTSTATUS: " + std::to_string(st));
  }
  SYSTEM_PROCESS_INFORMATION* spi = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(buffer.data());
  while (true) {
    if ((ULONGLONG)spi->UniqueProcessId == (ULONGLONG)PID) {
        PROCESS_INFO_EX info;

        info.NumberOfThreads = spi->NumberOfThreads;
        info.BasePriority = spi->BasePriority;
        info.UniqueProcessId = spi->UniqueProcessId;
        info.HandleCount = spi->HandleCount;
        info.SessionId = spi->SessionId;
        info.PeakVirtualSize = spi->PeakVirtualSize;
        info.VirtualSize = spi->VirtualSize;
        info.Reserved4 = spi->Reserved4;
        info.PeakWorkingSetSize = spi->PeakWorkingSetSize;
        info.WorkingSetSize = spi->WorkingSetSize;
        info.QuotaPagedPoolUsage = spi->QuotaPagedPoolUsage;
        info.QuotaNonPagedPoolUsage = spi->QuotaNonPagedPoolUsage;
        info.PagefileUsage = spi->PagefileUsage;
        info.PeakPagefileUsage = spi->PeakPagefileUsage;
        info.PrivatePageCount = spi->PrivatePageCount;
        if (spi->ImageName.Buffer) {
            info.ImageName = std::wstring(spi->ImageName.Buffer, spi->ImageName.Length / sizeof(WCHAR));
        }
        else {
            info.ImageName = L"";
        }
        GetProcessPriority(PID, info.PriorityClass);
        return info;
    }
    if (spi->NextEntryOffset == 0) break;
    spi = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(reinterpret_cast<BYTE*>(spi) + spi->NextEntryOffset);
  }
  return {0}; // Process not found
}
RTL_USER_PROCESS_PARAMETERS_ LoadPP() {
    PROCESS_BASIC_INFORMATION pib;
    RTL_USER_PROCESS_PARAMETERS_ PP;
    NTSTATUS st = NtQueryInformationProcess(
        hProcess,
        ProcessBasicInformation,
        &pib,
        sizeof(pib),
        nullptr
    );

    if (!NT_SUCCESS(st)) {
        return {0};
    }
    PEB_ peb;
    if (!ReadProcessMemory(hProcess, pib.PebBaseAddress, &peb, sizeof(peb), nullptr)) {
        throw std::runtime_error("Failed to Read PEB");
    }

    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &PP, sizeof(PP), nullptr)) {
        throw std::runtime_error("Failed to Read CMD Line");
    }
    return PP;
}



std::wstring Getcmdline() {
    auto PP = LoadPP();
    std::wstring x;

    x.resize((PP.CommandLine.Length / sizeof(WCHAR)));
    if (!ReadProcessMemory(hProcess, PP.CommandLine.Buffer, (void*)x.data(), PP.CommandLine.Length, nullptr)) {
        throw std::runtime_error("Failed to Read CMD Line");
    }
    return x;
}

std::wstring GetCD() {
    auto PP = LoadPP();
    std::wstring x;
    x.resize((PP.CurrentDirectoryPath.Length / sizeof(WCHAR)));
    if (!ReadProcessMemory(hProcess, PP.CurrentDirectoryPath.Buffer, (void*)x.data(), PP.CurrentDirectoryPath.Length, nullptr)) {
        throw std::runtime_error("Failed to Read Current Directory");
    }
    return x;
}


bool EnablePriv(std::string name) {
    return EnablePrivilege(name.c_str(), hProcess);
}

static std::unordered_map<std::wstring, DWORD> GetProcessMap() {
    std::unordered_map<std::wstring, DWORD> ProcMap = {};
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return ProcMap;
    }
    PROCESSENTRY32W pe; pe.dwSize = sizeof(pe);
    if (!Process32FirstW(snap, &pe)) {
        CloseHandle(snap);
        return ProcMap;
    }
        do {
            ProcMap[std::wstring(pe.szExeFile)] = pe.th32ProcessID;
        } while (Process32NextW(snap, &pe));
    
    CloseHandle(snap);
    return ProcMap;
}

static std::unordered_map<unsigned long long, PROCESS_INFO_EX> GetProcessInfoMap() {
    std::unordered_map< unsigned long long, PROCESS_INFO_EX> infomap;
    std::vector<BYTE> buffer;
    ULONG bytesneed = 0;
    NTSTATUS st = NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &bytesneed);
    if (bytesneed == 0) {
        throw std::runtime_error("NtQuerySystemInformation failed");
    }
    bytesneed = bytesneed + (bytesneed / 2);
    buffer.resize(bytesneed);
    st = NtQuerySystemInformation(SystemProcessInformation, buffer.data(), bytesneed, &bytesneed);
    if (!NT_SUCCESS(st)) {
        throw std::runtime_error("NtQuerySystemInformation failed NTSTATUS: " + std::to_string(st));
    }
    SYSTEM_PROCESS_INFORMATION* spi = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(buffer.data());
    while (true) {
        unsigned long long pid = (unsigned long long)spi->UniqueProcessId;
        PROCESS_INFO_EX info;

        info.NumberOfThreads = spi->NumberOfThreads;
        info.BasePriority = spi->BasePriority;
        info.UniqueProcessId = spi->UniqueProcessId;
        info.HandleCount = spi->HandleCount;
        info.SessionId = spi->SessionId;
        info.PeakVirtualSize = spi->PeakVirtualSize;
        info.VirtualSize = spi->VirtualSize;
        info.Reserved4 = spi->Reserved4;
        info.PeakWorkingSetSize = spi->PeakWorkingSetSize;
        info.WorkingSetSize = spi->WorkingSetSize;
        info.QuotaPagedPoolUsage = spi->QuotaPagedPoolUsage;
        info.QuotaNonPagedPoolUsage = spi->QuotaNonPagedPoolUsage;
        info.PagefileUsage = spi->PagefileUsage;
        info.PeakPagefileUsage = spi->PeakPagefileUsage;
        info.PrivatePageCount = spi->PrivatePageCount;
        if (spi->ImageName.Buffer) {
            info.ImageName = std::wstring(spi->ImageName.Buffer, spi->ImageName.Length / sizeof(WCHAR));
        }
        else {
            info.ImageName = L"";
        }
        GetProcessPriority(pid, info.PriorityClass);
        infomap[pid] = info;
        if (spi->NextEntryOffset == 0) break;
        spi = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(reinterpret_cast<BYTE*>(spi) + spi->NextEntryOffset);
    }
    return infomap;
}

static uint8_t* LowLevelProcessInfo() {
    ULONG bytesneed = (1024 * 1024);
    uint8_t* buffer = nullptr;
    while (true) {
    delete[] buffer;
    buffer = new uint8_t[bytesneed];
    NTSTATUS st = NtQuerySystemInformation(SystemProcessInformation, buffer, bytesneed, &bytesneed);

    if (st == 0xC0000004) continue;

    if (!NT_SUCCESS(st)) {
        delete[] buffer;
        throw std::runtime_error("NtQuerySystemInformation failed NTSTATUS: " + std::to_string(st));
    }
    break;
}
    return buffer;
}

static void FreeBuffer(uint8_t* buffer) {
    if (buffer) delete[] buffer;
}

DWORD IsCritcal(BOOL& value) {
    if (!IsProcessCritical(hProcess, &value)) {
        return GetLastError();
    }
    return 0;
}


};