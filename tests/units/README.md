# HardView.LiveView Unit Tests

This folder contains individual unit test files for each class in the HardView.LiveView module.


## Test Files

### Cross-Platform Tests
- **PyLiveCPU.py** - Tests CPU usage monitoring and CPUID information
- **PyLiveRam.py** - Tests RAM usage monitoring (percentage and raw bytes)
- **PyLiveDisk.py** - Tests disk usage monitoring (percentage and R/W speed)
- **PyLiveNetwork.py** - Tests network traffic monitoring

### Windows-Only Tests
- **PyLiveGpu.py** - Tests GPU usage monitoring
- **PyTempCpu.py** - Tests CPU temperature and fan monitoring
- **PyTempGpu.py** - Tests GPU temperature and fan monitoring
- **PyTempOther.py** - Tests motherboard and storage temperature monitoring
- **PySensor.py** - Tests advanced sensor monitoring
- **PyManageTemp.py** - Tests temperature management functions
- **PyRawInfo.py** - Tests raw SMBIOS data access

### Linux-Only Tests
- **PyLinuxSensor.py** - Tests Linux sensor monitoring (requires lm-sensors)


## How to Run Tests

### Individual Tests
Run any individual test file:
```bash
python PyLiveCPU.py
python PyLiveRam.py
# ... etc
```


## Requirements

### Windows
- HardwareWrapper.dll (for temperature monitoring)
- Windows Vista or later

### Linux
- lm-sensors library for temperature monitoring:
  ```bash
  sudo apt-get install lm-sensors libsensors4-dev  # Ubuntu/Debian
  sudo yum install lm_sensors lm_sensors-devel     # CentOS/RHEL
  ```

## Notes

- Tests automatically detect your platform and skip unsupported features
- All tests include error handling and informative output messages

## Test Output Format

Each test provides formatted output with:
- [OK] Success messages
- [ERROR] Error messages  
- [INFO] Information messages
- [W] Warning messages

## Troubleshooting

If tests fail:
1. Ensure HardView.LiveView is properly installed
2. Check that all required dependencies are available
3. Verify hardware support for temperature monitoring features
4. Run individual tests to isolate issues

For more information about the HardView.LiveView module, refer to the main API documentation.
