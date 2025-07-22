

# HardView Tools

This repository contains two tools built on top of the [HardView](https://github.com/gafoo173/hardview) hardware inspection library:

---

## 🖥️ HVapp
<img src="https://img.shields.io/badge/HVapp-1.0.1-informational" alt="HVapp" height="30">

**HVapp** is a graphical desktop application built using **[PySide6](https://doc.qt.io/qtforpython/)**, which is licensed under the [GNU Lesser General Public License v3 (LGPL-3.0)](https://www.gnu.org/licenses/lgpl-3.0.html).  
This repository includes a copy of the license in [`licenses/LICENSE.LGPL.txt`](licenses/LICENSE.LGPL.txt).

### Features:
- Modern PySide6 interface
- Categorized hardware views (CPU, RAM, GPU, etc.)
- Real-time performance statistics

---

---

## 🔐 HardID (Experimental)
<img src="https://img.shields.io/badge/HardID-2.4.0-blueviolet" alt="HardID" height="30">
**HardID** is a powerful command-line tool for generating unique hardware-based identifiers using **SHA-256 hashing**.
It uses data from the **HardView** library to collect detailed hardware info and generate consistent IDs.

---

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

---

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
