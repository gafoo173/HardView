#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
HardView Enhanced Test Script
Enhanced version with beautiful formatting, colors, and animations
"""

import json
import platform
import inspect
import time
from datetime import datetime
from rich.console import Console
from rich.align import Align

console = Console()

# Rich library imports for beautiful formatting
try:
    from rich.console import Console
    from rich.table import Table
    from rich.panel import Panel
    from rich.text import Text
    from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TimeRemainingColumn
    from rich.layout import Layout
    from rich.align import Align
    from rich.columns import Columns
    from rich import box
    from rich.markdown import Markdown
    from rich.tree import Tree
    from rich.status import Status
    import rich.traceback
    rich.traceback.install()
except ImportError:
    print("Rich library not found. Installing...")
    import subprocess
    import sys
    subprocess.check_call([sys.executable, "-m", "pip", "install", "rich"])
    from rich.console import Console
    from rich.table import Table
    from rich.panel import Panel
    from rich.text import Text
    from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TimeRemainingColumn
    from rich.layout import Layout
    from rich.align import Align
    from rich.columns import Columns
    from rich import box
    from rich.markdown import Markdown
    from rich.tree import Tree
    from rich.status import Status
    import rich.traceback
    rich.traceback.install()

# Initialize Rich console
console = Console()

# HardView logo (without markup tags)
HARDVIEW_LOGO_RAW = """
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒▓████▓▓▒░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒██▓▓▓▓▓▓▓▓███▒░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░▒███████▒░░░░░░░██▓▓▓▓▓▓▓▓▓▓▓▓▓██░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░▒██▓▓▓▓▓▓▓▓██▒░░░░▓█▓▓▓███▓▓▓▓▓▓▓▓▓▓█▒░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░▓█▓▓▓▓▓▓▓▓▓▓▓▓█▓░▒██▓▓▓▓▓▓▓▓▓▓█▓▓▓▓▓▓██░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░██▓▓▓▓██▓▒▒▓███▓█▒▒███▓▓▓███▓████▓▓▓▓▓▓█▒░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░███████████████████████████████████████▓▓▓▓██░░░░░░░░░░░░░░░░
░░░░░░░░░░░░▒█▒░▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░▒██▒░▒▒▒▒▒▒▒▒▒▒▒▓█▓▓▓▓██░░░░░░░░░░░░░░░░
░░░░░░░░░░░░▒█▒▒█▒░░░██░░░█▒█▒█░░░░▒▒░░░░░░░░░█▓█▒██▓▓▓▓██░░░░░░░░░░░░░░░░
░░░░░░░░░░░░▒█▒▒█████████▒█▒█▒█░█████████████████▒█▓▓▓▓▓█▒░░░░░░░░░░░░░░░░
░░░░░░░░░░░░▒█▒▒█████████▒░░░░░░███▓▒░░░░░░░▒█████▓▓▓▓▓██░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░▒█▒▒█▓▓▓▓██▒░░▒▒▒▒▒░██▒░░░░░░░░░░░▓██▓▓▓▓▓██░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░▒█▒▒█░░░░██▒█░▓███▒░█▓░░░▒████▓░░▓█▓▓▓▓▓▓██▒░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░██░▒█░░░░██░░▒████▓░█▒░░░▒█▒▒████▓▓▓▓▓▓▓█▓█▒░░░░░░░░░░░░░░░░░░
░░░░░░░░░░▒███░▒█▓▓▓▓██▓▒▒░▒▒░░░█▓░░░░▓████▓▓▓▓▓▓▓██▒░█▒░░░░░░░░░░░░░░░░░░
░░░░░░░░░▒█▓██░▒███████▒█▓▒█▓█░░██▓░░▒██▓▓▓▓▓▓▓▓████▒░█▒░░░░░░░░░░░░░░░░░░
░░░░░░░░░██▓▓█▒▒███████▒▒░░░▒░░░██████▓▓▓▓▓▓▓▓████▒░░░█▒░░░░░░░░░░░░░░░░░░
░░░░░░░░▒█▓▓▓██░░▓█▓▓░░░░▒█▒█▓▓░████▓▓▓▓▓▓▓███████▒▒░░████░░░░░░░░░░░░░░░░
░░░░░░░░▒█▓▓▓▓▓██▒░░▒▒▒▓▓▒█▒█▓▓░▓█▓▓▓▓▓▓▓██████████▓░░██▓▓█▒░░░░░░░░░░░░░░
░░░░░░░░░▓█▓▓▓▓▓▓▓████████████████████████▒▒░░░░░░░░░░██▓▓▓█▒░░░░░░░░░░░░░
░░░░░░░░░░██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█████████████▓▓▓██░░░░░░░░░░░░░
░░░░░░░░░░░▒██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████████▓▓▓▓██░░░░░░░░░░░░░
░░░░░░░░░░░░░▒▓███▓▓▓▓▓▓▓▓▓▓▓▓▓███████▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▓▓▓▓▓█▓░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░▒▒▒▒▒▒░░▓█████████▓████▓▓▓▓▓▓▓▓▓▓▓▓▓▓██░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░▒▒▒▒▒██████████▓▒▒▒▒███▓▓▓▓▓▓▓▓▓██▒░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░█████████████████████░░▒▓███████▓▒░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▒▒░░░░░░░░░░░▒▒░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░██▒░░░██░░░░░░░░░░░░░░░░░░░██▒██▒░░░██▓░█▓░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░██▒▒▒▒██░░█████▒▒████▓▒██████▒▒██▒░▒██░░██░▒█████░██▒░██▓░▓█▓░░░░░░
░░░░░░░████████░░▒▓▓▓██▒██▒░░██░░▒██▒░▓█▓▒██▒░░██▒██▓▓▓██░██▒███░██░░░░░░░
░░░░░░░██▒░░░██░██▒▒▒██▒██░░░██░░▒██▒░░████▓░░░██▒██▒▒▒▒▒░████▒███▓░░░░░░░
░░░░░░░██▒░░░██░▓██████▒██░░░▒██████▒░░▒███░░░░██░▒█████░░▒██▒░▓██░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
"""

# Try importing the HardView module
try:
    import HardView
    HARDVIEW_AVAILABLE = True
except ImportError as e:
    HARDVIEW_AVAILABLE = False
    console.print(f"[bold red]Error importing HardView module:[/bold red] {e}")
    console.print("[yellow]Please make sure the HardView.pyd (or HardView.so) file is present in the same directory or in the PYTHONPATH.[/yellow]")

def show_logo():
    """Display the HardView logo with animation"""
    logo_lines = HARDVIEW_LOGO_RAW.strip("\n").splitlines()

    for line in logo_lines:
        # Use Text object to apply styling properly
        styled_line = Text(line, style="red")
        console.print(Align.center(styled_line))
        time.sleep(0.02)

    console.print()
    console.print(Align.center("[bold green]HardView System Information Tool[/bold green]"))
    console.print(Align.center(f"[dim]Test started at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}[/dim]"))
    console.print()

def format_bytes(bytes_value):
    """Format bytes into human readable format"""
    if not isinstance(bytes_value, (int, float)):
        return str(bytes_value)
    
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if bytes_value < 1024.0:
            return f"{bytes_value:.2f} {unit}"
        bytes_value /= 1024.0
    return f"{bytes_value:.2f} PB"

def format_value(value, indent=0):
    """Format value with proper indentation and structure"""
    indent_str = "  " * indent
    
    if isinstance(value, dict):
        if not value:
            return "N/A"
        
        formatted_items = []
        for k, v in value.items():
            # Special formatting for common fields
            if 'bytes' in k.lower() or 'size' in k.lower() or 'memory' in k.lower():
                if isinstance(v, (int, float)) and v > 1024:
                    formatted_v = f"{v} ({format_bytes(v)})"
                else:
                    formatted_v = str(v)
            elif 'percent' in k.lower() or 'usage' in k.lower():
                formatted_v = f"{v}%" if isinstance(v, (int, float)) else str(v)
            else:
                formatted_v = str(v)
            
            if isinstance(v, (dict, list)):
                formatted_items.append(f"{indent_str}[bold cyan]{k}[/bold cyan]:")
                formatted_items.append(format_value(v, indent + 1))
            else:
                formatted_items.append(f"{indent_str}[bold cyan]{k}[/bold cyan]: [green]{formatted_v}[/green]")
        return "\n".join(formatted_items)
    
    elif isinstance(value, list):
        if not value:
            return "N/A"
        
        formatted_items = []
        for i, item in enumerate(value):
            if isinstance(item, dict):
                formatted_items.append(f"{indent_str}[bold yellow]━━━ Item {i+1} ━━━[/bold yellow]")
                formatted_items.append(format_value(item, indent + 1))
                if i < len(value) - 1:
                    formatted_items.append("")
            else:
                formatted_items.append(f"{indent_str}[bold yellow]•[/bold yellow] [green]{item}[/green]")
        return "\n".join(formatted_items)
    
    else:
        return f"[green]{value}[/green]"

def create_info_table(title, data, color_scheme="blue"):
    """Create a beautiful table for displaying information"""
    table = Table(
        title=f"[bold {color_scheme}]{title}[/bold {color_scheme}]",
        box=box.ROUNDED,
        border_style=color_scheme,
        title_style=f"bold {color_scheme}",
        show_header=True,
        header_style=f"bold {color_scheme}",
        show_lines=True,
        expand=True
    )
    
    table.add_column("Property", style="cyan", width=30, no_wrap=True)
    table.add_column("Value", style="white", ratio=2)
    
    if isinstance(data, dict):
        for key, value in data.items():
            formatted_value = format_value(value)
            table.add_row(
                f"[bold cyan]{key}[/bold cyan]",
                formatted_value
            )
    elif isinstance(data, list):
        for i, item in enumerate(data):
            formatted_value = format_value(item)
            table.add_row(
                f"[bold cyan]Item {i+1}[/bold cyan]",
                formatted_value
            )
    else:
        table.add_row("Value", format_value(data))
    
    return table

def create_specialized_table(title, data, table_type="default"):
    """Create specialized tables for different data types"""
    
    if table_type == "memory_modules" and isinstance(data, list):
        table = Table(
            title=f"[bold blue]💾 Memory Modules[/bold blue]",
            box=box.ROUNDED,
            border_style="blue",
            show_header=True,
            header_style="bold blue"
        )
        
        table.add_column("Module", style="cyan", width=10)
        table.add_column("Capacity", style="green", width=15)
        table.add_column("Speed", style="yellow", width=12)
        table.add_column("Manufacturer", style="magenta", width=15)
        table.add_column("Part Number", style="white", width=20)
        table.add_column("Serial", style="cyan", width=15)
        
        for i, module in enumerate(data):
            table.add_row(
                f"Module {i+1}",
                format_bytes(module.get('capacity_bytes', 0)),
                f"{module.get('speed_mhz', 0)} MHz",
                module.get('manufacturer', 'N/A'),
                module.get('part_number', 'N/A').strip(),
                module.get('serial_number', 'N/A')
            )
        
        return table
    
    elif table_type == "network_adapters" and isinstance(data, list):
        table = Table(
            title=f"[bold blue]🌐 Network Adapters[/bold blue]",
            box=box.ROUNDED,
            border_style="blue",
            show_header=True,
            header_style="bold blue"
        )
        
        table.add_column("Adapter", style="cyan", width=8)
        table.add_column("Description", style="green", width=35)
        table.add_column("MAC Address", style="yellow", width=18)
        table.add_column("IP Addresses", style="magenta", width=30)
        
        for i, adapter in enumerate(data):
            ip_list = adapter.get('ip_addresses', [])
            ip_str = "\n".join(ip_list[:3]) if ip_list else "N/A"
            if len(ip_list) > 3:
                ip_str += f"\n... and {len(ip_list) - 3} more"
            
            table.add_row(
                f"#{i+1}",
                adapter.get('description', 'N/A'),
                adapter.get('mac_address', 'N/A'),
                ip_str
            )
        
        return table
    
    elif table_type == "partitions" and isinstance(data, list):
        table = Table(
            title=f"[bold blue]💿 Disk Partitions[/bold blue]",
            box=box.ROUNDED,
            border_style="blue",
            show_header=True,
            header_style="bold blue"
        )
        
        table.add_column("Partition", style="cyan", width=15)
        table.add_column("Type", style="green", width=25)
        table.add_column("Size", style="yellow", width=15)
        table.add_column("Disk Model", style="magenta", width=20)
        table.add_column("Interface", style="white", width=10)
        
        for partition in data:
            table.add_row(
                partition.get('partition_device_id', 'N/A'),
                partition.get('partition_type', 'N/A'),
                format_bytes(partition.get('partition_size', 0)),
                partition.get('disk_model', 'N/A')[:20] + "..." if len(partition.get('disk_model', '')) > 20 else partition.get('disk_model', 'N/A'),
                partition.get('disk_interface', 'N/A')
            )
        
        return table
    
    # Default table for other cases
    return create_info_table(title, data)

def create_monitoring_chart(title, readings, data_type="cpu"):
    """Create a simple ASCII chart for monitoring data"""
    if not readings:
        return Panel("No data available", title=title, border_style="red")
    
    # Extract values based on data type
    values = []
    timestamps = []
    
    for i, reading in enumerate(readings):
        if data_type == "cpu":
            if 'cpu_usage' in reading and reading['cpu_usage']:
                values.append(reading['cpu_usage'][0].get('load_percentage', 0))
                timestamps.append(f"T{i+1}")
        elif data_type == "ram":
            if 'ram_usage' in reading:
                values.append(reading['ram_usage'].get('usage_percent', 0))
                timestamps.append(f"T{i+1}")
        elif data_type == "system":
            if 'system_performance' in reading:
                cpu_val = 0
                ram_val = 0
                perf = reading['system_performance']
                if 'cpu' in perf and perf['cpu']['cpu_usage']:
                    cpu_val = perf['cpu']['cpu_usage'][0].get('load_percentage', 0)
                if 'ram' in perf:
                    ram_val = perf['ram']['ram_usage'].get('usage_percent', 0)
                values.append((cpu_val, ram_val))
                timestamps.append(f"T{i+1}")
    
    if not values:
        return Panel("No valid data found", title=title, border_style="red")
    
    # Create chart content
    chart_lines = []
    chart_lines.append(f"[bold cyan]{title}[/bold cyan]")
    chart_lines.append("")
    
    if data_type in ["cpu", "ram"]:
        # Single value chart
        max_val = max(values) if values else 100
        chart_lines.append("Value │ Chart")
        chart_lines.append("──────┼" + "─" * 40)
        
        for i, (val, time) in enumerate(zip(values, timestamps)):
            bar_length = int((val / max_val) * 30) if max_val > 0 else 0
            bar = "█" * bar_length + "░" * (30 - bar_length)
            chart_lines.append(f"{val:5.1f}% │ {bar} {time}")
    
    else:
        # System performance (dual values)
        chart_lines.append("Time │ CPU%  │ RAM%  │ CPU Chart          │ RAM Chart")
        chart_lines.append("─────┼───────┼───────┼────────────────────┼─────────────────────")
        
        for i, (vals, time) in enumerate(zip(values, timestamps)):
            cpu_val, ram_val = vals
            cpu_bar = "█" * int(cpu_val / 5) + "░" * (20 - int(cpu_val / 5))
            ram_bar = "█" * int(ram_val / 5) + "░" * (20 - int(ram_val / 5))
            chart_lines.append(f"{time:4} │ {cpu_val:5.1f} │ {ram_val:5.1f} │ {cpu_bar} │ {ram_bar}")
    
    chart_content = "\n".join(chart_lines)
    return Panel(chart_content, title="📊 Performance Chart", border_style="green", padding=(1, 2))

def print_json_result(func_name, result_str, color_scheme="blue"):
    """Print the JSON result with beautiful formatting"""
    if result_str:
        try:
            json_obj = json.loads(result_str)
            
            # Special handling for monitoring functions
            if "monitor" in func_name:
                console.print(f"[bold {color_scheme}]📊 {func_name.replace('_', ' ').title()} Results[/bold {color_scheme}]")
                console.print()
                
                # Show summary info
                duration = json_obj.get(list(json_obj.keys())[0], {}).get('duration_seconds', 0)
                interval = json_obj.get(list(json_obj.keys())[0], {}).get('interval_ms', 0)
                readings = json_obj.get(list(json_obj.keys())[0], {}).get('readings', [])
                
                summary_table = Table(
                    title="[bold green]Monitoring Summary[/bold green]",
                    box=box.SIMPLE,
                    border_style="green"
                )
                summary_table.add_column("Parameter", style="cyan")
                summary_table.add_column("Value", style="white")
                summary_table.add_row("Duration", f"{duration} seconds")
                summary_table.add_row("Interval", f"{interval} ms")
                summary_table.add_row("Readings Count", str(len(readings)))
                
                console.print(summary_table)
                console.print()
                
                # Show chart
                if "cpu" in func_name:
                    chart = create_monitoring_chart("CPU Usage Over Time", readings, "cpu")
                elif "ram" in func_name:
                    chart = create_monitoring_chart("RAM Usage Over Time", readings, "ram")
                elif "system_performance" in func_name:
                    chart = create_monitoring_chart("System Performance Over Time", readings, "system")
                else:
                    chart = Panel("Chart not available", title="Chart", border_style="red")
                
                console.print(chart)
                
            # Use specialized tables for certain data types
            elif func_name == "get_ram_info" and "memory_modules" in json_obj:
                # Show total memory first
                if "total_physical_memory_bytes" in json_obj:
                    total_memory = json_obj["total_physical_memory_bytes"]
                    console.print(f"[bold green]Total Physical Memory:[/bold green] {format_bytes(total_memory)}")
                    console.print()
                
                # Show memory modules in specialized table
                modules_table = create_specialized_table("Memory Modules", json_obj["memory_modules"], "memory_modules")
                console.print(modules_table)
                
            elif func_name == "get_network_info" and "network_adapters" in json_obj:
                adapters_table = create_specialized_table("Network Adapters", json_obj["network_adapters"], "network_adapters")
                console.print(adapters_table)
                
            elif func_name == "get_partitions_info" and "partitions" in json_obj:
                partitions_table = create_specialized_table("Disk Partitions", json_obj["partitions"], "partitions")
                console.print(partitions_table)
                
            else:
                # Use default table
                table = create_info_table(f"{func_name.replace('_', ' ').title()} Information", json_obj, color_scheme)
                console.print(table)
            
            console.print()
            
        except json.JSONDecodeError:
            console.print(f"[bold red]JSON decoding error for {func_name}[/bold red]")
            console.print(Panel(result_str, title="Raw Output", border_style="red"))
    else:
        console.print(f"[bold yellow]No data returned for {func_name}[/bold yellow]")

def print_python_object_result(func_name, result_obj, color_scheme="green"):
    """Print the result as a Python object with beautiful formatting"""
    if result_obj is not None:
        if isinstance(result_obj, (dict, list)):
            table = create_info_table(f"{func_name} Information", result_obj, color_scheme)
            console.print(table)
        else:
            console.print(Panel(str(result_obj), title=f"{func_name} Information", border_style=color_scheme))
        console.print()
    else:
        console.print(f"[bold yellow]No data returned for {func_name}[/bold yellow]")

def run_test(func_name, is_monitoring_func=False, *extra_args):
    """Run a function and print results with beautiful formatting"""
    color_schemes = {
        "get_bios_info": "cyan",
        "get_system_info": "green",
        "get_baseboard_info": "blue",
        "get_chassis_info": "magenta",
        "get_cpu_info": "red",
        "get_ram_info": "yellow",
        "get_disk_info": "cyan",
        "get_network_info": "blue",
        "get_cpu_usage": "red",
        "get_ram_usage": "yellow",
        "get_system_performance": "green",
        "get_partitions_info": "magenta",
        "get_smart_info": "cyan",
        "get_gpu_info": "blue",
    }
    
    color_scheme = color_schemes.get(func_name, "white")
    
    # Show spinner while processing
    with Status(f"[bold {color_scheme}]Getting {func_name.replace('_', ' ').title()}...[/bold {color_scheme}]", 
                spinner="dots") as status:
        time.sleep(0.5)  # Small delay for effect
        
        # Try to get the function
        json_func = getattr(HardView, func_name, None)
        obj_func = getattr(HardView, f"{func_name}_objects", None)

        # Prefer JSON version first
        if json_func:
            json_result_str = None
            try:
                if is_monitoring_func:
                    json_result_str = json_func(*extra_args)
                else:
                    try:
                        json_result_str = json_func(True, *extra_args)
                    except TypeError as e:
                        if "takes no arguments" in str(e) or "takes 0 positional arguments" in str(e):
                            json_result_str = json_func(*extra_args)
                        else:
                            raise
            except Exception as e:
                console.print(f"[bold red]Error calling {func_name} (JSON):[/bold red] {e}")
                return
            
            if json_result_str is not None:
                status.stop()
                print_json_result(func_name, json_result_str, color_scheme)
                return
        
        # Fallback to Python object version
        if obj_func:
            obj_result = None
            try:
                if is_monitoring_func:
                    obj_result = obj_func(*extra_args)
                else:
                    try:
                        obj_result = obj_func(False, *extra_args)
                    except TypeError as e:
                        if "takes no arguments" in str(e) or "takes 0 positional arguments" in str(e):
                            obj_result = obj_func(*extra_args)
                        else:
                            raise
            except Exception as e:
                console.print(f"[bold red]Error calling {obj_func.__name__} (Python Object):[/bold red] {e}")
                return

            if obj_result is not None:
                status.stop()
                print_python_object_result(func_name, obj_result, color_scheme)
                return
        
        status.stop()
        console.print(f"[bold yellow]Function {func_name} not found or returned no data[/bold yellow]")

def main():
    """Main function to run all tests"""
    if not HARDVIEW_AVAILABLE:
        return
    
    # Show logo
    show_logo()
    
    # List of functions to test
    functions_to_test = [
        "get_bios_info",
        "get_system_info",
        "get_baseboard_info",
        "get_chassis_info",
        "get_cpu_info",
        "get_ram_info",
        "get_disk_info",
        "get_network_info",
        "get_cpu_usage",
        "get_ram_usage",
        "get_system_performance",
        "get_partitions_info",
        "get_smart_info",
        "get_gpu_info",
    ]

    # Monitoring functions (require extra arguments)
    monitoring_functions_to_test = [
        ("monitor_cpu_usage_duration", 5, 1000),  # 5 seconds, 1000 ms interval
        ("monitor_ram_usage_duration", 5, 1000),
        ("monitor_system_performance_duration", 5, 1000),
    ]

    # Create progress bar for standard tests
    with Progress(
        SpinnerColumn(),
        TextColumn("[progress.description]{task.description}"),
        BarColumn(),
        TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
        TimeRemainingColumn(),
        console=console
    ) as progress:
        
        # Standard tests
        task = progress.add_task("Running standard tests...", total=len(functions_to_test))
        
        for func_name in functions_to_test:
            progress.update(task, description=f"Testing {func_name}...")
            run_test(func_name)
            progress.advance(task)
        
        # Monitoring tests
        if monitoring_functions_to_test:
            console.print("\n" + "="*60)
            console.print(Align.center("[bold magenta]Monitoring Functions Tests[/bold magenta]"))
            console.print("="*60 + "\n")
            
            task2 = progress.add_task("Running monitoring tests...", total=len(monitoring_functions_to_test))
            
            for func_name, duration, interval in monitoring_functions_to_test:
                progress.update(task2, description=f"Testing {func_name} ({duration}s)...")
                console.print(f"[bold cyan]Running {func_name} for {duration} seconds...[/bold cyan]")
                run_test(func_name, True, duration, interval)
                progress.advance(task2)

    # Final message
    console.print("\n" + "="*60)
    console.print(Align.center("[bold green]✅ Library test completed successfully![/bold green]"))
    console.print(Align.center(f"[dim]Test finished at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}[/dim]"))
    console.print("="*60)

if __name__ == "__main__":
    main()
