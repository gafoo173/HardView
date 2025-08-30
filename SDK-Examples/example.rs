// example.rs
// Example of using the HardwareTemp.dll library from Rust.
// This example loads the library, checks for any additional dependencies,
// then tests some functions and prints the results, such as CPU and GPU temperatures, etc.

use libloading::{Library, Symbol};
use std::os::raw::{c_double, c_int};

type InitFn = unsafe extern "C" fn() -> c_int;
type ShutdownFn = unsafe extern "C" fn();
type GetTempFn = unsafe extern "C" fn() -> c_double;
type UpdateFn = unsafe extern "C" fn();

pub struct HardwareTemp {
    // Store only the symbols - they reference the leaked library
    init: Symbol<'static, InitFn>,
    shutdown: Symbol<'static, ShutdownFn>,
    get_cpu_temp: Symbol<'static, GetTempFn>,
    get_gpu_temp: Symbol<'static, GetTempFn>,
    get_mobo_temp: Symbol<'static, GetTempFn>,
    get_storage_temp: Symbol<'static, GetTempFn>,
    get_avg_cpu_temp: Symbol<'static, GetTempFn>,
    get_max_cpu_temp: Symbol<'static, GetTempFn>,
    get_cpu_fan_rpm: Symbol<'static, GetTempFn>,
    get_gpu_fan_rpm: Symbol<'static, GetTempFn>,
    update_monitor: Symbol<'static, UpdateFn>,
}

impl HardwareTemp {
    pub fn new(path: &str) -> Result<Self, Box<dyn std::error::Error>> {
        // Get the executable directory and construct the full DLL path
        let exe_path = std::env::current_exe()?;
        let exe_dir = exe_path.parent()
            .ok_or("Failed to get executable directory")?;
        let dll_path = exe_dir.join(path);
        
        // Load the library from the executable directory
        let lib = unsafe { Library::new(&dll_path)? };
        
        // Leak the library to give it 'static lifetime
        let lib = Box::leak(Box::new(lib));

        unsafe {
            // Load functions with the correct names from your header file
            let init: Symbol<InitFn> = lib.get(b"InitHardwareTempMonitor\0")
                .map_err(|e| format!("Failed to find InitHardwareTempMonitor: {}", e))?;
            
            let shutdown: Symbol<ShutdownFn> = lib.get(b"ShutdownHardwareTempMonitor\0")
                .map_err(|e| format!("Failed to find ShutdownHardwareTempMonitor: {}", e))?;
            
            let get_cpu_temp: Symbol<GetTempFn> = lib.get(b"GetCpuTemperatureTemp\0")
                .map_err(|e| format!("Failed to find GetCpuTemperatureTemp: {}", e))?;
            
            let get_gpu_temp: Symbol<GetTempFn> = lib.get(b"GetGpuTemperatureTemp\0")
                .map_err(|e| format!("Failed to find GetGpuTemperatureTemp: {}", e))?;
            
            let get_mobo_temp: Symbol<GetTempFn> = lib.get(b"GetMotherboardTemperatureTemp\0")
                .map_err(|e| format!("Failed to find GetMotherboardTemperatureTemp: {}", e))?;
            
            let get_storage_temp: Symbol<GetTempFn> = lib.get(b"GetStorageTemperatureTemp\0")
                .map_err(|e| format!("Failed to find GetStorageTemperatureTemp: {}", e))?;
            
            let get_avg_cpu_temp: Symbol<GetTempFn> = lib.get(b"GetAverageCpuCoreTemperatureTemp\0")
                .map_err(|e| format!("Failed to find GetAverageCpuCoreTemperatureTemp: {}", e))?;
            
            let get_max_cpu_temp: Symbol<GetTempFn> = lib.get(b"GetMaxCpuCoreTemperatureTemp\0")
                .map_err(|e| format!("Failed to find GetMaxCpuCoreTemperatureTemp: {}", e))?;
            
            let get_cpu_fan_rpm: Symbol<GetTempFn> = lib.get(b"GetCpuFanRpmTemp\0")
                .map_err(|e| format!("Failed to find GetCpuFanRpmTemp: {}", e))?;
            
            let get_gpu_fan_rpm: Symbol<GetTempFn> = lib.get(b"GetGpuFanRpmTemp\0")
                .map_err(|e| format!("Failed to find GetGpuFanRpmTemp: {}", e))?;
            
            let update_monitor: Symbol<UpdateFn> = lib.get(b"UpdateHardwareMonitorTemp\0")
                .map_err(|e| format!("Failed to find UpdateHardwareMonitorTemp: {}", e))?;

            Ok(HardwareTemp {
                init,
                shutdown,
                get_cpu_temp,
                get_gpu_temp,
                get_mobo_temp,
                get_storage_temp,
                get_avg_cpu_temp,
                get_max_cpu_temp,
                get_cpu_fan_rpm,
                get_gpu_fan_rpm,
                update_monitor,
            })
        }
    }

    pub fn init(&self) -> c_int {
        unsafe { (self.init)() }
    }

    pub fn shutdown(&self) {
        unsafe { (self.shutdown)() }
    }

    pub fn update(&self) {
        unsafe { (self.update_monitor)() }
    }

    pub fn cpu_temp(&self) -> f64 {
        unsafe { (self.get_cpu_temp)() }
    }

    pub fn gpu_temp(&self) -> f64 {
        unsafe { (self.get_gpu_temp)() }
    }

    pub fn motherboard_temp(&self) -> f64 {
        unsafe { (self.get_mobo_temp)() }
    }

    pub fn storage_temp(&self) -> f64 {
        unsafe { (self.get_storage_temp)() }
    }

    pub fn avg_cpu_core_temp(&self) -> f64 {
        unsafe { (self.get_avg_cpu_temp)() }
    }

    pub fn max_cpu_core_temp(&self) -> f64 {
        unsafe { (self.get_max_cpu_temp)() }
    }

    pub fn cpu_fan_rpm(&self) -> f64 {
        unsafe { (self.get_cpu_fan_rpm)() }
    }

    pub fn gpu_fan_rpm(&self) -> f64 {
        unsafe { (self.get_gpu_fan_rpm)() }
    }
}

// Helper function to check and display temperature values
fn display_temp_value(name: &str, value: f64, unit: &str) {
    match value {
        -1.0 => println!("{}: ERROR - Make sure the program is running as Administrator. If already running as Admin, this sensor is not supported on your system.", name),
        -99.0 => println!("{}: ERROR - Make sure these DLLs are present next to the application:\n  - HardwareWrapper.dll\n  - HardwareTemp.dll\n  - LibreHardwareMonitorlib.dll", name),
        _ => println!("{}: {:.1} {}", name, value, unit),
    }
}

// Helper function to check and display RPM values
fn display_rpm_value(name: &str, value: f64) {
    match value {
        -1.0 => println!("{}: ERROR - Make sure the program is running as Administrator. If already running as Admin, this sensor is not supported on your system.", name),
        -99.0 => println!("{}: ERROR - Make sure these DLLs are present next to the application:\n  - HardwareWrapper.dll\n  - HardwareTemp.dll\n  - LibreHardwareMonitorlib.dll", name),
        _ => println!("{}: {:.0} RPM", name, value),
    }
}

// Function to check if required DLLs exist next to the executable
fn check_required_dlls() -> Result<(), String> {
    let required_dlls = [
        "HardwareTemp.dll",
        "HardwareWrapper.dll", 
        "LibreHardwareMonitorLib.dll",
        "HidSharp.dll"
    ];

    // Get the directory where the executable is located
    let exe_path = std::env::current_exe().map_err(|e| {
        format!("Failed to get executable path: {}", e)
    })?;
    
    let exe_dir = exe_path.parent().ok_or_else(|| {
        "Failed to get executable directory".to_string()
    })?;

    let mut missing_dlls = Vec::new();

    for dll in &required_dlls {
        let dll_path = exe_dir.join(dll);
        if !dll_path.exists() {
            missing_dlls.push(*dll);
        }
    }

    if !missing_dlls.is_empty() {
        let exe_dir_str = exe_dir.display();
        let error_msg = format!(
            "ERROR: The following required DLL files are missing from the executable directory:\n{}\n\nExecutable directory: {}\n\nPlease ensure all DLL files are placed in the same directory as the executable.",
            missing_dlls.iter()
                .map(|dll| format!("  - {}", dll))
                .collect::<Vec<_>>()
                .join("\n"),
            exe_dir_str
        );
        return Err(error_msg);
    }

    Ok(())
}

fn main() {
    // Check for required DLLs before attempting to load
    if let Err(error) = check_required_dlls() {
        eprintln!("{}", error);
        std::process::exit(1);
    }

    println!("All required DLL files found. Initializing hardware monitor...");

    let hw = HardwareTemp::new("HardwareTemp.dll").expect("Failed to load DLL");

    // Initialize the hardware monitor
    let init_result = hw.init();
    if init_result != 0 {
        panic!("Failed to init hardware monitor. Error code: {}", init_result);
    }

    println!("Hardware monitor initialized successfully!");
    
    // Update hardware data before reading
    hw.update();

    println!("=== Temperature Readings ===");
    display_temp_value("CPU Temp", hw.cpu_temp(), "°C");
    display_temp_value("GPU Temp", hw.gpu_temp(), "°C");
    display_temp_value("Motherboard Temp", hw.motherboard_temp(), "°C");
    display_temp_value("Storage Temp", hw.storage_temp(), "°C");
    display_temp_value("Average CPU Core Temp", hw.avg_cpu_core_temp(), "°C");
    display_temp_value("Max CPU Core Temp", hw.max_cpu_core_temp(), "°C");
    
    println!("\n=== Fan Speed Readings ===");
    display_rpm_value("CPU Fan", hw.cpu_fan_rpm());
    display_rpm_value("GPU Fan", hw.gpu_fan_rpm());

    // Clean shutdown
    hw.shutdown();
    println!("\nHardware monitor shutdown successfully!");
}