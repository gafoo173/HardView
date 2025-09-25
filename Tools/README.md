# HardView Tools

This folder contains three tools built on top of the [HardView](https://github.com/gafoo173/hardview) hardware inspection library:

---

## 🖥️ HVapp
<img src="https://img.shields.io/badge/HVapp-1.0.1-informational" alt="HVapp" height="30">

**HVapp** is a graphical desktop application built using **[PySide6](https://doc.qt.io/qtforpython/)**

### Features:
- Modern PySide6 interface
- Categorized hardware views (CPU, RAM, GPU, etc.)
- Real-time performance statistics

---

## 📊 MMT (My Monitor Tray)
> **Warning:**  
> This tool is still in the experimental stage and may be unstable at times.

<img src="https://img.shields.io/badge/MMT-Alpha-red" alt="MMT" height="30">

**MMT** is an advanced system tray monitoring application that provides real-time hardware monitoring with customizable alerts and automatic system protection features.

Built using **[PySide6](https://doc.qt.io/qtforpython/)** and **[HardView.LiveView](https://github.com/gafoo173/hardview)** library for hardware data collection.

###  Dependencies:

* **[PySide6](https://doc.qt.io/qtforpython/)** - Qt6 bindings for Python (GUI framework)
* **[psutil](https://github.com/giampaolo/psutil)** - Cross-platform system and process utilities
* **[pywin32](https://github.com/mhammond/pywin32)** - Python for Windows extensions (Windows only)

### Installation Requirements:

```bash
pip install PySide6 psutil pywin32 HardView
```

###  Usage Examples:

```bash
python MMT.py                                    # Start with default settings
python MMT.py --monitor cpu_temp --threshold 75 # Monitor CPU temperature with 75°C threshold
python MMT.py --config my_config.json           # Load settings from file
python MMT.py --monitor ram_usage --threshold 90 --alert beep  # Monitor RAM with beep alert
python MMT.py --interval 500 --no-auto-start    # 500ms updates, manual start
python MMT.py --monitor gpu_usage --alert message_restart --threshold 95
```

### ✨ Features:

* 📈 **Real-time Hardware Monitoring:**
  * CPU Usage (%), CPU Temperature (°C)
  * GPU Usage (%), GPU Temperature (°C)
  * RAM Usage (%), Disk Usage (MB/s)

* **Advanced Alert System:**
  * **Message + Restart**: Shows warning and automatically restarts system after 30 seconds
  * **Beep Alert**: Plays customizable beep sounds (frequency and duration)
  * **Message Only**: Shows warning message without system actions

* ⚙️ **Configuration Options:**
  * Customizable update intervals (100-10000 ms)
  * Adjustable alert thresholds per monitor type
  * Auto-start monitoring on application launch
  * System tray integration with minimize to tray
  * Persistent settings storage (JSON configuration)

* 🖥️ **System Tray Integration:**
  * Real-time value display in tray tooltip
  * Color-coded tray icon (green/orange/red based on status)
  * Context menu for quick control access
  * Tray notifications for alerts and status changes

*  **Status Monitoring Window:**
  * Live value display with progress bars
  * Color-coded indicators (green/orange/red)
  * System information tab
  * Alert status and configuration display

*  **Advanced Settings:**
  * Thread-safe monitoring with proper error handling
  * Alert cooldown system (60 seconds between alerts)
  * Automatic error recovery and monitor restart
  * Administrator privilege detection for temperature monitoring

### Command Line Options:

```
--config FILE            Path to configuration file
--monitor TYPE           Monitor type: cpu_usage, cpu_temp, gpu_usage, gpu_temp, ram_usage, disk_usage
--threshold VALUE        Alert threshold value (10-1000)
--interval MS            Update interval in milliseconds (100-10000)
--alert METHOD           Alert method: message_restart, beep, message_only
--beep-freq HZ           Beep frequency in Hz (37-32767)
--beep-dur MS            Beep duration in milliseconds (100-10000)
--no-auto-start          Don't start monitoring automatically
--no-tray                Don't minimize to tray
--version                Show version information
```

### Supported Monitor Types:

| Type | Description | Unit | Admin Required |
|------|-------------|------|----------------|
| `cpu_usage` | CPU Usage Percentage | % | No |
| `cpu_temp` | CPU Temperature | °C | Yes (Windows) |
| `gpu_usage` | GPU Usage Percentage | % | No |
| `gpu_temp` | GPU Temperature | °C | Yes (Windows) |
| `ram_usage` | RAM Usage Percentage | % | No |
| `disk_usage` | Disk I/O Speed | MB/s | No |

### ⚠️ Important Notices:

*  **Administrator Privileges**
  Temperature monitoring on Windows requires administrator privileges. The application will warn you if admin rights are needed.

*  **System Protection**
  The "Message + Restart" alert method will automatically restart your system when critical thresholds are exceeded to prevent hardware damage.

* **Configuration Persistence**
  All settings are automatically saved to `mmt_settings.json` and restored on application restart.

*  **Alert Cooldown**
  Alerts have a 60-second cooldown period to prevent notification spam during sustained high usage.

### 🔧 Configuration File Format:

```json
{
  "monitor_type": "CPU Usage (%)",
  "interval": 1000,
  "enable_alert": true,
  "threshold": 80,
  "alert_method": "message_only",
  "beep_frequency": 1000,
  "beep_duration": 1000,
  "auto_start": true,
  "minimize_to_tray": true,
  "show_notifications": true
}
```

---

## 🔐 HardID (Experimental)
<img src="https://img.shields.io/badge/HardID-2.4.0-blueviolet" alt="HardID" height="30">

**HardID** is a powerful command-line tool for generating unique hardware-based identifiers using **SHA-256 hashing**.

It leverages the [**HardView**](https://github.com/gafoo173/HardView) library to collect detailed and consistent hardware information across devices.

Additionally, it uses the [**Cryptography**](https://github.com/pyca/cryptography) library to provide secure encryption features license [Apache License 2.0](https://github.com/gafoo173/HardView/blob/main/licenses/LICENSE-Apache-2.0.txt)

### 🚀 Usage Examples:

```bash
python HardID.py --type cpu           # Generate ID from CPU only
python HardID.py --combined           # Generate a unified ID from all components
python HardID.py --all --dis          # Generate all component IDs (no logo)

python HardID.py -enc secret.txt -ns        # Encrypt using non-static method
python HardID.py -enc -static secret.txt    # Encrypt using static hardware hash
python HardID.py -enc -c cpu secret.txt     # Encrypt using CPU info only
python HardID.py -enc -static secret.txt -2 # Encrypt using static level 2
python HardID.py -enc -static secret.txt -2 -key # Encrypt using static level 2 And Save The Key
python HardID.py -dec -auto secret.txt.encrypted  # Auto-decrypt using hardware
python HardID.py -dec secret.txt.encrypted        # Decrypt with saved key

python HardID.py --type ram -qr         # Generate RAM ID and QR code
```

### ✨ Features:

* ✅ Generate SHA-256 hardware IDs from:
  * CPU, RAM, GPU, DISK, SMART, BIOS, SYSTEM, BASEBOARD, CHASSIS, NET
* 🔄 Combined hardware ID (all components in one hash)
* 📷 Optional QR code generation for any hardware ID
* 🧩 Advanced file encryption using hardware info:
  * **Static encryption** (no key file needed, can decrypt on same hardware)
  * **Component-based encryption** (e.g., bind file to CPU only)
  * **Non-static encryption** (uses hardware + randomness + key file)
* 🛡️ Embedded encryption headers for auto-detection during decryption
* 📂 Support for decryption with saved keys or automatically using hardware

### 🧪 Stability Levels (for static encryption):

* `-1` Level 1 *(Default)*: Includes all components (high sensitivity)
* `-2` Level 2: Excludes NET
* `-3` Level 3: Only stable components (e.g., CPU, DISK, BIOS)

### ⚠️ Important Notices:

* 🧪 **Experimental Tool**
  This tool is still under active development. Behavior and options may change. Use it with caution in production.

* 📁 **Not suitable for large file encryption**
  Files are loaded fully into memory. Avoid encrypting large files (e.g., over 100MB) to prevent memory issues.

> ⚠️ **Warning:**  
> The versions of the libraries used during encryption — especially **HardView** — **must match** the versions used during decryption.  
> Additionally, the **tool version** used to encrypt **must be the same** as the version used to decrypt.  
> Mismatched versions may lead to incorrect results or failure to decrypt the data properly.

---

# License Notice

All files in this folder are licensed under the **GPL-3**.  
The full text of the license can be found in the `LICENSE.GPL3` file located in the root of this repository.
