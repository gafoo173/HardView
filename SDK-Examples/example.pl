#!/usr/bin/perl
# example.pl
# Example of using the HardwareTemp.dll library from Perl.
# This example loads the library, checks for any additional dependencies,
# then tests some functions and prints the results, such as CPU Temperature
#
# Requirements: 
# - Win32::API module: cpan install Win32::API
# - Or: ppm install Win32-API (for ActivePerl)

use strict;
use warnings;
use Win32::API;
use File::Basename;
use File::Spec;
use Cwd 'abs_path';

# HardwareTemp class to encapsulate the DLL functionality
package HardwareTemp;

# Import required modules for this package
use File::Basename;
use File::Spec;
use Cwd 'abs_path';

sub new {
    my ($class) = @_;
    my $self = {
        dll_handle => undef,
        initialized => 0,
        functions => {}
    };
    
    bless $self, $class;
    
    # Check dependencies and load DLL
    eval {
        $self->check_required_dlls();
        $self->load_dll();
        $self->load_functions();
    };
    
    if ($@) {
        die "Failed to initialize HardwareTemp: $@";
    }
    
    return $self;
}

# Check if required DLLs exist next to the executable/script
sub check_required_dlls {
    my ($self) = @_;
    
    my @required_dlls = (
        'HardwareTemp.dll',
        'HardwareWrapper.dll',
        'LibreHardwareMonitorLib.dll',
        'HidSharp.dll'
    );
    
    # Get script/executable directory
    my $script_path = abs_path($0);
    my $script_dir = dirname($script_path);
    
    print "Checking for required DLL files...\n";
    
    my @missing = ();
    for my $dll (@required_dlls) {
        my $dll_path = File::Spec->catfile($script_dir, $dll);
        unless (-f $dll_path) {
            push @missing, $dll;
        }
    }
    
    if (@missing) {
        my $error_msg = "ERROR: The following required DLL files are missing from the script directory:\n";
        for my $dll (@missing) {
            $error_msg .= "  - $dll\n";
        }
        $error_msg .= "\nScript directory: $script_dir\n";
        $error_msg .= "\nPlease ensure all DLL files are placed in the same directory as the script.";
        die $error_msg;
    }
}

# Load the DLL
sub load_dll {
    my ($self) = @_;
    
    # Get script directory and construct DLL path
    my $script_path = abs_path($0);
    my $script_dir = dirname($script_path);
    my $dll_path = File::Spec->catfile($script_dir, 'HardwareTemp.dll');
    
    # Load the DLL using Win32::API
    # We'll load each function individually since Win32::API works per-function
}

# Load all required functions from the DLL
sub load_functions {
    my ($self) = @_;
    
    # Get script directory for DLL path
    my $script_path = abs_path($0);
    my $script_dir = dirname($script_path);
    my $dll_path = File::Spec->catfile($script_dir, 'HardwareTemp.dll');
    
    # Define function mappings
    my %function_defs = (
        'init' => {
            name => 'InitHardwareTempMonitor',
            params => [],
            return => 'I'  # int
        },
        'shutdown' => {
            name => 'ShutdownHardwareTempMonitor', 
            params => [],
            return => 'V'  # void
        },
        'update' => {
            name => 'UpdateHardwareMonitorTemp',
            params => [],
            return => 'V'  # void
        },
        'get_cpu_temp' => {
            name => 'GetCpuTemperatureTemp',
            params => [],
            return => 'D'  # double
        },
        'get_gpu_temp' => {
            name => 'GetGpuTemperatureTemp',
            params => [],
            return => 'D'  # double
        },
        'get_mobo_temp' => {
            name => 'GetMotherboardTemperatureTemp',
            params => [],
            return => 'D'  # double
        },
        'get_storage_temp' => {
            name => 'GetStorageTemperatureTemp',
            params => [],
            return => 'D'  # double
        },
        'get_avg_cpu_temp' => {
            name => 'GetAverageCpuCoreTemperatureTemp',
            params => [],
            return => 'D'  # double
        },
        'get_max_cpu_temp' => {
            name => 'GetMaxCpuCoreTemperatureTemp',
            params => [],
            return => 'D'  # double
        },
        'get_cpu_fan_rpm' => {
            name => 'GetCpuFanRpmTemp',
            params => [],
            return => 'D'  # double
        },
        'get_gpu_fan_rpm' => {
            name => 'GetGpuFanRpmTemp',
            params => [],
            return => 'D'  # double
        }
    );
    
    # Load each function
    for my $func_key (keys %function_defs) {
        my $func_def = $function_defs{$func_key};
        
        my $api = Win32::API->new(
            $dll_path,                    # DLL path
            $func_def->{name},           # Function name
            $func_def->{params},         # Parameters
            $func_def->{return}          # Return type
        );
        
        unless ($api) {
            die "Failed to load function: $func_def->{name}";
        }
        
        $self->{functions}{$func_key} = $api;
    }
}

# Initialize the hardware monitor
sub init {
    my ($self) = @_;
    return 0 if $self->{initialized};
    
    my $result = $self->{functions}{init}->Call();
    $self->{initialized} = 1 if $result == 0;
    return $result;
}

# Shutdown the hardware monitor
sub shutdown {
    my ($self) = @_;
    if ($self->{initialized}) {
        $self->{functions}{shutdown}->Call();
        $self->{initialized} = 0;
    }
}

# Update hardware monitor data
sub update {
    my ($self) = @_;
    if ($self->{initialized}) {
        $self->{functions}{update}->Call();
    }
}

# Temperature getter methods
sub get_cpu_temp {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_cpu_temp}->Call() : -99.0;
}

sub get_gpu_temp {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_gpu_temp}->Call() : -99.0;
}

sub get_motherboard_temp {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_mobo_temp}->Call() : -99.0;
}

sub get_storage_temp {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_storage_temp}->Call() : -99.0;
}

sub get_avg_cpu_core_temp {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_avg_cpu_temp}->Call() : -99.0;
}

sub get_max_cpu_core_temp {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_max_cpu_temp}->Call() : -99.0;
}

# Fan RPM getter methods
sub get_cpu_fan_rpm {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_cpu_fan_rpm}->Call() : -99.0;
}

sub get_gpu_fan_rpm {
    my ($self) = @_;
    return $self->{initialized} ? $self->{functions}{get_gpu_fan_rpm}->Call() : -99.0;
}

# Helper subroutines for displaying values with error handling
package main;

sub display_temp_value {
    my ($name, $value, $unit) = @_;
    
    if ($value == -1.0) {
        print "$name: ERROR - Make sure the program is running as Administrator. If already running as Admin, this sensor is not supported on your system.\n";
    } elsif ($value == -99.0) {
        print "$name: ERROR - Make sure these DLLs are present next to the application:\n";
        print "  - HardwareWrapper.dll\n";
        print "  - HardwareTemp.dll\n"; 
        print "  - LibreHardwareMonitorLib.dll\n";
    } else {
        printf "%s: %.1f %s\n", $name, $value, $unit;
    }
}

sub display_rpm_value {
    my ($name, $value) = @_;
    
    if ($value == -1.0) {
        print "$name: ERROR - Make sure the program is running as Administrator. If already running as Admin, this sensor is not supported on your system.\n";
    } elsif ($value == -99.0) {
        print "$name: ERROR - Make sure these DLLs are present next to the application:\n";
        print "  - HardwareWrapper.dll\n";
        print "  - HardwareTemp.dll\n";
        print "  - LibreHardwareMonitorLib.dll\n";
    } else {
        printf "%s: %.0f RPM\n", $name, $value;
    }
}

# Main execution
sub main {
    my $hw;
    
    eval {
        print "All required DLL files found. Initializing hardware monitor...\n";
        
        # Create new HardwareTemp instance
        $hw = HardwareTemp->new();
        
        # Initialize the hardware monitor
        my $init_result = $hw->init();
        if ($init_result != 0) {
            die "Failed to init hardware monitor. Error code: $init_result";
        }
        
        print "Hardware monitor initialized successfully!\n";
        
        # Update hardware data before reading
        $hw->update();
        
        print "=== Temperature Readings ===\n";
        display_temp_value("CPU Temp", $hw->get_cpu_temp(), "°C");
        display_temp_value("GPU Temp", $hw->get_gpu_temp(), "°C"); 
        display_temp_value("Motherboard Temp", $hw->get_motherboard_temp(), "°C");
        display_temp_value("Storage Temp", $hw->get_storage_temp(), "°C");
        display_temp_value("Average CPU Core Temp", $hw->get_avg_cpu_core_temp(), "°C");
        display_temp_value("Max CPU Core Temp", $hw->get_max_cpu_core_temp(), "°C");
        
        print "\n=== Fan Speed Readings ===\n";
        display_rpm_value("CPU Fan", $hw->get_cpu_fan_rpm());
        display_rpm_value("GPU Fan", $hw->get_gpu_fan_rpm());
        
        print "\nHardware monitor shutdown successfully!\n";
    };
    
    if ($@) {
        print STDERR "Error: $@\n";
        exit 1;
    }
    
    # Ensure cleanup
    if ($hw) {
        $hw->shutdown();
    }
}

# Run the main function
main();
