// example.go
// Example of using the HardwareTemp.dll library from Go.
// This example loads the library, checks for any additional dependencies,
// then tests some functions and prints the results, such as CPU Temperature


package main

import (
	"fmt"
	"math"
	"os"
	"path/filepath"
	"runtime"
	"strings"
	"syscall"
)

// HardwareTemp represents the hardware temperature monitor
type HardwareTemp struct {
	dll                *syscall.DLL
	initProc           *syscall.Proc
	shutdownProc       *syscall.Proc
	getCpuTempProc     *syscall.Proc
	getGpuTempProc     *syscall.Proc
	getMotherboardProc *syscall.Proc
	getStorageTempProc *syscall.Proc
	getAvgCpuTempProc  *syscall.Proc
	getMaxCpuTempProc  *syscall.Proc
	getCpuFanRpmProc   *syscall.Proc
	getGpuFanRpmProc   *syscall.Proc
	updateMonitorProc  *syscall.Proc
}

// callFloat64 calls a no-argument export that returns a C `double`.
// The double comes back in XMM0, which Go exposes as the second return value (r2).
func callFloat64(p *syscall.Proc) float64 {
	_, xmm0, _ := p.Call()
	return math.Float64frombits(uint64(xmm0))
}

// Check if required DLLs exist next to the executable
func checkRequiredDLLs() error {
	requiredDLLs := []string{
		"HardwareTemp.dll",
		"HardwareWrapper.dll",
		"LibreHardwareMonitorLib.dll",
		"HidSharp.dll",
	}

	// Get executable directory
	exePath, err := os.Executable()
	if err != nil {
		return fmt.Errorf("failed to get executable path: %v", err)
	}
	exeDir := filepath.Dir(exePath)

	var missing []string
	for _, dll := range requiredDLLs {
		dllPath := filepath.Join(exeDir, dll)
		if _, err := os.Stat(dllPath); os.IsNotExist(err) {
			missing = append(missing, dll)
		}
	}

	if len(missing) > 0 {
		lines := make([]string, len(missing))
		for i, dll := range missing {
			lines[i] = fmt.Sprintf("  - %s", dll)
		}
		return fmt.Errorf("ERROR: The following required DLL files are missing from the executable directory:\n%s\n\nExecutable directory: %s\n\nPlease ensure all DLL files are placed in the same directory as the executable.",
			strings.Join(lines, "\n"), exeDir)
	}

	return nil
}

// NewHardwareTemp creates a new HardwareTemp instance
func NewHardwareTemp(dllName string) (*HardwareTemp, error) {
	// Get the executable directory and construct the full DLL path
	exePath, err := os.Executable()
	if err != nil {
		return nil, fmt.Errorf("failed to get executable path: %v", err)
	}
	exeDir := filepath.Dir(exePath)
	dllPath := filepath.Join(exeDir, dllName)

	// Load the DLL from the executable directory
	dll, err := syscall.LoadDLL(dllPath)
	if err != nil {
		return nil, fmt.Errorf("failed to load DLL: %v", err)
	}

	hw := &HardwareTemp{dll: dll}

	// Load all function procedures with error handling
	funcMap := map[string]**syscall.Proc{
		"InitHardwareTempMonitor":          &hw.initProc,
		"ShutdownHardwareTempMonitor":      &hw.shutdownProc,
		"GetCpuTemperatureTemp":            &hw.getCpuTempProc,
		"GetGpuTemperatureTemp":            &hw.getGpuTempProc,
		"GetMotherboardTemperatureTemp":    &hw.getMotherboardProc,
		"GetStorageTemperatureTemp":        &hw.getStorageTempProc,
		"GetAverageCpuCoreTemperatureTemp": &hw.getAvgCpuTempProc,
		"GetMaxCpuCoreTemperatureTemp":     &hw.getMaxCpuTempProc,
		"GetCpuFanRpmTemp":                 &hw.getCpuFanRpmProc,
		"GetGpuFanRpmTemp":                 &hw.getGpuFanRpmProc,
		"UpdateHardwareMonitorTemp":        &hw.updateMonitorProc,
	}

	for funcName, procPtr := range funcMap {
		proc, err := dll.FindProc(funcName)
		if err != nil {
			dll.Release()
			return nil, fmt.Errorf("failed to find %s: %v", funcName, err)
		}
		*procPtr = proc
	}

	return hw, nil
}

// Init initializes the hardware monitor
func (hw *HardwareTemp) Init() int {
	ret, _, _ := hw.initProc.Call()
	// The export returns a C `int` (32 bits); the upper half of RAX is undefined.
	return int(int32(ret))
}

// Shutdown shuts down the hardware monitor
func (hw *HardwareTemp) Shutdown() {
	hw.shutdownProc.Call()
	hw.dll.Release()
}

// Update updates hardware monitor data
func (hw *HardwareTemp) Update() {
	hw.updateMonitorProc.Call()
}

// GetCpuTemp returns CPU temperature
func (hw *HardwareTemp) GetCpuTemp() float64 { return callFloat64(hw.getCpuTempProc) }

// GetGpuTemp returns GPU temperature
func (hw *HardwareTemp) GetGpuTemp() float64 { return callFloat64(hw.getGpuTempProc) }

// GetMotherboardTemp returns motherboard temperature
func (hw *HardwareTemp) GetMotherboardTemp() float64 { return callFloat64(hw.getMotherboardProc) }

// GetStorageTemp returns storage temperature
func (hw *HardwareTemp) GetStorageTemp() float64 { return callFloat64(hw.getStorageTempProc) }

// GetAvgCpuCoreTemp returns average CPU core temperature
func (hw *HardwareTemp) GetAvgCpuCoreTemp() float64 { return callFloat64(hw.getAvgCpuTempProc) }

// GetMaxCpuCoreTemp returns maximum CPU core temperature
func (hw *HardwareTemp) GetMaxCpuCoreTemp() float64 { return callFloat64(hw.getMaxCpuTempProc) }

// GetCpuFanRpm returns CPU fan RPM
func (hw *HardwareTemp) GetCpuFanRpm() float64 { return callFloat64(hw.getCpuFanRpmProc) }

// GetGpuFanRpm returns GPU fan RPM
func (hw *HardwareTemp) GetGpuFanRpm() float64 { return callFloat64(hw.getGpuFanRpmProc) }

// Helper function to display temperature values with error handling
func displayTempValue(name string, value float64, unit string) {
	switch value {
	case -1.0:
		fmt.Printf("%s: ERROR - Make sure the program is running as Administrator. If already running as Admin, this sensor is not supported on your system.\n", name)
	case -99.0:
		fmt.Printf("%s: ERROR - Make sure these DLLs are present next to the application:\n  - HardwareWrapper.dll\n  - HardwareTemp.dll\n  - LibreHardwareMonitorLib.dll\n", name)
	default:
		fmt.Printf("%s: %.1f %s\n", name, value, unit)
	}
}

// Helper function to display RPM values with error handling
func displayRpmValue(name string, value float64) {
	switch value {
	case -1.0:
		fmt.Printf("%s: ERROR - Make sure the program is running as Administrator. If already running as Admin, this sensor is not supported on your system.\n", name)
	case -99.0:
		fmt.Printf("%s: ERROR - Make sure these DLLs are present next to the application:\n  - HardwareWrapper.dll\n  - HardwareTemp.dll\n  - LibreHardwareMonitorLib.dll\n", name)
	default:
		fmt.Printf("%s: %.0f RPM\n", name, value)
	}
}

func main() {
	// The CLR-hosting wrapper keeps per-thread state: stay on one OS thread.
	runtime.LockOSThread()

	// Check for required DLLs before attempting to load
	if err := checkRequiredDLLs(); err != nil {
		fmt.Fprintf(os.Stderr, "%s\n", err)
		os.Exit(1)
	}

	fmt.Println("All required DLL files found. Initializing hardware monitor...")

	// Create new HardwareTemp instance
	hw, err := NewHardwareTemp("HardwareTemp.dll")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to load DLL: %v\n", err)
		os.Exit(1)
	}

	// Initialize the hardware monitor
	initResult := hw.Init()
	if initResult != 0 {
		fmt.Fprintf(os.Stderr, "Failed to init hardware monitor. Error code: %d\n", initResult)
		os.Exit(1)
	}

	fmt.Println("Hardware monitor initialized successfully!")

	// Update hardware data before reading
	hw.Update()

	fmt.Println("=== Temperature Readings ===")
	displayTempValue("CPU Temp", hw.GetCpuTemp(), "°C")
	displayTempValue("GPU Temp", hw.GetGpuTemp(), "°C")
	displayTempValue("Motherboard Temp", hw.GetMotherboardTemp(), "°C")
	displayTempValue("Storage Temp", hw.GetStorageTemp(), "°C")
	displayTempValue("Average CPU Core Temp", hw.GetAvgCpuCoreTemp(), "°C")
	displayTempValue("Max CPU Core Temp", hw.GetMaxCpuCoreTemp(), "°C")

	fmt.Println("\n=== Fan Speed Readings ===")
	displayRpmValue("CPU Fan", hw.GetCpuFanRpm())
	displayRpmValue("GPU Fan", hw.GetGpuFanRpm())

	hw.Shutdown()
	fmt.Println("\nHardware monitor shutdown successfully!")
}
