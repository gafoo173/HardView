# HardView Frequently Asked Questions (FAQ)

This FAQ addresses common issues and questions about installing, using, and troubleshooting the HardView library.

---

### Q1: **pip install fails or cannot find a suitable wheel**
**A:**
- Ensure you are using a supported platform (Windows 32/64-bit or Linux x86/x86_64).
- Make sure your Python version is supported (see PyPI for available wheels).
- Upgrade pip to the latest version:
  ```bash
  pip install --upgrade pip
  ```
- If no wheel is available for your platform/Python version, try building from source (see INSTALL.md).

---

### Q2: **HardView does not work after installation**
**A:**
- Double-check your system architecture (32/64-bit) matches the installed wheel.
- Make sure you are using the latest version of HardView.
- If you are on an unsupported OS (Arch, Android, macOS), the library will not work.
- Try uninstalling and reinstalling:
  ```bash
  pip uninstall hardview
  pip install hardview
  ```

---

### Q3: **ImportError or cannot import HardView**
**A:**
- Ensure the library file in your `site-packages` directory is named `HardView.pyd` (uppercase `H` and `V`, no architecture suffix).
    - This issue was fixed in versions >= 0.1.0. If you have an older version, upgrade HardView.
- In your Python script, import using the correct case:
  ```python
  import HardView
  ```
- Python imports are case-sensitive on Linux and sometimes on Windows.

---

### Q4: **Output data is missing or not displayed correctly**
**A:**
- HardView always returns data as a JSON string. Make sure you parse it correctly in Python:
  ```python
  import json
  import HardView
  data = json.loads(HardView.get_cpu_info())
  print(data)
  ```
- If you see `{ "error": ... }` in the output, check the error message for details.

---

### Q5: **Does HardView support Linux, macOS, or other platforms?**
**A:**
- HardView supports Windows (32/64-bit) and Linux (x86/x86_64) only.
- There is no support for Arch Linux, Android, or macOS (no native code for these platforms).

---

### Q6: **Which Python versions are supported?**
**A:**
- HardView provides wheels for multiple Python versions (see PyPI for details).
- If your version is not supported, you can try building from source.

---

### Q7: **How do I get help or report a bug?**
**A:**
- Check the documentation in the `docs/` folder first.
- If your issue is not covered, open an issue on the project's GitHub page with details about your OS, Python version, and error message.

---

### Q8: **Is HardView thread-safe?**
**A:**
- HardView is not inherently thread-safe. If you use it in multi-threaded code, ensure calls are serialized or protected by the Python GIL.

---

### Q9: **How can I contribute to HardView?**
**A:**
- See `CONTRIBUTING.md` (if available) or the main README for contribution guidelines. Pull requests and bug reports are welcome!

---

### Q10: **Why is HardView faster than other Python libraries?**
**A:**
- HardView is written entirely in C and interacts directly with the system APIs, minimizing overhead and maximizing speed.

---

### Q11: **How do I monitor hardware performance over time?**
**A:**
- Use the monitoring functions (e.g., `monitor_cpu_usage(duration, interval)`) to collect real-time data at specified intervals. See `docs/What.md` for examples.

---

### Q12: **Can I use HardView in a virtual environment?**
**A:**
- Yes, HardView works in any standard Python environment, including virtualenv and venv.

---

For more details, see the full documentation in the `docs/` folder or open an issue if your question is not answered here. 
