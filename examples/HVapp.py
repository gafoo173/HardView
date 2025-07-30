"""
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License.
 See the LICENSE file in the project root for more details.
================================================================================
"""
import sys
import json
import time
import pprint
import subprocess
import multiprocessing # Import multiprocessing for running blocking operations in a separate process
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QTextEdit, QLabel, QTabWidget, QLineEdit, QProgressBar,
    QMessageBox, QScrollArea, QTreeWidget, QTreeWidgetItem, QSizePolicy,
    QHeaderView
)
from PySide6.QtCore import Qt, QThread, Signal, QTimer
from PySide6.QtGui import QFont, QIntValidator, QIcon, QPixmap, QPainter, QColor, QClipboard, QAction # Moved QAction here

# --- Function to install missing packages ---
def install_package(package_name):
    """
    Attempts to install a Python package using pip.
    Returns True if successful, False otherwise.
    """
    try:
        # Use sys.executable to ensure pip corresponds to the current Python interpreter
        # This is crucial for environments like PyInstaller or virtual environments
        subprocess.check_call([sys.executable, "-m", "pip", "install", package_name])
        return True
    except subprocess.CalledProcessError as e:
        QMessageBox.critical(None, "Installation Failed",
                             f"Failed to install '{package_name}'. Please ensure you have an active internet connection "
                             f"and sufficient permissions, then try running the application again.\n\nError: {e}")
        return False
    except Exception as e:
        QMessageBox.critical(None, "Installation Error",
                             f"An unexpected error occurred while trying to install '{package_name}':\n{e}")
        return False

# --- Attempt to import HardView, install if not found ---
# This block runs in the main process
try:
    from HardView import (
        get_bios_info_objects,
        get_system_info_objects,
        get_baseboard_info_objects,
        get_chassis_info_objects,
        get_cpu_info_objects,
        get_ram_info_objects,
        get_disk_info_objects,
        get_network_info_objects,
        get_cpu_usage_objects,
        get_ram_usage_objects,
        get_system_performance_objects,
        monitor_cpu_usage_duration_objects,
        monitor_ram_usage_duration_objects,
        monitor_system_performance_duration_objects,
        get_partitions_info_objects,
        get_smart_info_objects,
        get_gpu_info_objects
    )
    HARDVIEW_AVAILABLE = True
except ImportError:
    HARDVIEW_AVAILABLE = False
    msg_box = QMessageBox()
    msg_box.setIcon(QMessageBox.Warning)
    msg_box.setText("HardView library not found.")
    msg_box.setInformativeText("This application requires the 'HardView' library. Do you want to install it now?")
    msg_box.setStandardButtons(QMessageBox.Yes | QMessageBox.No)
    msg_box.setDefaultButton(QMessageBox.Yes)
    reply = msg_box.exec()

    if reply == QMessageBox.Yes:
        if install_package("HardView"):
            QMessageBox.information(None, "Installation Complete",
                                    "HardView installed successfully. Please restart the application to use it.")
            sys.exit(0)
        else:
            QMessageBox.critical(None, "Installation Failed",
                                 "HardView could not be installed. The application will now exit.")
            sys.exit(1)
    else:
        QMessageBox.critical(None, "HardView Required",
                             "HardView library is required to run this application. Exiting.")
        sys.exit(1)

# Wrapper functions for each HardView call (now directly returning Python objects)
# These wrappers are used by both the main process (for instant calls) and the child process (for monitoring)
def get_bios(): return get_bios_info_objects(False)
def get_system(): return get_system_info_objects(False)
def get_baseboard(): return get_baseboard_info_objects(False)
def get_chassis(): return get_chassis_info_objects(False)
def get_cpu(): return get_cpu_info_objects(False)
def get_ram(): return get_ram_info_objects(False)
def get_disk_drives(): return get_disk_info_objects(False)
def get_network_adapters(): return get_network_info_objects(False)
def get_gpu(): return get_gpu_info_objects(False)
def get_cpu_usage_current(): return get_cpu_usage_objects(False)
def get_ram_usage_current(): return get_ram_usage_objects(False)
def get_system_performance_current(): return get_system_performance_objects(False)
def monitor_cpu_usage_duration(duration_seconds, interval_ms):
    return monitor_cpu_usage_duration_objects(duration_seconds, interval_ms)
def monitor_ram_usage_duration(duration_seconds, interval_ms):
    return monitor_ram_usage_duration_objects(duration_seconds, interval_ms)
def monitor_system_performance_duration(duration_seconds, interval_ms):
    return monitor_system_performance_duration_objects(duration_seconds, interval_ms)
def get_partitions(): return get_partitions_info_objects(False)
def get_smart_info_objects_wrapper(): return get_smart_info_objects(False)


# --- Target function for the multiprocessing.Process ---
def _monitor_process_target(func_name, duration, interval, result_queue):
    """
    This function runs in a separate process. It calls the actual HardView
    monitoring function and puts the result (or an error) into the queue.
    """
    try:
        # Re-import HardView functions within the new process context
        # This is important for some OS (e.g., Windows) where processes don't inherit imports
        from HardView import (
            monitor_cpu_usage_duration_objects,
            monitor_ram_usage_duration_objects,
            monitor_system_performance_duration_objects,
        )
        # Map function names to actual functions in this process's scope
        func_map = {
            "monitor_cpu_usage_duration": monitor_cpu_usage_duration_objects,
            "monitor_ram_usage_duration": monitor_ram_usage_duration_objects,
            "monitor_system_performance_duration": monitor_system_performance_duration_objects,
        }
        func = func_map.get(func_name)
        if func is None:
            raise ValueError(f"Unknown monitoring function: {func_name}")

        result = func(duration, interval)
        result_queue.put({"status": "success", "data": result})
    except Exception as e:
        result_queue.put({"status": "error", "message": str(e)})


# --- Worker Thread for HardView Operations (now manages multiprocessing) ---
class HardViewWorker(QThread):
    """
    A QThread subclass to manage HardView operations.
    For monitoring functions, it spawns a multiprocessing.Process to prevent UI freezing.
    """
    result_signal = Signal(object, str) # (data, function_name)
    error_signal = Signal(str, str)     # (error_message, function_name)
    progress_signal = Signal(int, str)  # (percentage, message)

    def __init__(self, func, *args, **kwargs):
        super().__init__()
        self.func = func
        self.args = args
        self.kwargs = kwargs
        self.function_name = func.__name__
        self.process = None # To hold the multiprocessing.Process instance
        self.result_queue = None # To hold the multiprocessing.Queue

    def run(self):
        try:
            if self.function_name.startswith("monitor_"):
                duration = self.args[0]
                interval = self.args[1]

                self.progress_signal.emit(0, f"Collecting {self.function_name.replace('monitor_', '').replace('_duration', '')} data for {duration} seconds...")

                self.result_queue = multiprocessing.Queue()
                self.process = multiprocessing.Process(
                    target=_monitor_process_target,
                    args=(self.function_name, duration, interval, self.result_queue)
                )
                self.process.start()

                # Simulate progress while the child process is running
                start_time = time.time()
                # Loop to update progress bar until the process is no longer alive
                while self.process.is_alive():
                    elapsed = time.time() - start_time
                    progress = int((elapsed / duration) * 100)
                    if progress > 100:
                        progress = 100
                    self.progress_signal.emit(min(progress, 99), f"Collecting data ({int(elapsed)}/{duration}s)...")
                    time.sleep(0.1) # Update every 100ms

                # Ensure process has fully terminated and resources are cleaned up
                self.process.join()

                # Now, try to get the result from the queue with a short timeout
                # This handles cases where the process might have just finished but the item isn't immediately available
                try:
                    # Attempt to get the result from the queue with a timeout
                    queue_result = self.result_queue.get(timeout=5) # Wait up to 5 seconds for result
                    if queue_result["status"] == "success":
                        self.progress_signal.emit(100, "Processing results...")
                        self.result_signal.emit(queue_result["data"], self.function_name)
                    else:
                        self.error_signal.emit(queue_result["message"], self.function_name)
                except multiprocessing.queues.Empty:
                    # If the queue is empty after the process finishes, it means no data was put
                    self.error_signal.emit("Monitoring process finished but no data was received from the queue within the timeout. This might indicate an issue with HardView or data collection.", self.function_name)
                except Exception as e:
                    # Catch any other exceptions during queue retrieval
                    self.error_signal.emit(f"Error retrieving data from queue: {e}", self.function_name)

            else:
                # For non-monitoring functions, execute directly in this QThread
                result = self.func(*self.args, **self.kwargs)
                self.result_signal.emit(result, self.function_name)
        except Exception as e:
            self.error_signal.emit(str(e), self.function_name)
        finally:
            # Ensure final "Done" state
            self.progress_signal.emit(100, "Done.")


# --- Worker Thread for Live Performance Tab ---
class LivePerformanceWorker(QThread):
    """
    A dedicated QThread subclass for fetching all live performance metrics.
    Emits a single signal with all results and errors when finished.
    """
    finished_with_data = Signal(dict, dict) # (results_dict, errors_dict)

    def __init__(self, performance_functions):
        super().__init__()
        self.performance_functions = performance_functions

    def run(self):
        """
        Fetches all current performance metrics and emits them.
        """
        results = {}
        errors = {}
        for name, func in self.performance_functions.items():
            try:
                data = func()
                results[name] = data
            except Exception as e:
                errors[name] = str(e)
        self.finished_with_data.emit(results, errors)


# --- UI Components: Hardware Information Tab ---
class HardwareInfoTab(QWidget):
    """
    Tab for displaying static hardware information using a QTreeWidget.
    """
    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        # Tree widget to display structured hardware information
        self.tree_widget = QTreeWidget()
        self.tree_widget.setHeaderLabels(["Property", "Value"])
        self.tree_widget.setColumnCount(2)
        self.tree_widget.setAlternatingRowColors(True) # Alternate row colors for readability
        self.tree_widget.setFont(QFont("Segoe UI", 10))
        self.tree_widget.setStyleSheet("""
            QTreeWidget {
                background-color: #f0f0f0;
                border: 1px solid #ccc;
                border-radius: 5px;
                padding: 5px;
            }
            QTreeWidget::item {
                padding: 3px;
            }
            QTreeWidget::item:selected {
                background-color: #a0c0ff; /* Highlight selected item */
            }
            QHeaderView::section {
                background-color: #e0e0e0;
                padding: 5px;
                border: 1px solid #c0c0c0;
                font-weight: bold;
            }
        """)
        self.tree_widget.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.tree_widget.header().setStretchLastSection(False)
        self.tree_widget.header().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.tree_widget.header().setSectionResizeMode(1, QHeaderView.Stretch)
        self.tree_widget.setSelectionMode(QTreeWidget.ExtendedSelection) # Enable multiple selections

        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setWidget(self.tree_widget)
        self.scroll_area.setStyleSheet("QScrollArea { border: none; }") # Remove border from scroll area

        self.layout.addWidget(self.scroll_area)

        # Layout for buttons to fetch different info types and copy button
        self.buttons_layout = QHBoxLayout()
        self.layout.addLayout(self.buttons_layout)

        # Dictionary mapping button names to HardView functions
        self.info_functions = {
            "BIOS": get_bios,
            "System": get_system,
            "Baseboard": get_baseboard,
            "Chassis": get_chassis,
            "CPU": get_cpu,
            "RAM": get_ram,
            "Disk Drives": get_disk_drives,
            "Network Adapters": get_network_adapters,
            "GPU": get_gpu,
            "Partitions": get_partitions,
            "SMART Info": get_smart_info_objects_wrapper,
        }

        # Create buttons dynamically
        for name, func in self.info_functions.items():
            button = QPushButton(f"Get {name}")
            button.clicked.connect(lambda _, f=func, n=name: self.fetch_info(f, n))
            button.setStyleSheet("""
                QPushButton {
                    background-color: #4CAF50; /* Green */
                    color: white;
                    border-radius: 8px;
                    padding: 10px 15px;
                    font-weight: bold;
                    border: none;
                }
                QPushButton:hover {
                    background-color: #45a049;
                }
                QPushButton:pressed {
                    background-color: #3e8e41;
                }
                QPushButton:disabled {
                    background-color: #cccccc;
                    color: #666666;
                }
            """)
            self.buttons_layout.addWidget(button)

        # Add Copy Selected button
        self.copy_button = QPushButton("Copy Selected")
        self.copy_button.clicked.connect(self.copy_selected_info)
        self.copy_button.setStyleSheet("""
            QPushButton {
                background-color: #007BFF; /* Blue */
                color: white;
                border-radius: 8px;
                padding: 10px 15px;
                font-weight: bold;
                border: none;
            }
            QPushButton:hover {
                background-color: #0056b3;
            }
            QPushButton:pressed {
                background-color: #004085;
            }
            QPushButton:disabled {
                background-color: #cccccc;
                color: #666666;
            }
        """)
        self.buttons_layout.addWidget(self.copy_button)

        self.buttons_layout.addStretch(1) # Push buttons to the left

        self.current_worker = None # To keep track of the running worker thread

    def fetch_info(self, func, name):
        """
        Starts a worker thread to fetch hardware information.
        """
        if self.current_worker and self.current_worker.isRunning():
            QMessageBox.warning(self, "Busy", "Another operation is already running. Please wait.")
            return

        self.tree_widget.clear() # Clear previous data
        # Show a "Fetching..." message
        root_item = QTreeWidgetItem(self.tree_widget, [f"Fetching {name}...", ""])
        self.tree_widget.addTopLevelItem(root_item)
        self.tree_widget.expandAll() # Expand to show message

        # Create and start the worker thread
        self.current_worker = HardViewWorker(func)
        self.current_worker.result_signal.connect(self.display_info)
        self.current_worker.error_signal.connect(self.display_error)
        self.current_worker.start()

    def display_info(self, data, func_name):
        """
        Displays the fetched hardware information in the QTreeWidget.
        """
        self.tree_widget.clear() # Clear "Fetching..." message
        self._populate_tree(self.tree_widget.invisibleRootItem(), data)
        self.tree_widget.expandAll() # Expand all nodes for full view
        self.current_worker = None # Worker finished

    def display_error(self, error_message, func_name):
        """
        Displays an error message in the QTreeWidget and a QMessageBox.
        """
        self.tree_widget.clear()
        error_item = QTreeWidgetItem(self.tree_widget, ["Error", f"Failed to get {func_name.replace('get_', '').replace('_objects_wrapper', '').replace('_current', '')}: {error_message}"])
        self.tree_widget.addTopLevelItem(error_item)
        self.tree_widget.expandAll()
        self.current_worker = None # Worker finished
        QMessageBox.critical(self, "Error", f"Failed to retrieve data for {func_name.replace('get_', '').replace('_objects_wrapper', '').replace('_current', '')}:\n{error_message}")

    def _populate_tree(self, parent_item, data, key_prefix=""):
        """
        Recursively populates the QTreeWidget with data from a dictionary or list.
        Handles empty dictionaries/lists and None/empty string values.
        """
        if isinstance(data, dict):
            if not data: # If dictionary is empty
                QTreeWidgetItem(parent_item, ["(Empty Dictionary)", ""])
                return
            for key, value in data.items():
                if isinstance(value, dict):
                    if not value: # Empty nested dictionary
                        item = QTreeWidgetItem(parent_item, [key, "(Empty Dictionary)"])
                    else:
                        item = QTreeWidgetItem(parent_item, [key, ""])
                        self._populate_tree(item, value)
                elif isinstance(value, list):
                    if not value: # Empty nested list
                        item = QTreeWidgetItem(parent_item, [key, "(Empty List)"])
                    else:
                        item = QTreeWidgetItem(parent_item, [key, ""])
                        self._populate_tree(item, value)
                else:
                    display_value = str(value) if value is not None and str(value).strip() != "" else "N/A"
                    item = QTreeWidgetItem(parent_item, [key, display_value])
        elif isinstance(data, list):
            if not data: # If list is empty
                QTreeWidgetItem(parent_item, ["(Empty List)", ""])
                return
            for i, item_data in enumerate(data):
                if isinstance(item_data, (dict, list)):
                    item_name = f"Entry {i+1}"
                    if isinstance(item_data, dict):
                        if 'name' in item_data and item_data['name']:
                            item_name = str(item_data['name'])
                        elif 'Model' in item_data and item_data['Model']:
                            item_name = str(item_data['Model'])
                        elif 'Description' in item_data and item_data['Description']:
                            item_name = str(item_data['Description'])

                    item = QTreeWidgetItem(parent_item, [item_name, ""])
                    self._populate_tree(item, item_data)
                else:
                    display_value = str(item_data) if item_data is not None and str(item_data).strip() != "" else "N/A"
                    item = QTreeWidgetItem(parent_item, [f"Item {i+1}", display_value])

    def copy_selected_info(self):
        """
        Copies the text of selected items in the QTreeWidget to the clipboard.
        Formats the output as "Property: Value" or just "Value" for child items.
        """
        selected_items = self.tree_widget.selectedItems()
        if not selected_items:
            QMessageBox.information(self, "No Selection", "Please select one or more items to copy.")
            return

        clipboard_text = []
        for item in selected_items:
            # Check if it's a root item or a direct child of a root (e.g., "Property", "Value")
            # Or if it's a child of a non-list/non-dict parent (a leaf node)
            if item.parent() is None or \
               (item.parent() is not None and not (isinstance(item.parent().text(0), str) and ("Entry" in item.parent().text(0) or "Item" in item.parent().text(0)) and not item.parent().text(1))):
                # This handles top-level items and direct children of object items.
                # If it has a property and a value, copy both.
                if item.text(0) and item.text(1):
                    clipboard_text.append(f"{item.text(0)}: {item.text(1)}")
                elif item.text(0): # Just a property, likely a parent node for a dict/list
                    clipboard_text.append(f"{item.text(0)}")
            else:
                # For nested items (especially in lists or deeply nested dictionaries), just copy the value
                # This avoids redundant "Item X: Property: Value"
                if item.text(1): # If it has a value, copy it
                    clipboard_text.append(item.text(1))
                elif item.text(0): # If it has a property but no value, it might be a parent node
                     clipboard_text.append(item.text(0))


        if clipboard_text:
            text_to_copy = "\n".join(clipboard_text)
            clipboard = QApplication.clipboard()
            clipboard.setText(text_to_copy)
            QMessageBox.information(self, "Copied", f"{len(selected_items)} item(s) copied to clipboard.")
        else:
            QMessageBox.warning(self, "Nothing to Copy", "Selected items do not contain copyable text.")


# --- UI Components: Live Performance Tab ---
class PerformanceTab(QWidget):
    """
    Tab for displaying current CPU, RAM, and System performance.
    Includes auto-refresh functionality.
    """
    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.labels = {} # Dictionary to hold QLabel widgets for performance metrics
        self.performance_functions = {
            "CPU Usage": get_cpu_usage_current,
            "RAM Usage": get_ram_usage_current,
            "System Performance": get_system_performance_current,
        }

        # Create labels for each performance metric
        for name in self.performance_functions:
            label = QLabel(f"{name}: N/A")
            label.setFont(QFont("Segoe UI", 12, QFont.Bold))
            label.setStyleSheet("color: #333; margin-bottom: 10px; padding: 5px; border-bottom: 1px solid #eee;")
            self.labels[name] = label
            self.layout.addWidget(label)

        # Refresh button
        self.refresh_button = QPushButton("Refresh All")
        self.refresh_button.clicked.connect(self.refresh_all_performance)
        self.refresh_button.setStyleSheet("""
            QPushButton {
                background-color: #007BFF; /* Blue */
                color: white;
                border-radius: 8px;
                padding: 10px 15px;
                font-weight: bold;
                border: none;
                margin-top: 15px;
            }
            QPushButton:hover {
                background-color: #0056b3;
            }
            QPushButton:pressed {
                background-color: #004085;
            }
            QPushButton:disabled {
                    background-color: #cccccc;
                    color: #666666;
            }
        """)
        self.layout.addWidget(self.refresh_button)

        # Auto-refresh toggle button
        self.auto_refresh_checkbox = QPushButton("Auto Refresh (Off)")
        self.auto_refresh_checkbox.setCheckable(True) # Make it a toggle button
        self.auto_refresh_checkbox.clicked.connect(self.toggle_auto_refresh)
        self.auto_refresh_checkbox.setStyleSheet("""
            QPushButton {
                background-color: #6c757d; /* Gray */
                color: white;
                border-radius: 8px;
                padding: 10px 15px;
                font-weight: bold;
                border: none;
                margin-top: 5px;
            }
            QPushButton:hover {
                background-color: #5a6268;
            }
            QPushButton:checked {
                background-color: #28a745; /* Green when checked (Auto Refresh On) */
            }
            QPushButton:checked:hover {
                background-color: #218838;
            }
            QPushButton:pressed {
            }
        """)
        self.layout.addWidget(self.auto_refresh_checkbox)

        self.layout.addStretch(1) # Push content to the top

        self.timer = QTimer(self) # Timer for auto-refresh
        self.timer.timeout.connect(self.refresh_all_performance)
        self.current_worker = None # To manage the worker thread

        self.refresh_all_performance() # Initial refresh when tab is loaded

    def toggle_auto_refresh(self):
        """
        Toggles the auto-refresh functionality.
        """
        if self.auto_refresh_checkbox.isChecked():
            self.timer.start(3000) # Refresh every 3 seconds (3000 ms)
            self.auto_refresh_checkbox.setText("Auto Refresh (On)")
        else:
            self.timer.stop()
            self.auto_refresh_checkbox.setText("Auto Refresh (Off)")

    def refresh_all_performance(self):
        """
        Initiates fetching all current performance metrics in a background thread.
        """
        if self.current_worker and self.current_worker.isRunning():
            return # Don't start a new refresh if one is already running

        # Update labels to indicate fetching
        for name in self.performance_functions:
            self.labels[name].setText(f"{name}: Fetching...")

        # Create and start the dedicated worker thread for live performance
        self.current_worker = LivePerformanceWorker(self.performance_functions)
        self.current_worker.finished_with_data.connect(self._performance_fetch_finished)
        self.current_worker.start()

    def _performance_fetch_finished(self, results, errors):
        """
        Slot connected to the worker thread's finished_with_data signal.
        Updates the UI with the fetched performance data.
        """
        for name in self.performance_functions:
            if name in results:
                raw_data = results[name] # This is the object returned by the HardView function

                # Ensure raw_data is a dictionary before proceeding
                if not isinstance(raw_data, dict):
                    self.labels[name].setText(f"{name}: Error - Unexpected data format (expected dict, got {type(raw_data).__name__})")
                    continue # Skip to next item

                # --- CPU Usage ---
                if name == "CPU Usage":
                    cpu_data = raw_data.get('cpu_usage')
                    if isinstance(cpu_data, list) and len(cpu_data) > 0 and isinstance(cpu_data[0], dict):
                        total_usage = cpu_data[0].get('load_percentage') # Get raw value, None if not found
                        try:
                            if total_usage is not None:
                                total_usage_formatted = f"{float(total_usage):.1f}%"
                                self.labels[name].setText(f"CPU Usage: {total_usage_formatted} (Total)")
                            else:
                                self.labels[name].setText(f"CPU Usage: Not available.")
                        except (ValueError, TypeError):
                            self.labels[name].setText(f"CPU Usage: Data format error.")
                    else:
                        self.labels[name].setText(f"CPU Usage: Not available.")

                # --- RAM Usage ---
                elif name == "RAM Usage":
                    ram_data = raw_data.get('ram_usage')
                    if isinstance(ram_data, dict):
                        total_memory_kb = ram_data.get('total_memory_kb')
                        used_memory_kb = ram_data.get('used_memory_kb')
                        usage_percent = ram_data.get('usage_percent')

                        # Check if all critical RAM values are available and not None
                        if all(v is not None for v in [total_memory_kb, used_memory_kb, usage_percent]):
                            try:
                                total_gb = float(total_memory_kb) / (1024**2)
                                used_gb = float(used_memory_kb) / (1024**2)
                                available_gb = (float(total_memory_kb) - float(used_memory_kb)) / (1024**2)
                                used_percent = float(usage_percent)

                                self.labels[name].setText(
                                    f"RAM Usage: {used_gb:.2f}GB / {total_gb:.2f}GB ({used_percent:.1f}%) Available: {available_gb:.2f}GB"
                                )
                            except (ValueError, TypeError):
                                self.labels[name].setText(f"RAM Usage: Data format error.")
                        else:
                            self.labels[name].setText(f"RAM Usage: Not available.")
                    else:
                        self.labels[name].setText(f"RAM Usage: Not available.")

                # --- System Performance ---
                elif name == "System Performance":
                    system_perf_data = raw_data.get('system_performance')
                    if isinstance(system_perf_data, dict):
                        cpu_load_percent = "N/A"
                        ram_usage_percent = "N/A"
                        uptime_str = "N/A"
                        boot_time_str = "N/A"
                        users_str = "N/A"

                        # CPU within System Performance
                        cpu_data_container = system_perf_data.get('cpu')
                        if isinstance(cpu_data_container, dict):
                            cpu_sub_data = cpu_data_container.get('cpu_usage')
                            if isinstance(cpu_sub_data, list) and len(cpu_sub_data) > 0 and isinstance(cpu_sub_data[0], dict):
                                cpu_load = cpu_sub_data[0].get('load_percentage')
                                try:
                                    if cpu_load is not None:
                                        cpu_load_percent = f"{float(cpu_load):.1f}%"
                                except (ValueError, TypeError): pass

                        # RAM within System Performance
                        ram_data_container = system_perf_data.get('ram')
                        if isinstance(ram_data_container, dict):
                            ram_sub_data = ram_data_container.get('ram_usage')
                            if isinstance(ram_sub_data, dict):
                                ram_usage = ram_sub_data.get('usage_percent')
                                try:
                                    if ram_usage is not None:
                                        ram_usage_percent = f"{float(ram_usage):.1f}%"
                                except (ValueError, TypeError): pass

                        # Other system performance metrics
                        uptime_seconds = system_perf_data.get('uptime_seconds')
                        boot_time = system_perf_data.get('boot_time')
                        users = system_perf_data.get('logged_in_users')

                        if uptime_seconds is not None:
                            uptime_str = self._format_uptime(uptime_seconds)
                        if boot_time is not None:
                            boot_time_str = str(boot_time)
                        if users is not None:
                            users_str = str(users)

                        self.labels[name].setText(
                            f"System Performance: CPU: {cpu_load_percent} | RAM: {ram_usage_percent} | "
                            f"Uptime: {uptime_str} | Boot: {boot_time_str} | Users: {users_str}"
                        )
                    else:
                        self.labels[name].setText(f"System Performance: Not available.")

            elif name in errors:
                self.labels[name].setText(f"{name}: Error - {errors[name]}")
        self.current_worker = None # Worker finished

    def _format_uptime(self, seconds):
        """
        Helper function to format uptime from seconds into a human-readable string.
        """
        if seconds is None or seconds == 'N/A':
            return "N/A"
        try:
            seconds = int(seconds)
            days = seconds // (24 * 3600)
            seconds %= (24 * 3600)
            hours = seconds // 3600
            seconds %= 3600
            minutes = seconds // 60
            seconds %= 60
            return f"{days}d {hours}h {minutes}m {seconds}s"
        except (ValueError, TypeError):
            return "N/A"


# --- UI Components: Performance Monitoring Tab ---
class MonitoringTab(QWidget):
    """
    Tab for monitoring CPU, RAM, and System performance over a specified duration.
    Includes input fields for duration/interval, a progress bar, and an output area.
    """
    def __init__(self, parent=None):
        super().__init__(parent)
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        # Input fields for duration and interval
        input_layout = QHBoxLayout()
        self.duration_label = QLabel("Duration (seconds):")
        self.duration_input = QLineEdit("5")
        self.duration_input.setValidator(QIntValidator(1, 3600)) # 1 second to 1 hour
        self.interval_label = QLabel("Interval (ms):")
        self.interval_input = QLineEdit("1000")
        self.interval_input.setValidator(QIntValidator(100, 60000)) # 100ms to 1 minute

        input_layout.addWidget(self.duration_label)
        input_layout.addWidget(self.duration_input)
        input_layout.addWidget(self.interval_label)
        input_layout.addWidget(self.interval_input)
        self.layout.addLayout(input_layout)

        # Buttons for starting different monitoring types
        buttons_layout = QHBoxLayout()
        self.monitor_cpu_btn = QPushButton("Monitor CPU")
        self.monitor_ram_btn = QPushButton("Monitor RAM")
        self.monitor_system_btn = QPushButton("Monitor System")

        # Connect buttons to the start_monitoring method
        self.monitor_cpu_btn.clicked.connect(lambda: self.start_monitoring(monitor_cpu_usage_duration, "CPU"))
        self.monitor_ram_btn.clicked.connect(lambda: self.start_monitoring(monitor_ram_usage_duration, "RAM"))
        self.monitor_system_btn.clicked.connect(lambda: self.start_monitoring(monitor_system_performance_duration, "System"))

        # Apply consistent styling to monitoring buttons
        for btn in [self.monitor_cpu_btn, self.monitor_ram_btn, self.monitor_system_btn]:
            btn.setStyleSheet("""
                QPushButton {
                    background-color: #FFC107; /* Amber */
                    color: #333;
                    border-radius: 8px;
                    padding: 10px 15px;
                    font-weight: bold;
                    border: none;
                }
                QPushButton:hover {
                    background-color: #e0a800;
                }
                QPushButton:pressed {
                    background-color: #c69500;
                }
                QPushButton:disabled {
                    background-color: #cccccc;
                    color: #666666;
                }
            """)
            buttons_layout.addWidget(btn)
        self.layout.addLayout(buttons_layout)

        # Progress bar to show monitoring progress
        self.progress_bar = QProgressBar()
        self.progress_bar.setAlignment(Qt.AlignCenter) # Center the text
        self.progress_bar.setTextVisible(True) # Show percentage text
        self.progress_bar.setFormat("%p% - %v") # Format: %p = percentage, %v = current value (message)
        self.progress_bar.setValue(0)
        self.progress_bar.hide() # Hide initially until monitoring starts
        self.layout.addWidget(self.progress_bar)

        # Output area to display monitoring results (raw JSON)
        self.output_text = QTextEdit()
        self.output_text.setReadOnly(True) # Make it read-only
        self.output_text.setFont(QFont("Consolas", 9)) # Monospace font for JSON
        self.output_text.setStyleSheet("""
            QTextEdit {
                background-color: #2b2b2b; /* Dark background */
                color: #f8f8f2; /* Light text */
                border: 1px solid #444;
                border-radius: 5px;
                padding: 10px;
            }
        """)
        self.layout.addWidget(self.output_text)

        self.current_worker = None # To manage the worker thread

    def start_monitoring(self, func, name):
        """
        Starts a worker thread for performance monitoring.
        """
        if self.current_worker and self.current_worker.isRunning():
            QMessageBox.warning(self, "Busy", "Another monitoring operation is already running. Please wait.")
            return

        try:
            duration = int(self.duration_input.text())
            interval = int(self.interval_input.text())
            if duration <= 0 or interval <= 0:
                raise ValueError("Duration and interval must be positive integers.")
        except ValueError as e:
            QMessageBox.critical(self, "Invalid Input", f"Please enter valid integer values for duration and interval: {e}")
            return

        self.output_text.clear() # Clear previous results
        self.output_text.append(f"Starting {name} monitoring for {duration} seconds with {interval}ms interval...")
        self.progress_bar.setValue(0)
        self.progress_bar.setFormat(f"0% - Starting {name} monitoring...")
        self.progress_bar.show() # Show progress bar
        self._set_buttons_enabled(False) # Disable buttons during monitoring

        # Create and start the worker thread for monitoring
        self.current_worker = HardViewWorker(func, duration, interval)
        self.current_worker.result_signal.connect(self.display_monitoring_results)
        self.current_worker.error_signal.connect(self.display_monitoring_error)
        self.current_worker.progress_signal.connect(self.update_progress)
        self.current_worker.finished.connect(self._monitoring_finished) # Connect to finished signal
        self.current_worker.start()

    def update_progress(self, percentage, message):
        """
        Updates the progress bar with current percentage and message.
        """
        self.progress_bar.setValue(percentage)
        self.progress_bar.setFormat(f"{percentage}% - {message}")

    def display_monitoring_results(self, data, func_name):
        """
        Displays the collected monitoring results in the output text area.
        Now uses pprint for Python object readability, with an added check for empty data.
        """
        self.output_text.append("\n--- Monitoring Complete ---")
        if data: # Check if data is not empty (e.g., not None, not empty list, not empty dict)
            self.output_text.append(pprint.pformat(data, indent=2, width=80)) # Use pprint for Python objects
        else:
            self.output_text.append("No data was collected during monitoring, or the returned data was empty.")
        self.progress_bar.setFormat("Done.")

    def display_monitoring_error(self, error_message, func_name):
        """
        Displays an error message if monitoring fails.
        """
        self.output_text.append(f"\n--- Monitoring Error ---")
        self.output_text.append(f"Failed to monitor {func_name.replace('monitor_', '').replace('_duration', '')}: {error_message}")
        QMessageBox.critical(self, "Error", f"Monitoring failed for {func_name.replace('monitor_', '').replace('_duration', '')}:\n{error_message}")
        self.progress_bar.setFormat("Error.")

    def _monitoring_finished(self):
        """
        Slot connected when the monitoring worker thread finishes.
        Re-enables buttons and clears worker reference.
        """
        self._set_buttons_enabled(True)
        self.current_worker = None # Clear worker reference


    def _set_buttons_enabled(self, enabled):
        """
        Enables or disables monitoring-related buttons and input fields.
        """
        self.monitor_cpu_btn.setEnabled(enabled)
        self.monitor_ram_btn.setEnabled(enabled)
        self.monitor_system_btn.setEnabled(enabled)
        self.duration_input.setEnabled(enabled)
        self.interval_input.setEnabled(enabled)


# --- Main Application Window ---
class HardViewApp(QMainWindow):
    """
    The main application window for the HardView System Monitor.
    Manages the tab widget and overall application styling.
    """
    def __init__(self):
        super().__init__()
        self.setWindowTitle("HardView System Monitor")
        self.setGeometry(100, 100, 1000, 700) # x, y, width, height for the window

        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        self.main_layout = QVBoxLayout(self.central_widget)

        # Tab widget to organize different sections of the application
        self.tab_widget = QTabWidget()
        self.tab_widget.setFont(QFont("Segoe UI", 11, QFont.Bold))
        self.tab_widget.setStyleSheet("""
            QTabWidget::pane { /* The tab widget frame */
                border-top: 1px solid #c2c7cb;
                background: #f8f8f8;
                border-radius: 5px;
            }
            QTabBar::tab {
                background: #e0e0e0;
                border: 1px solid #c2c7cb;
                border-bottom-color: #c2c7cb; /* same as pane color */
                border-top-left-radius: 4px;
                border-top-right-radius: 4px;
                min-width: 8ex;
                padding: 8px 15px;
                margin-right: 2px; /* Small gap between tabs */
            }
            QTabBar::tab:selected {
                background: #f8f8f8;
                border-color: #c2c7cb;
                border-bottom-color: #f8f8f8; /* same as pane color to make it look connected */
            }
            QTabBar::tab:hover {
                background: #d0d0d0;
            }
        """)

        # Create instances of each tab
        self.hardware_tab = HardwareInfoTab()
        self.performance_tab = PerformanceTab()
        self.monitoring_tab = MonitoringTab()

        # Add tabs to the tab widget
        self.tab_widget.addTab(self.hardware_tab, "Hardware Info")
        self.tab_widget.addTab(self.performance_tab, "Live Performance")
        self.tab_widget.addTab(self.monitoring_tab, "Performance Monitoring")

        self.main_layout.addWidget(self.tab_widget)

        # Status bar at the bottom of the main window
        self.status_bar = self.statusBar()
        self.status_bar.showMessage("Ready")

        # Set overall application style
        self.setStyleSheet("""
            QMainWindow {
                background-color: #f5f5f5; /* Light gray background */
            }
            QLabel {
                color: #333; /* Dark gray text for labels */
            }
            QLineEdit {
                border: 1px solid #ccc;
                border-radius: 5px;
                padding: 5px;
                background-color: white;
            }
            QProgressBar {
                border: 1px solid #ccc;
                border-radius: 5px;
                text-align: center;
                background-color: #e0e0e0;
                color: #333;
            }
            QProgressBar::chunk {
                background-color: #28a745; /* Green for progress */
                border-radius: 5px;
            }
        """)
        self.setWindowIcon(self.create_icon()) # Set a custom window icon

    def create_icon(self):
        """
        Creates a simple custom icon for the application window.
        """
        pixmap = QPixmap(64, 64)
        pixmap.fill(Qt.transparent) # Transparent background
        painter = QPainter(pixmap)
        painter.setRenderHint(QPainter.Antialiasing) # For smooth edges
        painter.setBrush(QColor("#007BFF")) # Blue circle
        painter.drawEllipse(5, 5, 54, 54) # Draw a circle
        painter.setPen(QColor("white")) # White text
        painter.setFont(QFont("Arial", 30, QFont.Bold)) # Font for text
        painter.drawText(pixmap.rect(), Qt.AlignCenter, "HV") # Draw "HV" in the center
        painter.end()
        return QIcon(pixmap)


# --- Main execution block ---
if __name__ == "__main__":
    # This is important for multiprocessing on Windows to avoid issues
    multiprocessing.freeze_support()

    # Create the QApplication instance
    app = QApplication(sys.argv)
    # Set a consistent default font for the entire application
    app.setFont(QFont("Segoe UI", 10))
    # Create and show the main application window
    window = HardViewApp()
    window.show()
    # Start the Qt event loop
    sys.exit(app.exec())
