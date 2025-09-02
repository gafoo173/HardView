import sys
import os
import argparse
import json
import time
import ctypes
import win32api
import win32con
import win32event
import winerror
import psutil
import subprocess
from threading import Lock, Timer
from typing import Optional, Dict, Any, Callable
from PySide6.QtWidgets import (QApplication, QSystemTrayIcon, QMenu, QMessageBox, 
                              QDialog, QVBoxLayout, QLabel, QLineEdit, QPushButton, 
                              QComboBox, QSpinBox, QCheckBox, QRadioButton, QButtonGroup,
                              QGroupBox, QFormLayout, QHBoxLayout, QTextEdit, QTabWidget,
                              QWidget, QProgressBar, QFrame)
from PySide6.QtGui import QIcon, QAction, QFont, QPixmap, QPainter, QColor
from PySide6.QtCore import QTimer, Qt, Signal, QObject, QThread, QMutex
from HardView.LiveView import *

class MonitorConfig:
    """Configuration management for monitors"""
    
    DEFAULT_CONFIG = {
        "monitor_type": "CPU Usage (%)",
        "interval": 1000,
        "enable_alert": True,
        "threshold": 80,
        "alert_method": "message_restart",
        "beep_frequency": 1000,
        "beep_duration": 1000,
        "auto_start": True,
        "minimize_to_tray": True,
        "show_notifications": True
    }
    
    CONFIG_FILE = "mmt_settings.json"
    
    @classmethod
    def load(cls) -> Dict[str, Any]:
        """Load configuration from file"""
        try:
            if os.path.exists(cls.CONFIG_FILE):
                with open(cls.CONFIG_FILE, "r", encoding="utf-8") as f:
                    config = json.load(f)
                # Merge with defaults to ensure all keys exist
                result = cls.DEFAULT_CONFIG.copy()
                result.update(config)
                return result
        except Exception as e:
            print(f"Error loading configuration: {e}")
        
        return cls.DEFAULT_CONFIG.copy()
    
    @classmethod
    def save(cls, config: Dict[str, Any]) -> bool:
        """Save configuration to file"""
        try:
            with open(cls.CONFIG_FILE, "w", encoding="utf-8") as f:
                json.dump(config, f, indent=2, ensure_ascii=False)
            return True
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False

class MonitorFactory:
    """Factory for creating different monitor types"""
    
    MONITOR_MAP = {
        "CPU Usage (%)": ("cpu_usage", "PyLiveCPU", "get_usage", "%"),
        "CPU Temperature (°C)": ("cpu_temp", "PyTempCpu", "get_temp", "°C"),
        "GPU Usage (%)": ("gpu_usage", "PyLiveGpu", "get_usage", "%"),
        "GPU Temperature (°C)": ("gpu_temp", "PyTempGpu", "get_temp", "°C"),
        "RAM Usage (%)": ("ram_usage", "PyLiveRam", "get_usage", "%"),
        "Disk Usage (MB/s)": ("disk_usage", "PyLiveDisk", "get_usage", "MB/s")
    }
    
    @classmethod
    def create_monitor(cls, monitor_type: str, interval: int = 1000):
        """Create a monitor instance based on type"""
        if monitor_type not in cls.MONITOR_MAP:
            raise ValueError(f"Unsupported monitor type: {monitor_type}")
        
        key, class_name, method_name, unit = cls.MONITOR_MAP[monitor_type]
        
        try:
            if class_name == "PyLiveCPU":
                monitor = PyLiveCPU()
                get_value = lambda: monitor.get_usage(interval)
            elif class_name == "PyTempCpu":
                monitor = PyTempCpu()
                get_value = lambda: monitor.get_temp()
            elif class_name == "PyLiveGpu":
                monitor = PyLiveGpu()
                get_value = lambda: monitor.get_usage(interval)
            elif class_name == "PyTempGpu":
                monitor = PyTempGpu()
                get_value = lambda: monitor.get_temp()
            elif class_name == "PyLiveRam":
                monitor = PyLiveRam()
                get_value = lambda: monitor.get_usage()
            elif class_name == "PyLiveDisk":
                monitor = PyLiveDisk(mode=1)
                get_value = lambda: sum(speed[1] for speed in monitor.get_usage(interval))
            else:
                raise ValueError(f"Unknown monitor class: {class_name}")
            
            return monitor, get_value, unit
            
        except Exception as e:
            raise RuntimeError(f"Failed to create monitor {monitor_type}: {e}")

class SystemMonitor(QObject):
    """Thread-safe system monitoring with proper error handling"""
    
    value_updated = Signal(float)
    error_occurred = Signal(str)
    alert_triggered = Signal(float, float)  # current_value, threshold
    
    def __init__(self, monitor_type: str, interval: int = 1000):
        super().__init__()
        self.monitor_type = monitor_type
        self.interval = interval
        self.current_value = 0.0
        self.is_running = False
        self.mutex = QMutex()
        self.error_count = 0
        self.max_errors = 5
        
        # Initialize monitor
        try:
            self.monitor, self.get_value, self.unit = MonitorFactory.create_monitor(monitor_type, interval)
        except Exception as e:
            self.error_occurred.emit(f"Monitor initialization failed: {e}")
            raise
        
        # Alert settings
        self.threshold = None
        self.alert_enabled = False
        self.alert_method = "message_only"
        self.beep_frequency = 1000
        self.beep_duration = 1000
        self.last_alert_time = 0
        self.alert_cooldown = 60  # 60 seconds between alerts
        
        # Setup timer
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_value)
    
    def configure_alerts(self, enabled: bool, threshold: float, method: str, 
                        beep_freq: int = 1000, beep_dur: int = 1000):
        """Configure alert settings"""
        with QMutexLocker(self.mutex):
            self.alert_enabled = enabled
            self.threshold = threshold
            self.alert_method = method
            self.beep_frequency = beep_freq
            self.beep_duration = beep_dur
    
    def start_monitoring(self):
        """Start the monitoring process"""
        if not self.is_running:
            self.is_running = True
            self.error_count = 0
            self.timer.start(self.interval)
    
    def stop_monitoring(self):
        """Stop the monitoring process"""
        if self.is_running:
            self.is_running = False
            self.timer.stop()
    
    def update_value(self):
        """Update the monitored value"""
        if not self.is_running:
            return
        
        try:
            # Get new value
            if self.monitor_type == "CPU Temperature (°C)" or self.monitor_type == "GPU Temperature (°C)":
                # Update temperature monitors
                if hasattr(self.monitor, 'update'):
                    self.monitor.update()
            
            value = self.get_value()
            
            with QMutexLocker(self.mutex):
                self.current_value = value
                self.error_count = 0  # Reset error count on success
            
            self.value_updated.emit(value)
            
            # Check for alerts
            self.check_alert(value)
            
        except Exception as e:
            self.error_count += 1
            error_msg = f"Error reading {self.monitor_type}: {e}"
            self.error_occurred.emit(error_msg)
            
            # Stop monitoring if too many consecutive errors
            if self.error_count >= self.max_errors:
                self.error_occurred.emit(f"Too many errors ({self.max_errors}), stopping monitor")
                self.stop_monitoring()
    
    def check_alert(self, value: float):
        """Check if alert should be triggered"""
        if not self.alert_enabled or self.threshold is None:
            return
        
        if value >= self.threshold:
            current_time = time.time()
            # Implement cooldown to prevent spam
            if current_time - self.last_alert_time > self.alert_cooldown:
                self.last_alert_time = current_time
                self.alert_triggered.emit(value, self.threshold)
                self.trigger_alert(value)
    
    def trigger_alert(self, value: float):
        """Trigger the configured alert"""
        if self.alert_method == "message_restart":
            self.show_restart_warning(value)
        elif self.alert_method == "beep":
            self.play_beep()
        elif self.alert_method == "message_only":
            self.show_warning_message(value)
    
    def show_restart_warning(self, value: float):
        """Show restart warning dialog"""
        reply = QMessageBox.warning(
            None,
            "MMT - Critical Alert",
            f"CRITICAL: {self.monitor_type} has reached {value:.2f} {self.unit}\n"
            f"Threshold: {self.threshold} {self.unit}\n\n"
            f"System will restart in 30 seconds to prevent damage.\n"
            f"Click OK to restart immediately, or Cancel to abort restart.",
            QMessageBox.Ok | QMessageBox.Cancel,
            QMessageBox.Ok
        )
        
        if reply == QMessageBox.Ok:
            # Immediate restart
            self.restart_system()
        else:
            # Schedule restart after 30 seconds
            QTimer.singleShot(30000, self.restart_system)
    
    def play_beep(self):
        """Play beep sound"""
        try:
            win32api.Beep(self.beep_frequency, self.beep_duration)
        except Exception as e:
            print(f"Error playing beep: {e}")
    
    def show_warning_message(self, value: float):
        """Show warning message only"""
        QMessageBox.warning(
            None,
            "MMT - Alert",
            f"Warning: {self.monitor_type} has exceeded threshold!\n\n"
            f"Current: {value:.2f} {self.unit}\n"
            f"Threshold: {self.threshold} {self.unit}",
            QMessageBox.Ok
        )
    
    def restart_system(self):
        """Restart the system"""
        try:
            if os.name == 'nt':  # Windows
                os.system("shutdown /r /t 0")
            else:  # Linux/Unix
                os.system("sudo reboot")
        except Exception as e:
            print(f"Error restarting system: {e}")
    
    def get_current_value(self) -> float:
        """Get current monitored value thread-safely"""
        with QMutexLocker(self.mutex):
            return self.current_value

class QMutexLocker:
    """Context manager for QMutex"""
    def __init__(self, mutex):
        self.mutex = mutex
    
    def __enter__(self):
        self.mutex.lock()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.mutex.unlock()

class StatusDialog(QDialog):
    """Real-time status monitoring dialog"""
    
    def __init__(self, monitor: SystemMonitor, parent=None):
        super().__init__(parent)
        self.monitor = monitor
        self.setWindowTitle("MMT - System Monitor Status")
        self.setFixedSize(500, 400)
        self.init_ui()
        
        # Connect signals
        self.monitor.value_updated.connect(self.update_display)
        self.monitor.error_occurred.connect(self.show_error)
        
        # Update timer
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self.update_info)
        self.update_timer.start(1000)
    
    def init_ui(self):
        layout = QVBoxLayout()
        
        # Create tabs
        tabs = QTabWidget()
        
        # Status tab
        status_widget = QWidget()
        status_layout = QVBoxLayout()
        
        # Current value display
        self.value_label = QLabel("Initializing...")
        self.value_label.setFont(QFont("Arial", 16, QFont.Bold))
        self.value_label.setAlignment(Qt.AlignCenter)
        self.value_label.setStyleSheet("QLabel { background-color: #f0f0f0; padding: 10px; border-radius: 5px; }")
        
        # Progress bar
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        
        # Alert status
        self.alert_status = QLabel("Alert: Disabled")
        self.alert_status.setAlignment(Qt.AlignCenter)
        
        # Error display
        self.error_display = QTextEdit()
        self.error_display.setMaximumHeight(100)
        self.error_display.setReadOnly(True)
        
        status_layout.addWidget(QLabel("Current Value:"))
        status_layout.addWidget(self.value_label)
        status_layout.addWidget(self.progress_bar)
        status_layout.addWidget(self.alert_status)
        status_layout.addWidget(QLabel("Error Log:"))
        status_layout.addWidget(self.error_display)
        
        status_widget.setLayout(status_layout)
        tabs.addTab(status_widget, "Status")
        
        # Info tab
        info_widget = QWidget()
        info_layout = QVBoxLayout()
        
        self.info_display = QTextEdit()
        self.info_display.setReadOnly(True)
        info_layout.addWidget(self.info_display)
        
        info_widget.setLayout(info_layout)
        tabs.addTab(info_widget, "System Info")
        
        layout.addWidget(tabs)
        
        # Close button
        close_button = QPushButton("Close")
        close_button.clicked.connect(self.accept)
        layout.addWidget(close_button)
        
        self.setLayout(layout)
    
    def update_display(self, value: float):
        """Update the display with new value"""
        monitor_type = self.monitor.monitor_type
        unit = self.monitor.unit
        
        self.value_label.setText(f"{value:.2f} {unit}")
        
        # Update progress bar (for percentage values)
        if unit == "%":
            self.progress_bar.setValue(min(int(value), 100))
            # Color coding
            if value > 90:
                self.progress_bar.setStyleSheet("QProgressBar::chunk { background-color: red; }")
            elif value > 70:
                self.progress_bar.setStyleSheet("QProgressBar::chunk { background-color: orange; }")
            else:
                self.progress_bar.setStyleSheet("QProgressBar::chunk { background-color: green; }")
        else:
            self.progress_bar.setValue(min(int(value), 100))
        
        # Update alert status
        if self.monitor.alert_enabled and self.monitor.threshold:
            threshold = self.monitor.threshold
            if value >= threshold:
                self.alert_status.setText(f"🚨 ALERT ACTIVE: {value:.2f} >= {threshold} {unit}")
                self.alert_status.setStyleSheet("color: red; font-weight: bold;")
            else:
                remaining = threshold - value
                self.alert_status.setText(f"Alert: {remaining:.2f} {unit} below threshold")
                self.alert_status.setStyleSheet("color: green;")
        else:
            self.alert_status.setText("Alert: Disabled")
            self.alert_status.setStyleSheet("color: gray;")
    
    def show_error(self, error_msg: str):
        """Display error message"""
        timestamp = time.strftime("%H:%M:%S")
        self.error_display.append(f"[{timestamp}] {error_msg}")
        
        # Auto-scroll to bottom
        scrollbar = self.error_display.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())
    
    def update_info(self):
        """Update system info tab"""
        try:
            info_text = f"""MMT System Monitor Information
Monitor Type: {self.monitor.monitor_type}
Update Interval: {self.monitor.interval} ms
Current Value: {self.monitor.get_current_value():.2f} {self.monitor.unit}
Monitor Status: {"Running" if self.monitor.is_running else "Stopped"}

Alert Configuration:
- Enabled: {self.monitor.alert_enabled}
- Threshold: {self.monitor.threshold} {self.monitor.unit if self.monitor.threshold else ""}
- Method: {self.monitor.alert_method}
- Cooldown: {self.monitor.alert_cooldown} seconds

System Information:
- Platform: {sys.platform}
- CPU Count: {psutil.cpu_count()}
- Total RAM: {psutil.virtual_memory().total / (1024**3):.2f} GB
- Python Version: {sys.version.split()[0]}
- PySide6: Available

Error Statistics:
- Error Count: {self.monitor.error_count}
- Max Errors: {self.monitor.max_errors}
"""
            self.info_display.setPlainText(info_text)
        except Exception as e:
            self.info_display.setPlainText(f"Error updating info: {e}")

class SettingsDialog(QDialog):
    """Enhanced settings dialog with validation"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("MMT - Settings")
        self.setFixedSize(450, 600)
        self.init_ui()
        self.load_settings()
    
    def init_ui(self):
        layout = QVBoxLayout()
        
        # Monitor settings group
        monitor_group = QGroupBox("Monitor Settings")
        monitor_layout = QFormLayout()
        
        self.monitor_type = QComboBox()
        self.monitor_type.addItems([
            "CPU Usage (%)",
            "CPU Temperature (°C)",
            "GPU Usage (%)",
            "GPU Temperature (°C)",
            "RAM Usage (%)",
            "Disk Usage (MB/s)"
        ])
        
        self.interval = QSpinBox()
        self.interval.setRange(100, 10000)
        self.interval.setValue(1000)
        self.interval.setSuffix(" ms")
        
        monitor_layout.addRow("Monitor Type:", self.monitor_type)
        monitor_layout.addRow("Update Interval:", self.interval)
        
        monitor_group.setLayout(monitor_layout)
        layout.addWidget(monitor_group)
        
        # Alert settings group
        alert_group = QGroupBox("Alert Settings")
        alert_layout = QVBoxLayout()
        
        self.enable_alert = QCheckBox("Enable Alert System")
        self.enable_alert.setChecked(True)
        
        # Threshold settings
        threshold_layout = QHBoxLayout()
        threshold_layout.addWidget(QLabel("Alert Threshold:"))
        self.threshold = QSpinBox()
        self.threshold.setRange(0, 1000)
        self.threshold.setValue(80)
        threshold_layout.addWidget(self.threshold)
        
        # Alert method group
        alert_method_group = QGroupBox("Alert Method")
        alert_method_layout = QVBoxLayout()
        
        self.message_restart = QRadioButton("Show message and restart system")
        self.beep_alert = QRadioButton("Play beep sound")
        self.message_only = QRadioButton("Show message only")
        
        self.message_restart.setChecked(True)
        
        alert_method_layout.addWidget(self.message_restart)
        alert_method_layout.addWidget(self.beep_alert)
        alert_method_layout.addWidget(self.message_only)
        
        alert_method_group.setLayout(alert_method_layout)
        
        # Beep settings
        beep_group = QGroupBox("Beep Settings")
        beep_layout = QFormLayout()
        
        self.beep_frequency = QSpinBox()
        self.beep_frequency.setRange(37, 32767)
        self.beep_frequency.setValue(1000)
        self.beep_frequency.setSuffix(" Hz")
        
        self.beep_duration = QSpinBox()
        self.beep_duration.setRange(100, 10000)
        self.beep_duration.setValue(1000)
        self.beep_duration.setSuffix(" ms")
        
        beep_layout.addRow("Frequency:", self.beep_frequency)
        beep_layout.addRow("Duration:", self.beep_duration)
        
        beep_group.setLayout(beep_layout)
        
        # Application settings
        app_group = QGroupBox("Application Settings")
        app_layout = QVBoxLayout()
        
        self.auto_start = QCheckBox("Start monitoring automatically")
        self.minimize_to_tray = QCheckBox("Start minimized to tray")
        self.show_notifications = QCheckBox("Show tray notifications")
        
        self.auto_start.setChecked(True)
        self.minimize_to_tray.setChecked(True)
        self.show_notifications.setChecked(True)
        
        app_layout.addWidget(self.auto_start)
        app_layout.addWidget(self.minimize_to_tray)
        app_layout.addWidget(self.show_notifications)
        
        app_group.setLayout(app_layout)
        
        # Add to main layout
        alert_layout.addWidget(self.enable_alert)
        alert_layout.addLayout(threshold_layout)
        alert_layout.addWidget(alert_method_group)
        alert_layout.addWidget(beep_group)
        
        alert_group.setLayout(alert_layout)
        
        layout.addWidget(alert_group)
        layout.addWidget(app_group)
        
        # Buttons
        buttons_layout = QHBoxLayout()
        
        self.test_button = QPushButton("Test Alert")
        self.save_button = QPushButton("Save")
        self.cancel_button = QPushButton("Cancel")
        
        self.test_button.clicked.connect(self.test_alert)
        self.save_button.clicked.connect(self.save_settings)
        self.cancel_button.clicked.connect(self.reject)
        
        buttons_layout.addWidget(self.test_button)
        buttons_layout.addWidget(self.save_button)
        buttons_layout.addWidget(self.cancel_button)
        
        layout.addLayout(buttons_layout)
        
        self.setLayout(layout)
        
        # Connect signals for enabling/disabling controls
        self.enable_alert.toggled.connect(self.toggle_alert_controls)
        self.beep_alert.toggled.connect(self.toggle_beep_controls)
        self.monitor_type.currentTextChanged.connect(self.update_threshold_range)
    
    def toggle_alert_controls(self, enabled: bool):
        """Enable/disable alert controls"""
        controls = [self.threshold, self.message_restart, self.beep_alert, 
                   self.message_only, self.beep_frequency, self.beep_duration]
        for control in controls:
            control.setEnabled(enabled)
        
        if enabled:
            self.toggle_beep_controls(self.beep_alert.isChecked())
    
    def toggle_beep_controls(self, enabled: bool):
        """Enable/disable beep controls"""
        self.beep_frequency.setEnabled(enabled and self.enable_alert.isChecked())
        self.beep_duration.setEnabled(enabled and self.enable_alert.isChecked())
    
    def update_threshold_range(self, monitor_type: str):
        """Update threshold range based on monitor type"""
        if "Temperature" in monitor_type:
            self.threshold.setRange(30, 100)
            self.threshold.setValue(75)
            self.threshold.setSuffix(" °C")
        elif "Usage" in monitor_type:
            self.threshold.setRange(10, 100)
            self.threshold.setValue(80)
            self.threshold.setSuffix(" %")
        else:  # Disk
            self.threshold.setRange(10, 1000)
            self.threshold.setValue(100)
            self.threshold.setSuffix(" MB/s")
    
    def test_alert(self):
        """Test the selected alert method"""
        if self.beep_alert.isChecked():
            try:
                win32api.Beep(self.beep_frequency.value(), self.beep_duration.value())
            except Exception as e:
                QMessageBox.warning(self, "Test Failed", f"Beep test failed: {e}")
        else:
            QMessageBox.information(
                self, 
                "MMT - Test Alert", 
                f"This is a test alert for {self.monitor_type.currentText()}\n\n"
                f"Threshold: {self.threshold.value()}"
            )
    
    def load_settings(self):
        """Load settings from configuration"""
        config = MonitorConfig.load()
        
        self.monitor_type.setCurrentText(config.get("monitor_type", "CPU Usage (%)"))
        self.interval.setValue(config.get("interval", 1000))
        self.enable_alert.setChecked(config.get("enable_alert", True))
        self.threshold.setValue(config.get("threshold", 80))
        
        alert_method = config.get("alert_method", "message_restart")
        if alert_method == "message_restart":
            self.message_restart.setChecked(True)
        elif alert_method == "beep":
            self.beep_alert.setChecked(True)
        else:
            self.message_only.setChecked(True)
        
        self.beep_frequency.setValue(config.get("beep_frequency", 1000))
        self.beep_duration.setValue(config.get("beep_duration", 1000))
        self.auto_start.setChecked(config.get("auto_start", True))
        self.minimize_to_tray.setChecked(config.get("minimize_to_tray", True))
        self.show_notifications.setChecked(config.get("show_notifications", True))
        
        # Update controls
        self.update_threshold_range(self.monitor_type.currentText())
        self.toggle_alert_controls(self.enable_alert.isChecked())
    
    def save_settings(self):
        """Save settings to configuration"""
        config = {
            "monitor_type": self.monitor_type.currentText(),
            "interval": self.interval.value(),
            "enable_alert": self.enable_alert.isChecked(),
            "threshold": self.threshold.value(),
            "alert_method": ("message_restart" if self.message_restart.isChecked() else 
                           "beep" if self.beep_alert.isChecked() else "message_only"),
            "beep_frequency": self.beep_frequency.value(),
            "beep_duration": self.beep_duration.value(),
            "auto_start": self.auto_start.isChecked(),
            "minimize_to_tray": self.minimize_to_tray.isChecked(),
            "show_notifications": self.show_notifications.isChecked()
        }
        
        if MonitorConfig.save(config):
            QMessageBox.information(self, "Settings Saved", "Settings have been saved successfully!")
            self.accept()
        else:
            QMessageBox.critical(self, "Save Failed", "Failed to save settings!")

class MMTSystemTray:
    """Main system tray application"""
    
    def __init__(self, app: QApplication):
        self.app = app
        self.config = MonitorConfig.load()
        self.monitor = None
        self.status_dialog = None
        
        # Check admin privileges for temperature monitoring
        self.check_admin_privileges()
        
        # Setup system tray
        self.setup_tray()
        
        # Initialize monitor
        self.initialize_monitor()
        
        # Auto-start if enabled
        if self.config.get("auto_start", True):
            self.start_monitoring()
    
    def check_admin_privileges(self):
        """Check admin privileges for temperature monitoring"""
        monitor_type = self.config.get("monitor_type", "")
        if "Temperature" in monitor_type and not self.is_admin():
            reply = QMessageBox.warning(
                None,
                "MMT - Administrator Privileges Required",
                f"Monitoring {monitor_type} requires administrator privileges.\n\n"
                "Without admin rights, temperature monitoring may not work correctly.\n\n"
                "Continue anyway?",
                QMessageBox.Yes | QMessageBox.No,
                QMessageBox.No
            )
            
            if reply == QMessageBox.No:
                sys.exit(1)
    
    def is_admin(self) -> bool:
        """Check if running with admin privileges"""
        try:
            return ctypes.windll.shell32.IsUserAnAdmin()
        except:
            return False
    
    def setup_tray(self):
        """Setup system tray icon and menu"""
        self.tray_icon = QSystemTrayIcon()
        
        # Create a simple icon (you can replace with actual icon file)
        pixmap = QPixmap(16, 16)
        pixmap.fill(QColor("blue"))
        self.tray_icon.setIcon(QIcon(pixmap))
        
        # Create menu
        tray_menu = QMenu()
        
        # Current value display
        self.value_action = QAction("Initializing...", self.tray_icon)
        self.value_action.setEnabled(False)
        tray_menu.addAction(self.value_action)
        
        tray_menu.addSeparator()
        
        # Start/Stop monitoring
        self.start_stop_action = QAction("Start Monitoring", self.tray_icon)
        self.start_stop_action.triggered.connect(self.toggle_monitoring)
        tray_menu.addAction(self.start_stop_action)
        
        # Show status window
        status_action = QAction("Show Status", self.tray_icon)
        status_action.triggered.connect(self.show_status)
        tray_menu.addAction(status_action)
        
        # Settings
        settings_action = QAction("Settings", self.tray_icon)
        settings_action.triggered.connect(self.show_settings)
        tray_menu.addAction(settings_action)
        
        tray_menu.addSeparator()
        
        # About
        about_action = QAction("About", self.tray_icon)
        about_action.triggered.connect(self.show_about)
        tray_menu.addAction(about_action)
        
        # Exit
        exit_action = QAction("Exit", self.tray_icon)
        exit_action.triggered.connect(self.exit_app)
        tray_menu.addAction(exit_action)
        
        self.tray_icon.setContextMenu(tray_menu)
        self.tray_icon.setToolTip("MMT - System Monitor")
        
        # Show tray icon
        if not QSystemTrayIcon.isSystemTrayAvailable():
            QMessageBox.critical(None, "System Tray", "System tray is not available on this system.")
            sys.exit(1)
        
        self.tray_icon.show()
        
        # Show startup message if notifications enabled
        if self.config.get("show_notifications", True):
            self.tray_icon.showMessage(
                "MMT Started",
                f"Monitoring {self.config.get('monitor_type', 'System')}",
                QSystemTrayIcon.Information,
                3000
            )
    
    def initialize_monitor(self):
        """Initialize the system monitor"""
        try:
            monitor_type = self.config.get("monitor_type", "CPU Usage (%)")
            interval = self.config.get("interval", 1000)
            
            self.monitor = SystemMonitor(monitor_type, interval)
            
            # Configure alerts
            self.monitor.configure_alerts(
                enabled=self.config.get("enable_alert", True),
                threshold=self.config.get("threshold", 80),
                method=self.config.get("alert_method", "message_restart"),
                beep_freq=self.config.get("beep_frequency", 1000),
                beep_dur=self.config.get("beep_duration", 1000)
            )
            
            # Connect signals
            self.monitor.value_updated.connect(self.update_tray_display)
            self.monitor.error_occurred.connect(self.handle_monitor_error)
            self.monitor.alert_triggered.connect(self.handle_alert)
            
        except Exception as e:
            error_msg = f"Failed to initialize monitor: {e}"
            QMessageBox.critical(None, "MMT - Initialization Error", error_msg)
            self.value_action.setText("Monitor initialization failed")
    
    def start_monitoring(self):
        """Start monitoring"""
        if self.monitor:
            self.monitor.start_monitoring()
            self.start_stop_action.setText("Stop Monitoring")
            if self.config.get("show_notifications", True):
                self.tray_icon.showMessage(
                    "MMT - Monitoring Started",
                    f"Now monitoring {self.monitor.monitor_type}",
                    QSystemTrayIcon.Information,
                    2000
                )
    
    def stop_monitoring(self):
        """Stop monitoring"""
        if self.monitor:
            self.monitor.stop_monitoring()
            self.start_stop_action.setText("Start Monitoring")
            if self.config.get("show_notifications", True):
                self.tray_icon.showMessage(
                    "MMT - Monitoring Stopped",
                    "System monitoring has been stopped",
                    QSystemTrayIcon.Information,
                    2000
                )
    
    def toggle_monitoring(self):
        """Toggle monitoring on/off"""
        if self.monitor and self.monitor.is_running:
            self.stop_monitoring()
        else:
            self.start_monitoring()
    
    def update_tray_display(self, value: float):
        """Update tray icon tooltip and menu with current value"""
        if self.monitor:
            monitor_type = self.monitor.monitor_type
            unit = self.monitor.unit
            
            # Update menu action
            self.value_action.setText(f"{monitor_type}: {value:.2f} {unit}")
            
            # Update tooltip
            tooltip = f"MMT - {monitor_type}: {value:.2f} {unit}"
            if self.monitor.alert_enabled and self.monitor.threshold:
                if value >= self.monitor.threshold:
                    tooltip += f" ⚠️ ALERT!"
                else:
                    remaining = self.monitor.threshold - value
                    tooltip += f" ({remaining:.1f} below threshold)"
            
            self.tray_icon.setToolTip(tooltip)
            
            # Change icon color based on alert status
            self.update_tray_icon_color(value)
    
    def update_tray_icon_color(self, value: float):
        """Update tray icon color based on current value and threshold"""
        pixmap = QPixmap(16, 16)
        
        if self.monitor.alert_enabled and self.monitor.threshold:
            threshold = self.monitor.threshold
            if value >= threshold:
                pixmap.fill(QColor("red"))  # Alert state
            elif value >= threshold * 0.8:
                pixmap.fill(QColor("orange"))  # Warning state
            else:
                pixmap.fill(QColor("green"))  # Normal state
        else:
            pixmap.fill(QColor("blue"))  # No alert configured
        
        self.tray_icon.setIcon(QIcon(pixmap))
    
    def handle_monitor_error(self, error_msg: str):
        """Handle monitor errors"""
        print(f"Monitor error: {error_msg}")
        
        if self.config.get("show_notifications", True):
            self.tray_icon.showMessage(
                "MMT - Monitor Error",
                error_msg,
                QSystemTrayIcon.Critical,
                5000
            )
        
        # Update display to show error
        self.value_action.setText(f"Error: {error_msg[:30]}...")
    
    def handle_alert(self, current_value: float, threshold: float):
        """Handle alert notifications"""
        if self.config.get("show_notifications", True):
            self.tray_icon.showMessage(
                "MMT - ALERT TRIGGERED!",
                f"{self.monitor.monitor_type}: {current_value:.2f} {self.monitor.unit}\n"
                f"Threshold: {threshold} {self.monitor.unit}",
                QSystemTrayIcon.Critical,
                10000
            )
    
    def show_status(self):
        """Show status dialog"""
        if self.monitor:
            if self.status_dialog is None:
                self.status_dialog = StatusDialog(self.monitor)
            
            self.status_dialog.show()
            self.status_dialog.raise_()
            self.status_dialog.activateWindow()
        else:
            QMessageBox.warning(None, "MMT", "Monitor is not initialized!")
    
    def show_settings(self):
        """Show settings dialog"""
        dialog = SettingsDialog()
        if dialog.exec() == QDialog.Accepted:
            # Reload configuration
            old_config = self.config.copy()
            self.config = MonitorConfig.load()
            
            # Check if monitor type or interval changed
            if (old_config.get("monitor_type") != self.config.get("monitor_type") or
                old_config.get("interval") != self.config.get("interval")):
                
                # Reinitialize monitor
                self.reinitialize_monitor()
            else:
                # Just update alert settings
                if self.monitor:
                    self.monitor.configure_alerts(
                        enabled=self.config.get("enable_alert", True),
                        threshold=self.config.get("threshold", 80),
                        method=self.config.get("alert_method", "message_restart"),
                        beep_freq=self.config.get("beep_frequency", 1000),
                        beep_dur=self.config.get("beep_duration", 1000)
                    )
                    
                    # Update timer interval
                    self.monitor.interval = self.config.get("interval", 1000)
                    if self.monitor.is_running:
                        self.monitor.timer.setInterval(self.monitor.interval)
    
    def reinitialize_monitor(self):
        """Reinitialize monitor with new settings"""
        # Stop current monitor
        if self.monitor:
            self.monitor.stop_monitoring()
        
        # Close status dialog if open
        if self.status_dialog:
            self.status_dialog.close()
            self.status_dialog = None
        
        # Check admin privileges again
        self.check_admin_privileges()
        
        # Initialize new monitor
        self.initialize_monitor()
        
        # Auto-start if enabled
        if self.config.get("auto_start", True):
            self.start_monitoring()
    
    def show_about(self):
        """Show about dialog"""
        about_text = """
<h2>MMT - My Monitor Tray</h2>
<p><b>Version:</b> Alpha</p>
<p><b>Description:</b> Advanced system monitoring tool with real-time alerts</p>

<h3>Features:</h3>
<ul>
<li>Real-time CPU, GPU, RAM, and Disk monitoring</li>
<li>Temperature monitoring (Windows)</li>
<li>Customizable alert thresholds</li>
<li>Multiple alert methods (Restart, Beep, Message)</li>
<li>System tray integration</li>
<li>Cross-platform support</li>
</ul>

<h3>Supported Monitors:</h3>
<ul>
<li>CPU Usage (%)</li>
<li>CPU Temperature (°C) - Windows only</li>
<li>GPU Usage (%)</li>
<li>GPU Temperature (°C) - Windows only</li>
<li>RAM Usage (%)</li>
<li>Disk Usage (MB/s)</li>
</ul>

<p><b>Dependencies:</b> HardView.LiveView, PySide6, psutil, pywin32</p>
<p><b>Author:</b> gafoo</p>
"""
        
        QMessageBox.about(None, "About MMT", about_text)
    
    def exit_app(self):
        """Exit the application"""
        if self.monitor:
            self.monitor.stop_monitoring()
        
        if self.status_dialog:
            self.status_dialog.close()
        
        self.tray_icon.hide()
        self.app.quit()

def create_mutex(name: str) -> bool:
    """Create a named mutex to prevent multiple instances"""
    try:
        mutex = win32event.CreateMutex(None, False, name)
        if win32api.GetLastError() == winerror.ERROR_ALREADY_EXISTS:
            return False
        return True
    except Exception:
        return True  # On non-Windows or error, allow multiple instances

def check_dependencies() -> tuple[bool, list[str]]:
    """Check if all required dependencies are available"""
    missing = []
    
    # Check required modules
    try:
        import PySide6
    except ImportError:
        missing.append("PySide6")
    
    try:
        import psutil
    except ImportError:
        missing.append("psutil")
    
    if sys.platform == "win32":
        try:
            import win32api
            import win32con
            import win32event
            import winerror
        except ImportError:
            missing.append("pywin32")
    
    try:
        from HardView.LiveView import PyLiveCPU
    except ImportError:
        missing.append("HardView.LiveView")
    
    return len(missing) == 0, missing

def main():
    """Main entry point"""
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="MMT - My Monitor Tray: Advanced system monitoring with real-time alerts",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                                    # Start with default settings
  %(prog)s --monitor cpu_temp --threshold 75 # Monitor CPU temperature with 75°C threshold
  %(prog)s --config my_config.json           # Load settings from file
  %(prog)s --monitor ram_usage --threshold 90 --alert beep  # Monitor RAM with beep alert
        """
    )
    
    parser.add_argument("--config", help="Path to configuration file")
    parser.add_argument("--monitor", 
                       choices=["cpu_usage", "cpu_temp", "gpu_usage", "gpu_temp", "ram_usage", "disk_usage"],
                       help="Monitor type to use")
    parser.add_argument("--threshold", type=int, help="Alert threshold value")
    parser.add_argument("--interval", type=int, help="Update interval in milliseconds (100-10000)")
    parser.add_argument("--alert", 
                       choices=["message_restart", "beep", "message_only"],
                       help="Alert method when threshold is exceeded")
    parser.add_argument("--beep-freq", type=int, help="Beep frequency in Hz (37-32767)")
    parser.add_argument("--beep-dur", type=int, help="Beep duration in milliseconds (100-10000)")
    parser.add_argument("--no-auto-start", action="store_true", help="Don't start monitoring automatically")
    parser.add_argument("--no-tray", action="store_true", help="Don't minimize to tray")
    parser.add_argument("--version", action="version", version="MMT Alpha")
    
    args = parser.parse_args()
    
    # Check dependencies
    deps_ok, missing_deps = check_dependencies()
    if not deps_ok:
        print("Error: Missing required dependencies:")
        for dep in missing_deps:
            print(f"  - {dep}")
        print("\nInstall missing dependencies and try again.")
        sys.exit(1)
    
    # Check for single instance (Windows only)
    if sys.platform == "win32":
        mutex_name = "MMT_MyMonitorTray_Mutex_v2"
        if not create_mutex(mutex_name):
            if QApplication.instance() is None:
                app = QApplication(sys.argv)
            QMessageBox.warning(None, "MMT - Already Running", 
                              "MMT is already running in the system tray.\n"
                              "Check the system tray area for the MMT icon.")
            sys.exit(1)
    
    # Initialize Qt application
    app = QApplication(sys.argv)
    app.setQuitOnLastWindowClosed(False)
    app.setApplicationName("MMT")
    app.setApplicationVersion("Alpha")
    app.setOrganizationName("HardView Project")
    
    # Apply command line configuration if provided
    if args.config and os.path.exists(args.config):
        try:
            with open(args.config, "r", encoding="utf-8") as f:
                external_config = json.load(f)
            if MonitorConfig.save(external_config):
                print(f"Configuration loaded from {args.config}")
            else:
                print(f"Failed to apply configuration from {args.config}")
        except Exception as e:
            print(f"Error loading config file {args.config}: {e}")
    
    # Apply command line arguments
    if any(vars(args).values()):
        config = MonitorConfig.load()
        
        # Map command line arguments to config
        if args.monitor:
            monitor_map = {
                "cpu_usage": "CPU Usage (%)",
                "cpu_temp": "CPU Temperature (°C)",
                "gpu_usage": "GPU Usage (%)",
                "gpu_temp": "GPU Temperature (°C)",
                "ram_usage": "RAM Usage (%)",
                "disk_usage": "Disk Usage (MB/s)"
            }
            config["monitor_type"] = monitor_map.get(args.monitor, config["monitor_type"])
        
        if args.threshold is not None:
            if 10 <= args.threshold <= 1000:
                config["threshold"] = args.threshold
            else:
                print("Warning: Threshold must be between 10 and 1000")
        
        if args.interval is not None:
            if 100 <= args.interval <= 10000:
                config["interval"] = args.interval
            else:
                print("Warning: Interval must be between 100 and 10000 ms")
        
        if args.alert:
            config["alert_method"] = args.alert
        
        if args.beep_freq is not None:
            if 37 <= args.beep_freq <= 32767:
                config["beep_frequency"] = args.beep_freq
            else:
                print("Warning: Beep frequency must be between 37 and 32767 Hz")
        
        if args.beep_dur is not None:
            if 100 <= args.beep_dur <= 10000:
                config["beep_duration"] = args.beep_dur
            else:
                print("Warning: Beep duration must be between 100 and 10000 ms")
        
        if args.no_auto_start:
            config["auto_start"] = False
        
        if args.no_tray:
            config["minimize_to_tray"] = False
        
        # Save updated configuration
        if not MonitorConfig.save(config):
            print("Warning: Failed to save command line configuration")
    
    try:
        # Create and run main application
        tray_app = MMTSystemTray(app)
        
        print(f"MMT started successfully")
        print(f"Monitoring: {tray_app.config.get('monitor_type', 'Unknown')}")
        print(f"Check system tray for MMT icon")
        
        # Run application
        exit_code = app.exec()
        
    except Exception as e:
        error_msg = f"Application error: {e}"
        print(error_msg)
        
        if QApplication.instance():
            QMessageBox.critical(None, "MMT - Fatal Error", error_msg)
        
        exit_code = 1
    
    sys.exit(exit_code)

if __name__ == "__main__":
    main()
