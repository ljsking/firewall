#include <ntddk.h>
#include <ntddndis.h>
#include <pfhook.h>
#include "Filter.h"

#define dprintf DbgPrint
#define NT_DEVICE_NAME L"\\Device\\MyDriver"
#define DOS_DEVICE_NAME L"\\DosDevices\\MyDriver"

NTSTATUS DrvDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID DrvUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction);
PF_FORWARD_ACTION cbFilterFunction(IN unsigned char *PacketHeader,IN unsigned char *Packet, IN unsigned int PacketLength, IN unsigned int RecvInterfaceIndex, IN unsigned int SendInterfaceIndex, IN unsigned long RecvLinkNextHop, IN unsigned long SendLinkNextHop);
NTSTATUS AddFilterToList(IPFilter *pf);
void ClearFilterList(void);

struct filterList *first = NULL;
struct filterList *last = NULL;
unsigned int PacketLengthsum = 0;

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
	IPFilter			*nf;

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
		SetFilterFunction(cbFilterFunction);
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
			SetFilterFunction(cbFilterFunction);
			dprintf("DrvFltIp.SYS: START_IP_HOOK\n");
			break;

			// ioctl to stop filtering
		case STOP_IP_HOOK:
			PacketLengthsum = 0;
			SetFilterFunction(NULL);
			dprintf("DrvFltIp.SYS: STOP_IP_HOOK\n");
			break;

			// ioctl to add a filter rule
		case ADD_FILTER:
			if(inputBufferLength == sizeof(IPFilter))
			{
				dprintf("DrvFltIp.SYS: ADD_FILTER\n");
				nf = (IPFilter *)ioBuffer;
				AddFilterToList(nf);
			}
			break;

			// ioctl to free filter rule list
		case CLEAR_FILTER:
		
			dprintf("DrvFltIp.SYS: CLEAR_FILTER\n");
			break;

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
	ClearFilterList();
   
    // Delete the symbolic link
    RtlInitUnicodeString(&deviceLinkUnicodeString, DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&deviceLinkUnicodeString);

    
	// Delete the device object
    IoDeleteDevice(DriverObject->DeviceObject);
}

/*++

Routine Description:

    Add a rule to the filter list

Arguments:

      pf - pointer to filter rule


Return Value:

    STATUS_SUCCESS if successful,
    STATUS_INSUFFICIENT_RESOURCES otherwise
 
--*/
NTSTATUS AddFilterToList(IPFilter *pf)
{
	FilterList *aux = NULL;
	// first, we reserve memory (non paged) to the new filter
	aux=(struct filterList *) ExAllocatePool(NonPagedPool, sizeof(struct filterList));

	if(aux == NULL)
	{
		dprintf("Problem reserving memory\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//fill the new structure
	aux->ipf.destinationIp = pf->destinationIp;
	aux->ipf.sourceIp = pf->sourceIp;

	aux->ipf.destinationMask = pf->destinationMask;
	aux->ipf.sourceMask = pf->sourceMask;

	aux->ipf.destinationPort = pf->destinationPort;
	aux->ipf.sourcePort = pf->sourcePort;

	aux->ipf.protocol = pf->protocol;

	aux->ipf.drop=pf->drop;

	
	//Add the new filter to the filter list
	if(first == NULL)
	{
		first = last = aux;
		first->next = NULL;
	}
	else
	{
		last->next = aux;
		last = aux;
		last->next = NULL;
	}

	dprintf("Rule Added2\n\t%x %x\n\t%x %x\n\t%x\n\t%x", aux->ipf.sourceIp
														, aux->ipf.sourceMask
														, aux->ipf.destinationIp
														, aux->ipf.destinationMask
														, aux->ipf.sourcePort
														, aux->ipf.destinationPort);

	return STATUS_SUCCESS;
}

/*++

Routine Description:

    Get a reference to IpFilterDriver so we will be able to install the filter

Arguments:

    pDeviceObject - pointer to a pointer of device object

    pFileObject   - pointer to a pointer of file object

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction)
{
	NTSTATUS status = STATUS_SUCCESS, waitStatus=STATUS_SUCCESS;
	UNICODE_STRING filterName;
	PDEVICE_OBJECT ipDeviceObject=NULL;
	PFILE_OBJECT ipFileObject=NULL;

	PF_SET_EXTENSION_HOOK_INFO filterData;

	KEVENT event;
	IO_STATUS_BLOCK ioStatus;
	PIRP irp;

	dprintf("Getting pointer to IpFilterDriver\n");

	//first of all, we have to get a pointer to IpFilterDriver Device
	RtlInitUnicodeString(&filterName, DD_IPFLTRDRVR_DEVICE_NAME);
	status = IoGetDeviceObjectPointer(&filterName,STANDARD_RIGHTS_ALL, &ipFileObject, &ipDeviceObject);

	if(NT_SUCCESS(status))
	{
		//initialize the struct with functions parameters
		filterData.ExtensionPointer = filterFunction;

		//we need initialize the event used later by the IpFilterDriver to signal us
		//when it finished its work
		KeInitializeEvent(&event, NotificationEvent, FALSE);

		//we build the irp needed to establish fitler function
		irp = IoBuildDeviceIoControlRequest(IOCTL_PF_SET_EXTENSION_POINTER,
				ipDeviceObject,
				(PVOID) &filterData,
				sizeof(PF_SET_EXTENSION_HOOK_INFO),
				NULL,
				0,
				FALSE,
				&event,
				&ioStatus);


		if(irp != NULL)
		{
			// we send the IRP
			status = IoCallDriver(ipDeviceObject, irp);

			//and finally, we wait for "acknowledge" of IpDriverFilter
			if (status == STATUS_PENDING) 
			{
				waitStatus = KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);

				if (waitStatus 	!= STATUS_SUCCESS ) 
					dprintf("Error waiting for IpFilterDriver response.");
			}

			status = ioStatus.Status;

			if(!NT_SUCCESS(status))
				dprintf("Error, IO error with ipFilterDriver\n");
		}

		else
		{
			//if we cant allocate the space, we return the corresponding code error
			status = STATUS_INSUFFICIENT_RESOURCES;

			dprintf("Error building IpFilterDriver IRP\n");
		}

		if(ipFileObject != NULL)
			ObDereferenceObject(ipFileObject);

		ipFileObject = NULL;
		ipDeviceObject = NULL;
	}

	else
		dprintf("Error while getting the pointer\n");

	return status;
}

/*++

Routine Description:

    Filter each packet is received or sended

	To see parameters and return you can read it in MSDN
--*/

PF_FORWARD_ACTION cbFilterFunction(IN unsigned char *PacketHeader,IN unsigned char *Packet, IN unsigned int PacketLength, IN unsigned int RecvInterfaceIndex, IN unsigned int SendInterfaceIndex, IN unsigned long RecvLinkNextHop, IN unsigned long SendLinkNextHop)
{
	IPPacket *ipp;
	TCPHeader *tcph;
	UDPHeader *udph;

	int countRule=0;

	struct filterList *aux = first;

	//we "extract" the ip Header 
	ipp=(IPPacket *)PacketHeader;

	dprintf("Tama?: %x, %d", PacketLength, RecvInterfaceIndex);
	dprintf("Source: %x\nDestination: %x\nProtocol: %d", ipp->ipSource, ipp->ipDestination, ipp->ipProtocol);

	dprintf("PacketLength: %d", PacketLength);
	PacketLengthsum +=PacketLength;
	dprintf("PacketLength 총 합: %d", PacketLengthsum);

	//otherwise, we compare the packet with our rules
	while(aux != NULL)
	{
		dprintf("Comparing with Rule %d", countRule);

		//if protocol is the same....
		if(aux->ipf.protocol == 0 || ipp->ipProtocol == aux->ipf.protocol)
		{
			//we look in source Address
			if(aux->ipf.sourceIp != 0 && (ipp->ipSource & aux->ipf.sourceMask) != aux->ipf.sourceIp)
			{
				aux=aux->next;
			
				countRule++;
				continue;
			}
									
			// we look in destination address
			if(aux->ipf.destinationIp != 0 && (ipp->ipDestination & aux->ipf.destinationMask) != aux->ipf.destinationIp)
			{
				aux=aux->next;

				countRule++;
				continue;
			}
			
			//if we have a tcp packet, we look in ports
			//tcp, protocol = 6
            if(ipp->ipProtocol == 6) 
			{
                tcph=(TCPHeader *)Packet; 
                dprintf("티씨피입니다\n");
                dprintf("%d     %d\n",tcph->sourcePort, aux->ipf.sourcePort);
				if(aux->ipf.sourcePort == 0 || tcph->sourcePort == aux->ipf.sourcePort)
				{ 
					if(aux->ipf.destinationPort == 0 || tcph->destinationPort == aux->ipf.destinationPort) //puerto tcp destino
					{

                      	//now we decided what to do with the packet
						if(aux->ipf.drop)
                        {
                            dprintf("TCP Packet Drop\n");
                            return  PF_DROP;
                        }

							else
								return PF_FORWARD;
					}
				}
			}
				
			//udp, protocol = 17
			else if(ipp->ipProtocol == 17) 
			{
				udph=(UDPHeader *)Packet; 
                 dprintf("유디피입니다\n");

				if(aux->ipf.sourcePort == 0 || udph->sourcePort == aux->ipf.sourcePort) 
				{ 
					if(aux->ipf.destinationPort == 0 || udph->destinationPort == aux->ipf.destinationPort) 
					{
                      	//now we decided what to do with the packet
						if(aux->ipf.drop)
					    {
                            dprintf("UDP Packet Drop\n");
                            return  PF_DROP;
                        }	
						
						else
							return PF_FORWARD;
					}
				}
			}	
			
			else
			{
				//for other packet we dont look more and ....
				//now we decided what to do with the packet
				if(aux->ipf.drop)
					return  PF_DROP;
				else
					return PF_FORWARD;
			}	
		}
		
		//compare with the next rule
		countRule++;
		aux=aux->next;
	}

	return PF_FORWARD;
}

/*++

Routine Description:

    Remove the linked list where the rules were saved.

Arguments:


Return Value:

 
--*/
void ClearFilterList(void)
{
	struct filterList *aux = NULL;

	//free the linked list
	dprintf("Removing the filter List...");
	
	while(first != NULL)
	{
		aux = first;
		first = first->next;
		ExFreePool(aux);

		dprintf("One Rule removed");
	}

	first = last = NULL;

	dprintf("Removed is complete.");
}