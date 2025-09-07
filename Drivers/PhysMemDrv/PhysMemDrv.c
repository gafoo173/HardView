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
/*
MIT License

Copyright (c) 2025 gafoo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#include <ntddk.h>

#define DRIVER_TAG 'rPhD'

 // IOCTLs (METHOD_BUFFERED for simplicity)
#define IOCTL_READ_PHYS  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define IOCTL_WRITE_MMIO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

// Limits
#define MAX_READ_SIZE (64 * 1024)  // max bytes read in a single call
#define MAX_MAP_SIZE  (128 * 1024) // safety cap for mapping

// Input structures (user -> driver)
#pragma pack(push, 1)
typedef struct _PHYS_READ_INPUT {
    PHYSICAL_ADDRESS Address; // starting physical address
    ULONG Length;             // number of bytes to read
} PHYS_READ_INPUT, * PPHYS_READ_INPUT;

typedef struct _MMIO_WRITE_INPUT {
    PHYSICAL_ADDRESS Address; // physical address to write
    ULONG Value;              // 32-bit value to write
    ULONG Width;              // 1=byte,2=word,4=dword (only 1/2/4 supported)
} MMIO_WRITE_INPUT, * PMMIO_WRITE_INPUT;
#pragma pack(pop)

// Globals
PVOID g_DeviceObject = NULL;
PHYSICAL_ADDRESS g_MaxPhys = { 0 }; // max allowed physical address (computed at init)

// Forward
DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;
_Dispatch_type_(IRP_MJ_CREATE) DRIVER_DISPATCH DeviceCreateClose;
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH DeviceIoControl;

static VOID ComputeMaxPhysRange(VOID)
{
    PPHYSICAL_MEMORY_RANGE ranges = MmGetPhysicalMemoryRanges();
    if (!ranges) {
        g_MaxPhys.QuadPart = 0;
        return;
    }

    // Find the end of the highest range that has non-zero Length
    PPHYSICAL_MEMORY_RANGE iter = ranges;
    PHYSICAL_ADDRESS maxAddr = { 0 };
    while (iter->NumberOfBytes.QuadPart != 0) {  // Fixed: access QuadPart member
        PHYSICAL_ADDRESS endAddr;
        endAddr.QuadPart = iter->BaseAddress.QuadPart + iter->NumberOfBytes.QuadPart;  // Fixed: proper LARGE_INTEGER access
        if (endAddr.QuadPart > maxAddr.QuadPart) maxAddr = endAddr;
        iter++;
    }
    g_MaxPhys = maxAddr;

    ExFreePool(ranges);
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControl;

    // create a simple device (named) so user-mode can open it with CreateFile
    UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\PhysMemDrv");
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\PhysMemDrv");

    PDEVICE_OBJECT DeviceObject = NULL;
    NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
    if (!NT_SUCCESS(status)) return status;

    status = IoCreateSymbolicLink(&symLink, &devName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        return status;
    }

    g_DeviceObject = DeviceObject;

    // compute a conservative allowed max physical address using OS ranges
    ComputeMaxPhysRange();

    return STATUS_SUCCESS;
}

VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\PhysMemDrv");
    IoDeleteSymbolicLink(&symLink);
    if (g_DeviceObject) IoDeleteDevice(g_DeviceObject);
}

NTSTATUS DeviceCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// Helper: safe check for IRQL and simple sanity
static NTSTATUS ValidateContextAndInput(_In_ PIRP Irp, _In_ ULONG requiredInputSize)
{
	// must be at PASSIVE_LEVEL to access user buffers and map memory
    if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
        return STATUS_INVALID_DEVICE_STATE;
    }
	// check input buffer size
    PIO_STACK_LOCATION st = IoGetCurrentIrpStackLocation(Irp);
    if (st->Parameters.DeviceIoControl.InputBufferLength < requiredInputSize) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    return STATUS_SUCCESS;
}

// Align physical address down to page boundary; return offset
static SIZE_T AlignPhysAddrForMap(_In_ PHYSICAL_ADDRESS PhysAddr, _Out_ PHYSICAL_ADDRESS* Aligned)
{
    LONGLONG pageMask = (LONGLONG)(PAGE_SIZE - 1);
    Aligned->QuadPart = PhysAddr.QuadPart & ~pageMask;
    SIZE_T offset = (SIZE_T)(PhysAddr.QuadPart - Aligned->QuadPart);
    return offset;
}

// Align the physical address and calculate the offset and map size.
// Map the memory range into virtual space using MmMapIoSpace.
// Copy the required data from the mapped memory to the output buffer.
// Unmap the memory after copying, handling any potential exceptions during the process.

NTSTATUS ReadPhysicalMemoryInternal(PHYSICAL_ADDRESS PhysAddr, PVOID OutBuf, SIZE_T Length)
{
	// validate parameters 
	// if Length is 0 or too large return an error: if Length is 0, error because no point in reading 0 bytes
	// if it is too large, return an error because drivers should not read too much memory at once for Performance And Systems Stability
    if (Length == 0 || Length > MAX_READ_SIZE) return STATUS_INVALID_PARAMETER;

    // check against computed max physical (best-effort). If g_MaxPhys is 0 we skip strict check.
    if (g_MaxPhys.QuadPart != 0) {
        ULONGLONG end = PhysAddr.QuadPart + (ULONGLONG)Length;
        if (end > (ULONGLONG)g_MaxPhys.QuadPart) return STATUS_ACCESS_VIOLATION;
    }

    PHYSICAL_ADDRESS aligned;
    SIZE_T offset = AlignPhysAddrForMap(PhysAddr, &aligned);
    SIZE_T mapSize = offset + Length;
    if (mapSize > MAX_MAP_SIZE) return STATUS_INVALID_PARAMETER;

    PVOID mapped = MmMapIoSpace(aligned, mapSize, MmNonCached);
    if (!mapped) return STATUS_INSUFFICIENT_RESOURCES;

    __try {
		//Copy Bytes We needed from mapped memory to output buffer
        RtlCopyMemory(OutBuf, (PUCHAR)mapped + offset, Length);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        MmUnmapIoSpace(mapped, mapSize);
        return STATUS_ACCESS_VIOLATION;
    }

    MmUnmapIoSpace(mapped, mapSize);
    return STATUS_SUCCESS;
}

NTSTATUS WriteMmioInternal(PHYSICAL_ADDRESS PhysAddr, ULONG Value, ULONG Width)
{
	// validate parameters 
    if (!(Width == 1 || Width == 2 || Width == 4)) return STATUS_INVALID_PARAMETER;

    PHYSICAL_ADDRESS aligned;
    SIZE_T offset = AlignPhysAddrForMap(PhysAddr, &aligned);
    SIZE_T mapSize = offset + Width;
    if (mapSize > MAX_MAP_SIZE) return STATUS_INVALID_PARAMETER;

    PVOID mapped = MmMapIoSpace(aligned, mapSize, MmNonCached);
    if (!mapped) return STATUS_INSUFFICIENT_RESOURCES;

    __try {
        PUCHAR base = (PUCHAR)mapped + offset;
        if (Width == 1) {
			//Safe write a byte to the mapped memory
            WRITE_REGISTER_UCHAR((volatile UCHAR*)base, (UCHAR)Value);
        }
        else if (Width == 2) {
            WRITE_REGISTER_USHORT((volatile USHORT*)base, (USHORT)Value);
        }
        else {
            WRITE_REGISTER_ULONG((volatile ULONG*)base, (ULONG)Value);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        MmUnmapIoSpace(mapped, mapSize);
        return STATUS_ACCESS_VIOLATION; //This is often the cause of the error.
    }

    MmUnmapIoSpace(mapped, mapSize);
    return STATUS_SUCCESS;
}

NTSTATUS DeviceIoControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{

    UNREFERENCED_PARAMETER(DeviceObject);

    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    PIO_STACK_LOCATION st = IoGetCurrentIrpStackLocation(Irp);
    ULONG inLen = st->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outLen = st->Parameters.DeviceIoControl.OutputBufferLength;

    UNREFERENCED_PARAMETER(inLen); // suppress warning

    switch (st->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_READ_PHYS:
    {
		//Read physical memory IOCTL
        // validate basic context and sizes
        status = ValidateContextAndInput(Irp, sizeof(PHYS_READ_INPUT));
        if (!NT_SUCCESS(status)) break;

        // METHOD_BUFFERED: input and output share Irp->AssociatedIrp.SystemBuffer
        PHYS_READ_INPUT* in = (PHYS_READ_INPUT*)Irp->AssociatedIrp.SystemBuffer;
        PHYSICAL_ADDRESS addr = in->Address;
        SIZE_T length = in->Length;

        // output buffer must be at least 'length'
        if (outLen < length) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        // For METHOD_BUFFERED, we don't need to probe UserBuffer
        // The I/O Manager has already copied user data to SystemBuffer

        // read
        status = ReadPhysicalMemoryInternal(addr, Irp->AssociatedIrp.SystemBuffer, length);
        if (NT_SUCCESS(status)) {
            Irp->IoStatus.Information = length;
        }
        break;
    }

    case IOCTL_WRITE_MMIO:
    {
        status = ValidateContextAndInput(Irp, sizeof(MMIO_WRITE_INPUT));
        if (!NT_SUCCESS(status)) break;

        PMMIO_WRITE_INPUT in = (PMMIO_WRITE_INPUT)Irp->AssociatedIrp.SystemBuffer;
        PHYSICAL_ADDRESS addr = in->Address;
        ULONG val = in->Value;
        ULONG width = in->Width;

        status = WriteMmioInternal(addr, val, width);
        if (NT_SUCCESS(status)) Irp->IoStatus.Information = 0;
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}
