/*
 __  __ ____  ____    ____       _
|  \/  / ___||  _ \  |  _ \ _ __(_)_   _____ _ __
| |\/| \___ \| |_) | | | | | '__| \ \ / / _ \ '__|
| |  | |___) |  _ <  | |_| | |  | |\ V /  __/ |
|_|  |_|____/|_| \_\ |____/|_|  |_| \_/ \___|_|
 */

 /*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License
 See the LICENSE file in the project root for more details.
================================================================================
*/
#pragma once
#include <ntddk.h>

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD     MsrUnload;

_Dispatch_type_(IRP_MJ_CREATE)
DRIVER_DISPATCH  MsrCreateClose;

_Dispatch_type_(IRP_MJ_CLOSE)
DRIVER_DISPATCH  MsrCreateClose;

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH  MsrDeviceControl;
