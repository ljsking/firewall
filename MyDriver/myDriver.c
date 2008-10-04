#include "Filter.h"
#include <ntddk.h>

#define dprintf DbgPrint
#define NT_DEVICE_NAME L"\\Device\\MyDriver"
#define DOS_DEVICE_NAME L"\\DosDevices\\MyDriver"

//
// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
//




//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//


NTSTATUS DrvDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID DrvUnload(IN PDRIVER_OBJECT DriverObject);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	PDEVICE_OBJECT         deviceObject = NULL;
    NTSTATUS               ntStatus;
    UNICODE_STRING         deviceNameUnicodeString;
    UNICODE_STRING         deviceLinkUnicodeString;
	
	dprintf("MyDriver.SYS: entering DriverEntry\n");
	//we have to create the device
	RtlInitUnicodeString(&deviceNameUnicodeString, NT_DEVICE_NAME);

	ntStatus = IoCreateDevice(DriverObject, 
								0,
								&deviceNameUnicodeString, 
								FILE_DEVICE_DRVFLTIP,
								0,
								FALSE,
								&deviceObject);

    if ( NT_SUCCESS(ntStatus) )
    {
    
        // Create a symbolic link that Win32 apps can specify to gain access
        // to this driver/device
        RtlInitUnicodeString(&deviceLinkUnicodeString, DOS_DEVICE_NAME);

        ntStatus = IoCreateSymbolicLink(&deviceLinkUnicodeString, &deviceNameUnicodeString);

        if ( !NT_SUCCESS(ntStatus) )
        {
            dprintf("myDriver.SYS: IoCreateSymbolicLink failed\n");
        }

        //
        // Create dispatch points for device control, create, close.
        //

        DriverObject->MajorFunction[IRP_MJ_CREATE]         = 
        DriverObject->MajorFunction[IRP_MJ_CLOSE]          =
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvDispatch;
        DriverObject->DriverUnload                         = DrvUnload;
    }

    if ( !NT_SUCCESS(ntStatus) )
    {
        dprintf("myDriver.SYS: Error in initialization. Unloading...");

		DrvUnload(DriverObject);
    }

    return ntStatus;
}

/*++

Routine Description:

    Process the IRPs sent to this device.

Arguments:

    DeviceObject - pointer to a device object

    Irp          - pointer to an I/O Request Packet

Return Value:

--*/
NTSTATUS DrvDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{

    PIO_STACK_LOCATION  irpStack;
    PVOID               ioBuffer;
    ULONG               inputBufferLength;
    ULONG               outputBufferLength;
    ULONG               ioControlCode;
    NTSTATUS            ntStatus;

    Irp->IoStatus.Status      = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;


 
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    irpStack = IoGetCurrentIrpStackLocation(Irp);


    // Get the pointer to the input/output buffer and it's length
    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_CREATE:

        dprintf("DrvFltIp.SYS: IRP_MJ_CREATE\n");

        break;

    case IRP_MJ_CLOSE:

        dprintf("DrvFltIp.SYS: IRP_MJ_CLOSE\n");

        break;

    case IRP_MJ_DEVICE_CONTROL:

        dprintf("DrvFltIp.SYS: IRP_MJ_DEVICE_CONTROL\n");

        ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

        switch (ioControlCode)
        {
			// ioctl code to start filtering
			case START_IP_HOOK:
			{
           		dprintf("DrvFltIp.SYS: START_IP_HOOK\n");

				break;
			}

			// ioctl to stop filtering
			case STOP_IP_HOOK:
			{
               dprintf("DrvFltIp.SYS: STOP_IP_HOOK\n");
	            
				break;
			}

            // ioctl to add a filter rule
			case ADD_FILTER:
			{
				dprintf("DrvFltIp.SYS: ADD_FILTER\n");

				break;
			}

			// ioctl to free filter rule list
			case CLEAR_FILTER:
			{
				dprintf("DrvFltIp.SYS: CLEAR_FILTER\n");

				break;
			}

			default:
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;

				dprintf("DrvFltIp.SYS: unknown IRP_MJ_DEVICE_CONTROL\n");

				break;
        }

        break;
    }


    //
    // DON'T get cute and try to use the status field of
    // the irp in the return status.  That IRP IS GONE as
    // soon as you call IoCompleteRequest.
    //

    ntStatus = Irp->IoStatus.Status;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);


    //
    // We never have pending operation so always return the status code.
    //

    return ntStatus;
}

/*++

Routine Description:

    Free all the allocated resources, etc.

Arguments:

    DriverObject - pointer to a driver object

Return Value:


--*/
VOID DrvUnload(IN PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING         deviceLinkUnicodeString;

	dprintf("DrvFltIp.SYS: Unloading\n");

    //SetFilterFunction(NULL);

	// Free any resources
	//ClearFilterList();
   
    // Delete the symbolic link
    RtlInitUnicodeString(&deviceLinkUnicodeString, DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&deviceLinkUnicodeString);

    
	// Delete the device object
    IoDeleteDevice(DriverObject->DeviceObject);
}