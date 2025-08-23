## Overview

**HardwareWrapper** is an internal C++/CLI library for the **HardView** project that exposes hardware monitoring functions from **LibreHardwareMonitorLib** via a simple `extern "C"` interface.

This allows developers to access hardware sensors (temperature, voltage, fan speed) from C, C++, or any language that supports C-style linkage, without dealing directly with .NET or C++/CLI complexities.

## Features

* **C++/CLI Bridge**: Connects native code with the managed LibreHardwareMonitorLib.
* **Cross-Language Access**: Exported functions can be called from C, C++, or other languages with C-style linkage.
