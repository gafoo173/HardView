import sys
import time

from rich.console import Console
from rich.table import Table
from rich.panel import Panel
from rich.live import Live
from rich.prompt import IntPrompt
from rich import box

console = Console()


def select_from_list(options, title="Select an option"):
    import msvcrt

    index = 0
    scroll_offset = 0

    def render():
        nonlocal scroll_offset

        max_window = max(5, console.size.height - 8)
        window_size = min(len(options), max_window)

        if index < scroll_offset:
            scroll_offset = index
        elif index > scroll_offset + window_size - 1:
            scroll_offset = index - window_size + 1

        visible = options[scroll_offset:scroll_offset + window_size]

        table = Table(box=box.SIMPLE, show_header=False)
        table.add_column("")

        if scroll_offset > 0:
            table.add_row("[dim]  ... more above ...[/dim]")

        for offset, opt in enumerate(visible):
            i = scroll_offset + offset
            if i == index:
                table.add_row(f"[bold cyan]> {opt}[/bold cyan]")
            else:
                table.add_row(f"  {opt}")

        if scroll_offset + window_size < len(options):
            table.add_row("[dim]  ... more below ...[/dim]")

        subtitle = f"Use Up/Down arrows and Enter to select ({index + 1}/{len(options)})"
        return Panel(table, title=title, subtitle=subtitle)

    with Live(render(), console=console, auto_refresh=False, transient=True) as live:
        while True:
            key = msvcrt.getch()
            if key in (b"\xe0", b"\x00"):
                key2 = msvcrt.getch()
                if key2 == b"H":
                    index = (index - 1) % len(options)
                elif key2 == b"P":
                    index = (index + 1) % len(options)
                live.update(render(), refresh=True)
            elif key in (b"\r", b"\n"):
                break
            elif key == b"\x03":
                raise KeyboardInterrupt

    return index


def print_static_info():
    from HardView import smbios

    try:
        parser = smbios.SMBIOSParser()
        parser.load_smbios_data()
        parser.parse_smbios_data()
        info = parser.get_parsed_info()
    except Exception as e:
        console.print(f"[bold red]Failed to read SMBIOS data: {e}[/bold red]")
        return

    console.rule("[bold cyan]Static System Information (SMBIOS)")

    system_table = Table(title="System", box=box.ROUNDED, show_header=False)
    system_table.add_column("Field", style="bold")
    system_table.add_column("Value")
    system_table.add_row("Manufacturer", info.system.manufacturer)
    system_table.add_row("Product Name", info.system.product_name)
    system_table.add_row("Version", info.system.version)
    system_table.add_row("Serial Number", info.system.serial_number)
    system_table.add_row("UUID", info.system.uuid)
    system_table.add_row("SKU", info.system.sku_number)
    system_table.add_row("Family", info.system.family)
    console.print(system_table)

    bios_table = Table(title="BIOS", box=box.ROUNDED, show_header=False)
    bios_table.add_column("Field", style="bold")
    bios_table.add_column("Value")
    bios_table.add_row("Vendor", info.bios.vendor)
    bios_table.add_row("Version", info.bios.version)
    bios_table.add_row("Release Date", info.bios.release_date)
    bios_table.add_row("Release", f"{info.bios.major_release}.{info.bios.minor_release}")
    console.print(bios_table)

    board_table = Table(title="Baseboard", box=box.ROUNDED, show_header=False)
    board_table.add_column("Field", style="bold")
    board_table.add_column("Value")
    board_table.add_row("Manufacturer", info.baseboard.manufacturer)
    board_table.add_row("Product", info.baseboard.product)
    board_table.add_row("Version", info.baseboard.version)
    board_table.add_row("Serial Number", info.baseboard.serial_number)
    console.print(board_table)

    enclosure = info.system_enclosure
    chassis_type = parser.get_chassis_type_string(enclosure.chassis_type)
    enclosure_table = Table(title="Chassis / Enclosure", box=box.ROUNDED, show_header=False)
    enclosure_table.add_column("Field", style="bold")
    enclosure_table.add_column("Value")
    enclosure_table.add_row("Manufacturer", enclosure.manufacturer)
    enclosure_table.add_row("Type", chassis_type)
    enclosure_table.add_row("Serial Number", enclosure.serial_number)
    console.print(enclosure_table)

    cpu_table = Table(title="Processors", box=box.ROUNDED)
    cpu_table.add_column("Socket")
    cpu_table.add_column("Manufacturer")
    cpu_table.add_column("Model")
    cpu_table.add_column("Cores")
    cpu_table.add_column("Threads")
    cpu_table.add_column("Max Speed (MHz)")
    cpu_table.add_column("Current Speed (MHz)")
    for cpu in info.processors:
        cpu_table.add_row(
            cpu.socket_designation,
            cpu.manufacturer,
            cpu.version,
            str(cpu.core_count),
            str(cpu.thread_count),
            str(cpu.max_speed),
            str(cpu.current_speed),
        )
    console.print(cpu_table)

    mem_table = Table(title="Memory Devices", box=box.ROUNDED)
    mem_table.add_column("Slot")
    mem_table.add_column("Size (MB)")
    mem_table.add_column("Type")
    mem_table.add_column("Form Factor")
    mem_table.add_column("Speed (MT/s)")
    mem_table.add_column("Manufacturer")
    mem_table.add_column("Part Number")
    for mem in info.memory_devices:
        mem_type = parser.get_memory_type_string(mem.memory_type)
        form_factor = parser.get_form_factor_string(mem.form_factor)
        mem_table.add_row(
            mem.device_locator,
            str(mem.size_mb),
            mem_type,
            form_factor,
            str(mem.speed),
            mem.manufacturer,
            mem.part_number,
        )
    console.print(mem_table)

    cache_table = Table(title="Cache", box=box.ROUNDED)
    cache_table.add_column("Designation")
    cache_table.add_column("Type")
    cache_table.add_column("Installed Size (KB)")
    cache_table.add_column("Max Size (KB)")
    for cache in info.caches:
        cache_type = parser.get_cache_type_string(cache.system_cache_type)
        cache_table.add_row(
            cache.socket_designation,
            cache_type,
            str(cache.installed_size),
            str(cache.maximum_cache_size),
        )
    console.print(cache_table)


def build_device_sensors_table(device_name, sensor_names, sensor):
    table = Table(title=f"Live Sensors - {device_name}", box=box.ROUNDED)
    table.add_column("Sensor")
    table.add_column("Value", justify="right")
    for name in sensor_names:
        label = name.split(" - ", 1)[1] if " - " in name else name
        try:
            value = sensor.get_value_by_name(name)
            table.add_row(label, f"{value:.2f}")
        except Exception:
            table.add_row(label, "N/A")
    return table


def monitor_sensors():
    from HardView.LiveView import PySensor, PyManageTemp

    console.rule("[bold cyan]Live Sensors Monitoring")

    manager = PyManageTemp()
    sensor = PySensor(True) #Auto initialize

    try:
        #manager.init() no need to initialize because PySensor does it
        manager.update()
        sensor.reget()
    except Exception as e:
        console.print(f"[bold red]Failed to initialize sensors: {e}[/bold red]")
        return

    all_names = sensor.get_all_sensors()
    hardware_map = {}
    for name in all_names:
        hw_name = name.split(" - ")[0]
        if hw_name in hardware_map:
            continue
        try:
            hw_id = manager.get_hardware_id_by_name(hw_name)
        except Exception:
            continue
        if hw_id is not None and hw_id >= 0:
            hardware_map[hw_name] = hw_id

    if not hardware_map:
        console.print("[yellow]No hardware devices with sensors were found.[/yellow]")
        try:
            manager.close()
        except Exception:
            pass
        return

    device_names = sorted(hardware_map.keys())
    console.print("\n[bold]Select the hardware device you want to monitor:[/bold]")
    selected_index = select_from_list(device_names, title="Hardware Devices")
    selected_name = device_names[selected_index]
    selected_id = hardware_map[selected_name]

    sensor_names = [name for name in all_names if name.split(" - ")[0] == selected_name]

    duration_seconds = IntPrompt.ask(
        f"\nEnter number of seconds to monitor '{selected_name}'", default=10
    )

    try:
        with Live(
            build_device_sensors_table(selected_name, sensor_names, sensor),
            console=console,
            refresh_per_second=4,
        ) as live:
            elapsed = 0
            while elapsed < duration_seconds:
                time.sleep(1)
                elapsed += 1
                manager.specific_update(selected_id)
                sensor.reget()
                live.update(build_device_sensors_table(selected_name, sensor_names, sensor))
    except Exception as e:
        console.print(f"[bold red]Error while monitoring sensors: {e}[/bold red]")
    finally:
        try:
            manager.close()
        except Exception:
            pass


def print_smart_attributes():
    from HardView import SMART

    console.rule("[bold cyan]SMART Attributes")

    for drive_number in range(8):
        try:
            info = SMART.get_disk_info_s(drive_number)
            if info is None:
                continue

            controller_type = SMART.detect_ssd_type(info)
            controller_name = SMART.ssd_type_to_string(controller_type)

            header = Table(title=f"Drive {drive_number}", box=box.ROUNDED, show_header=False)
            header.add_column("Field", style="bold")
            header.add_column("Value")
            header.add_row("Path", f"\\\\.\\PhysicalDrive{drive_number}")
            header.add_row("Model", info.model_upper)
            header.add_row("Firmware", info.firmware_rev)
            header.add_row("Media", "SSD" if info.is_ssd else "HDD")
            header.add_row("Controller", controller_name)
            console.print(header)

            attr_table = Table(title=f"SMART Attributes - Drive {drive_number}", box=box.ROUNDED)
            attr_table.add_column("ID")
            attr_table.add_column("Attribute Name")
            attr_table.add_column("Current")
            attr_table.add_column("Worst")
            attr_table.add_column("Raw Value")
            for attr in info.attributes:
                name = SMART.get_attribute_name_by_id_and_type(controller_type, attr.id)
                attr_table.add_row(
                    f"{attr.id:02X}",
                    name,
                    str(attr.current),
                    str(attr.worst),
                    str(attr.raw_value),
                )
            console.print(attr_table)

        except Exception:
            continue


def main():
    if sys.platform != "win32":
        console.print("[bold red]HardView Quick Start is only available on Windows.[/bold red]")
        return

    console.print(Panel("HardView Quick Start", style="bold cyan", expand=False))

    print_static_info()

    monitor_sensors()

    print_smart_attributes()

    console.rule("[bold green]Done")


if __name__ == "__main__":
    main()