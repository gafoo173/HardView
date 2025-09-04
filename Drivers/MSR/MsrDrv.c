
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

#include "msrdrv.h"
#include <intrin.h>

#define DEVICE_NAME      L"\\Device\\MsrDrv"
#define SYMLINK_NAME     L"\\DosDevices\\MsrDrv"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, MsrUnload)
#pragma alloc_text(PAGE, MsrCreateClose)
#pragma alloc_text(PAGE, MsrDeviceControl)
#endif
#define FILE_DEVICE_MSR    0x8000
//=================================================== =IOCTLS========================================================//
#define IOCTL_MSR_READ   CTL_CODE(FILE_DEVICE_MSR, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MSR_WRITE  CTL_CODE(FILE_DEVICE_MSR, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MSR_READ_INPUT {
    UINT32 Register;
} MSR_READ_INPUT, * PMSR_READ_INPUT;

typedef struct _MSR_READ_OUTPUT {
    UINT64 Value;
} MSR_READ_OUTPUT, * PMSR_READ_OUTPUT;

typedef struct _MSR_WRITE_INPUT {
    UINT32 Register;
    UINT64 Value;
} MSR_WRITE_INPUT, * PMSR_WRITE_INPUT;
//=================================================Create Driver====================================================================
static PDEVICE_OBJECT g_DeviceObject = NULL;

NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

#if !(defined(_M_IX86) || defined(_M_AMD64))
    return STATUS_NOT_SUPPORTED;
#endif

    UNICODE_STRING devName = RTL_CONSTANT_STRING(DEVICE_NAME);
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(SYMLINK_NAME);

    NTSTATUS status = IoCreateDevice(
        DriverObject,
        0,
        &devName,
        FILE_DEVICE_MSR,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &g_DeviceObject
    );

    if (!NT_SUCCESS(status)) {
        return status;
    }

    g_DeviceObject->Flags |= DO_BUFFERED_IO;

    status = IoCreateSymbolicLink(&symLink, &devName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(g_DeviceObject);
        g_DeviceObject = NULL;
        return status;
    }

    DriverObject->DriverUnload = MsrUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = MsrCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = MsrCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MsrDeviceControl;

    g_DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    return STATUS_SUCCESS;
}

VOID
MsrUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    PAGED_CODE();

    UNICODE_STRING symLink = RTL_CONSTANT_STRING(SYMLINK_NAME);
    IoDeleteSymbolicLink(&symLink);

    if (g_DeviceObject) {
        IoDeleteDevice(g_DeviceObject);
        g_DeviceObject = NULL;
    }
}

NTSTATUS
MsrCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
//===============================================Read And Write Functions==============================================
static __forceinline NTSTATUS SafeReadMsr(UINT32 reg, UINT64* outVal)
{
#if defined(_M_IX86) || defined(_M_AMD64)
    __try {
        *outVal = __readmsr(reg);
        return STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        *outVal = 0;
        return STATUS_PRIVILEGED_INSTRUCTION;
        return STATUS_PRIVILEGED_INSTRUCTION;
    }
#else
    UNREFERENCED_PARAMETER(reg);
    UNREFERENCED_PARAMETER(outVal);
    return STATUS_NOT_SUPPORTED;
#endif
}

static __forceinline NTSTATUS SafeWriteMsr(UINT32 reg, UINT64 val)
{
#if defined(_M_IX86) || defined(_M_AMD64)
    __try {
        __writemsr(reg, val);
        return STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return STATUS_PRIVILEGED_INSTRUCTION;
    }
#else
    UNREFERENCED_PARAMETER(reg);
    UNREFERENCED_PARAMETER(val);
    return STATUS_NOT_SUPPORTED;
#endif
}

NTSTATUS
MsrDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    ULONG_PTR info = 0;

    ULONG code = irpSp->Parameters.DeviceIoControl.IoControlCode;

    switch (code)
    {
    case IOCTL_MSR_READ:
    {
        if (irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof(MSR_READ_INPUT) ||
            irpSp->Parameters.DeviceIoControl.OutputBufferLength < sizeof(MSR_READ_OUTPUT)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PMSR_READ_INPUT in = (PMSR_READ_INPUT)Irp->AssociatedIrp.SystemBuffer;
        PMSR_READ_OUTPUT out = (PMSR_READ_OUTPUT)Irp->AssociatedIrp.SystemBuffer;

        UINT64 val = 0;
        status = SafeReadMsr(in->Register, &val);
        if (NT_SUCCESS(status)) {
            out->Value = val;
            info = sizeof(MSR_READ_OUTPUT);
        }
        break;
    }

    case IOCTL_MSR_WRITE:
    {
        if (irpSp->Parameters.DeviceIoControl.InputBufferLength < sizeof(MSR_WRITE_INPUT)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PMSR_WRITE_INPUT in = (PMSR_WRITE_INPUT)Irp->AssociatedIrp.SystemBuffer;
        status = SafeWriteMsr(in->Register, in->Value);
        if (NT_SUCCESS(status)) {
            info = 0;
        }
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = info;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}
