# ❓ HardView Frequently Asked Questions (FAQ)

This FAQ addresses common issues and questions about installing, using, and troubleshooting the HardView library.

---

### Q1: `pip install` fails or cannot find a suitable wheel

**A:**

* Ensure you are using a supported platform (Windows 32/64-bit or Linux x86/x86\_64).
* Make sure your Python version is supported (check PyPI).
* Upgrade pip to the latest version:

  ```bash
  pip install --upgrade pip
  ```
* If no wheel is available for your platform, try building from source (see `INSTALL.md`).

---

### Q2: HardView does not work after installation

**A:**

* Confirm that your system architecture (32/64-bit) matches the installed wheel.
* Ensure you have the latest version of HardView.
* Unsupported platforms like Arch, Android, or macOS are not supported.
* Try reinstalling:

  ```bash
  pip uninstall hardview
  pip install hardview
  ```

---

### Q3: `ImportError` or `cannot import HardView`

**A:**

* Verify that the binary file is named exactly `HardView.pyd` or `HardView.so` (uppercase H and V).
* Upgrade to HardView > 0.1.0 to fix import issues.
* Use correct casing in your import statement:

  ```python
  import HardView
  ```

---

### Q4: Output data is missing or not displayed correctly

**A:**

* HardView can return JSON strings or native Python objects.
* For JSON:

  ```python
  import json
  import HardView
  data = json.loads(HardView.get_cpu_info(True))
  print(data)
  ```
* For Python objects:

  ```python
  import HardView
  data_obj = HardView.get_cpu_info_objects(False)
  print(data_obj)
  ```
* If the output includes `{ "error": ... }`, check the message for details.

---

### Q5: Does HardView support Linux, macOS, or other platforms?

**A:**

* HardView supports Windows (32/64-bit) and Linux (x86/x86\_64) only.
* Not supported on Arch, Android, or macOS.

---

### Q6: Which Python versions are supported?

**A:**

* See PyPI for available wheels.
* If your version is not listed, try building from source.

---

### Q7: How do I get help or report a bug?

**A:**

* Check `docs/` first (especially `What.md`).
* If your issue persists, open a GitHub issue with OS, Python version, and error logs.

---

### Q8: Is HardView thread-safe?

**A:**

 **HardView is not thread-safe.**

---

### Q9: How can I contribute to HardView?

**A:**

* See `CONTRIBUTING.md` or main `README.md`.
* Pull requests and bug reports are welcome!

---

### Q10: Why is HardView faster than other Python libraries?

**A:**

* It's written entirely in C and communicates directly with system APIs — minimal overhead.

---

### Q11: How do I monitor hardware performance over time?

**A:**

* Use functions like `monitor_cpu_usage_duration(duration, interval)`.
* Both JSON and Python object formats are available.
* Examples are in `docs/What.md`.

---

### Q12: Can I use HardView in a virtual environment?

**A:**

* Yes. HardView works with standard virtual environments like `virtualenv` and `venv`.

---

For more details, check the full documentation in `docs/` or open an issue if your question isn’t listed.
