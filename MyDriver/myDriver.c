#include <ntddk.h>
#include <ntddndis.h>
#include <pfhook.h>
#include <tchar.h>

#include "Filter.h"

#define dprintf DbgPrint
#define NT_DEVICE_NAME L"\\Device\\MyDriver"
#define DOS_DEVICE_NAME L"\\DosDevices\\MyDriver"

NTSTATUS DrvDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID DrvUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS SetFilterFunction(PacketFilterExtensionPtr filterFunction);
PF_FORWARD_ACTION cbFilterFunction(IN unsigned char *PacketHeader,IN unsigned char *Packet, IN unsigned int PacketLength, IN unsigned int RecvInterfaceIndex, IN unsigned int SendInterfaceIndex, IN unsigned long RecvLinkNextHop, IN unsigned long SendLinkNextHop);
NTSTATUS AddFilterToList(IPFilter *pf);
NTSTATUS AddWordToList(WordFilter *wf);
PortList *FindPort(USHORT port);
void ClearFilterList(void);
void ClearWordList(void);
void ClearPortList(void);
void DeleteRule(USHORT);
void DeleteWord(USHORT);
void DeletePort(USHORT);

struct filterList *firstFilter = NULL;
struct filterList *lastFilter = NULL;
struct wordList *firstWord = NULL;
struct wordList *lastWord = NULL;
struct portList *firstPort = NULL;
struct portList *lastPort = NULL;
unsigned int PacketLengthsum = 0;
FirewallSetting setting;

unsigned short ntohs(unsigned short value)
{
    return (value >> 8) | (value << 8);
}

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
			dprintf("MyDriver.SYS: IoCreateSymbolicLink failed\n");
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
		dprintf("MyDriver.SYS: Error in initialization. Unloading...");

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
	WordFilter			*wf;
	FirewallSetting 	*fs;
	PortList			*pl;

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
		dprintf("MyDriver.SYS: IRP_MJ_CREATE\n");
		//SetFilterFunction(cbFilterFunction);
		break;

	case IRP_MJ_CLOSE:
		SetFilterFunction(NULL);
		ClearFilterList();
		ClearWordList();
		ClearPortList();
		dprintf("MyDriver.SYS: IRP_MJ_CLOSE\n");
		break;

	case IRP_MJ_DEVICE_CONTROL:
		ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
		//%dprintf("MyDriver.SYS: IRP_MJ_DEVICE_CONTROL :%d\n", ioControlCode);

		//memcpy(lpOutBuf, myTemp, nOutBufSize);
		switch (ioControlCode)
		{
			// ioctl code to start filtering
		case START_IP_HOOK:
			SetFilterFunction(cbFilterFunction);
			dprintf("MyDriver.SYS: START_IP_HOOK\n");
			break;

			// ioctl to stop filtering
		case STOP_IP_HOOK:
			PacketLengthsum = 0;
			SetFilterFunction(NULL);
			dprintf("MyDriver.SYS: STOP_IP_HOOK\n");
			break;

			// ioctl to add a filter rule
		case ADD_FILTER:
			if(inputBufferLength == sizeof(IPFilter))
			{
				//dprintf("MyDriver.SYS: ADD_FILTER\n");
				nf = (IPFilter *)ioBuffer;
				AddFilterToList(nf);
			}
			break;

		case ADD_WORD:
			//dprintf("MyDriver.SYS: ADD_WORD1 %d %d\n", inputBufferLength, sizeof(WordFilter));
			if(inputBufferLength == sizeof(WordFilter))
			{
				//dprintf("MyDriver.SYS: ADD_WORD2\n");
				wf = (WordFilter *)ioBuffer;
				AddWordToList(wf);
			}
			break;

			// ioctl to free filter rule list
		case CLEAR_FILTER:
			ClearFilterList();
			//dprintf("MyDriver.SYS: CLEAR_FILTER\n");
			break;

		case SET_SETTING:
			//dprintf("MyDriver.SYS: SET_SETTING %d %d\n", inputBufferLength, sizeof(FirewallSetting));
			if(inputBufferLength == sizeof(FirewallSetting))
			{
				fs = (FirewallSetting *)ioBuffer;
				setting.IPFilter = fs->IPFilter;
				setting.Exceed = fs->Exceed;
				setting.IP = fs->IP;
				setting.PortMonitor = fs->PortMonitor;
				setting.SessionFilter = fs->SessionFilter;
				setting.WordFilter = fs->WordFilter;
				//dprintf("MyDriver.SYS: SET_SETTING\n");
				//memcpy(&setting, ioBuffer, sizeof(FirewallSetting));
			}
			break;

		case GET_TOTAL:
			//dprintf("MyDriver.SYS: GET_SETTING %d %d\n", outputBufferLength, sizeof(int));
			if(outputBufferLength == sizeof(ULONG))
			{
				RtlCopyMemory(ioBuffer, &PacketLengthsum, sizeof(ULONG));
				Irp->IoStatus.Information = sizeof(ULONG);
			}
			break;

		case GET_PORTUSAGE:
			//dprintf("MyDriver.SYS: GET_PORTUSAGE %d %d %d %d\n", inputBufferLength, sizeof(USHORT), outputBufferLength, sizeof(ULONG));
			if(outputBufferLength == sizeof(int) && inputBufferLength == sizeof(USHORT))
			{
				//dprintf("MyDriver.SYS: GET_PORTUSAGE input: %u\n",*((USHORT *)ioBuffer));
				pl = FindPort(*((USHORT *)ioBuffer));
				if(pl!=NULL)
				{
					RtlCopyMemory(ioBuffer, &(pl->pusage.usage), sizeof(ULONG));
					Irp->IoStatus.Information = sizeof(ULONG);
					//dprintf("MyDriver.SYS: GET_PORTUSAGE usage %d\n",pl->pusage.usage);
				}
				
			}
			break;

		case DEL_RULE:
			dprintf("MyDriver.SYS: DEL_RULE input: %d, %d\n",inputBufferLength, sizeof(USHORT));
			if(inputBufferLength == sizeof(USHORT))
			{
				DeleteRule(*((USHORT *)ioBuffer));
				//dprintf("MyDriver.SYS: DEL_PORT usage %d\n",*((USHORT *)ioBuffer));
			}
			break;

		case DEL_WORD:
			if(inputBufferLength == sizeof(USHORT))
			{
				DeleteWord(*((USHORT *)ioBuffer));
				//dprintf("MyDriver.SYS: DEL_PORT usage %d\n",*((USHORT *)ioBuffer));
			}
			break;
		
		case DEL_PORT:
			if(inputBufferLength == sizeof(USHORT))
			{
				DeletePort(*((USHORT *)ioBuffer));
				//dprintf("MyDriver.SYS: DEL_PORT usage %d\n",*((USHORT *)ioBuffer));
			}
			break;

		default:
			Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
			dprintf("MyDriver.SYS: unknown IRP_MJ_DEVICE_CONTROL\n");
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

	dprintf("MyDriver.SYS: Unloading\n");

    SetFilterFunction(NULL);

	// Free any resources
	ClearFilterList();
	ClearWordList();
	ClearPortList();

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
		dprintf("MyDriver.SYS: Problem reserving memory\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//fill the new structure
	aux->ipf.id = pf->id;
	aux->ipf.destinationIp = pf->destinationIp;
	aux->ipf.sourceIp = pf->sourceIp;

	aux->ipf.destinationMask = pf->destinationMask;
	aux->ipf.sourceMask = pf->sourceMask;

	aux->ipf.destinationPort = pf->destinationPort;
	aux->ipf.sourcePort = pf->sourcePort;

	aux->ipf.protocol = pf->protocol;

	//Add the new filter to the filter list
	if(firstFilter == NULL)
	{
		firstFilter = lastFilter = aux;
		firstFilter->next = NULL;
	}
	else
	{
		lastFilter->next = aux;
		lastFilter = aux;
		lastFilter->next = NULL;
	}

	dprintf("MyDriver.SYS: Rule Added2\n\t%x %x\n\t%x %x\n\t%x\n\t%x", aux->ipf.sourceIp
														, aux->ipf.sourceMask
														, aux->ipf.destinationIp
														, aux->ipf.destinationMask
														, aux->ipf.sourcePort
														, aux->ipf.destinationPort);

	return STATUS_SUCCESS;
}

/*++

Routine Description:

    Add a word to the word list

Arguments:

     wf - pointer to word


Return Value:

    STATUS_SUCCESS if successful,
    STATUS_INSUFFICIENT_RESOURCES otherwise
 
--*/
NTSTATUS AddWordToList(WordFilter *wf)
{
	WordList *aux = NULL;
	// first, we reserve memory (non paged) to the new filter
	aux=(struct wordList *) ExAllocatePool(NonPagedPool, sizeof(struct wordList));

	if(aux == NULL)
	{
		dprintf("MyDriver.SYS: Problem reserving memory\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	//fill the new structure
	aux->wordf.id = wf->id;
	strcpy(aux->wordf.word, wf->word);
	
	//Add the new filter to the filter list
	if(firstWord == NULL)
	{
		firstWord = lastWord = aux;
		firstWord->next = NULL;
	}
	else
	{
		lastWord->next = aux;
		lastWord = aux;
		lastWord->next = NULL;
	}

	dprintf("MyDriver.SYS: Word Added\n\t%d %s\n", aux->wordf.id
									, aux->wordf.word);

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

	dprintf("MyDriver.SYS: Getting pointer to IpFilterDriver\n");

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
					dprintf("MyDriver.SYS: Error waiting for IpFilterDriver response.");
			}

			status = ioStatus.Status;

			if(!NT_SUCCESS(status))
				dprintf("MyDriver.SYS: Error, IO error with ipFilterDriver\n");
		}

		else
		{
			//if we cant allocate the space, we return the corresponding code error
			status = STATUS_INSUFFICIENT_RESOURCES;

			dprintf("MyDriver.SYS: Error building IpFilterDriver IRP\n");
		}

		if(ipFileObject != NULL)
			ObDereferenceObject(ipFileObject);

		ipFileObject = NULL;
		ipDeviceObject = NULL;
	}

	else
		dprintf("MyDriver.SYS: Error while getting the pointer\n");

	return status;
}

/*++

Routine Description:

    Monitor ports. Accumulate a usage of each port.
--*/

int PortMonitoring(IPPacket *ipp, unsigned char *Packet, unsigned int PacketLength)
{
	TCPHeader *tcph;
	USHORT port;
	PortList *aux;
	if(!setting.PortMonitor||ipp->ipProtocol != 6)
		return 0;
	tcph=(TCPHeader *)Packet;
	dprintf("MyDriver.SYS: %u %u %u %u %u\n", setting.IP, ipp->ipSource, ipp->ipDestination, 
		tcph->sourcePort, tcph->destinationPort );
	if(ipp->ipSource == setting.IP){
		dprintf("MyDriver.SYS: destination\n");
		port = tcph->sourcePort;}
	else if(ipp->ipDestination == setting.IP)
	{
		port = tcph->destinationPort;
		dprintf("MyDriver.SYS: destination\n");
	}
	else
	{
		dprintf("MyDriver.SYS: No matching\n");
		return 0;
	}
	aux = firstPort;
	while(aux != NULL)
	{
		if(aux->pusage.port == port)
		{
			aux->pusage.usage+=PacketLength;
			dprintf("MyDriver.SYS: Found it %d %d %d\n", port, aux->pusage.usage, PacketLength);
			break;
		}
		aux = aux->next;
	}
	return 0;
}

/*++

Routine Description:

    Filter the udp packet by word rules
--*/

PF_FORWARD_ACTION FilterByWords(IPPacket *ipp, unsigned char *Packet)
{
	struct wordList *aux = firstWord;
	if(!setting.WordFilter)
		return PF_FORWARD;
	if(ipp->ipProtocol != 17)
		return PF_FORWARD;
	while(aux != NULL)
	{
		if(strstr(Packet + sizeof(UDPHeader), aux->wordf.word) != NULL)
        {
            dprintf("MyDriver.SYS: Detected word\n");
            return PF_DROP;
        }
		aux=aux->next;
	}
	return PF_FORWARD;
}

/*++

Routine Description:

    Filter the packet by rules
--*/

PF_FORWARD_ACTION FilterByRules(IPPacket *ipp, unsigned char *Packet)
{
	TCPHeader *tcph;
	UDPHeader *udph;
	int countRule=0;
	struct filterList *aux = firstFilter;
	if(!setting.IPFilter)
		return PF_FORWARD;
	while(aux != NULL)
	{
		//dprintf("MyDriver.SYS: Comparing with Rule %d", countRule);

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
				dprintf("MyDriver.SYS: TCP: %d,%d\n",ntohs(tcph->sourcePort),ntohs(tcph->destinationPort));
				if(aux->ipf.sourcePort == 0 || tcph->sourcePort == aux->ipf.sourcePort)
				{ 
					if(aux->ipf.destinationPort == 0 || tcph->destinationPort == aux->ipf.destinationPort) //puerto tcp destino
					{
                        dprintf("MyDriver.SYS: TCP Packet Drop\n");
                        return  PF_DROP;
					}
				}
			}
				
			//udp, protocol = 17
			else if(ipp->ipProtocol == 17) 
			{
				udph=(UDPHeader *)Packet; 
				dprintf("MyDriver.SYS: UDP: %d,%d\n",ntohs(udph->sourcePort),ntohs(udph->destinationPort));

				if(aux->ipf.sourcePort == 0 || udph->sourcePort == aux->ipf.sourcePort) 
				{ 
					if(aux->ipf.destinationPort == 0 || udph->destinationPort == aux->ipf.destinationPort) 
					{
                        dprintf("MyDriver.SYS: UDP Packet Drop\n");
                        return  PF_DROP;
					}
				}
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

    Session control
--*/
PF_FORWARD_ACTION FilterBySession(IPPacket *ipp, unsigned char *Packet)
{
	if(!setting.SessionFilter)
		return PF_FORWARD;
	dprintf("MyDriver.SYS: %d\n", setting.Exceed);
	if( setting.SessionFilter && ipp->ipProtocol == 6)
	{
		TCPHeader *tcph=(TCPHeader *)Packet; 


		//if we havent the bit SYN activate, we pass the packets
		if((tcph->flags == 0x02)) 
		{
			dprintf("MyDriver.SYS: SYN FLAGS drop\n");
			if(setting.Exceed && setting.SessionFilter)
				return PF_DROP;
		}
	}
	return PF_FORWARD;
}

/*++

Routine Description:

    Filter each packet is received or sended

	To see parameters and return you can read it in MSDN
--*/

PF_FORWARD_ACTION cbFilterFunction(IN unsigned char *PacketHeader,IN unsigned char *Packet, IN unsigned int PacketLength, IN unsigned int RecvInterfaceIndex, IN unsigned int SendInterfaceIndex, IN unsigned long RecvLinkNextHop, IN unsigned long SendLinkNextHop)
{
	IPPacket *ipp;
	PF_FORWARD_ACTION rz;
	
	ipp=(IPPacket *)PacketHeader;

	rz = FilterByRules(ipp, Packet);
	if(rz == PF_FORWARD)
		rz = FilterBySession(ipp, Packet);
	if(rz == PF_FORWARD)
		rz = FilterByWords(ipp, Packet);
	if(rz == PF_FORWARD)
		PortMonitoring(ipp, Packet, PacketLength);
	if(rz == PF_FORWARD)
		PacketLengthsum +=PacketLength;
	return rz;
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
	dprintf("MyDriver.SYS: Removing the filter List...");
	
	while(firstFilter != NULL)
	{
		aux = firstFilter;
		firstFilter = firstFilter->next;
		ExFreePool(aux);

		dprintf("MyDriver.SYS: One Rule removed");
	}

	firstFilter = lastFilter = NULL;

	dprintf("MyDriver.SYS: Removed is complete.");
}

/*++

Routine Description:

    Remove the linked list where the words were saved.

Arguments:


Return Value:

 
--*/
void ClearWordList(void)
{
	struct wordList *aux = NULL;

	//free the linked list
	dprintf("MyDriver.SYS: Removing the word List...");
	
	while(firstWord != NULL)
	{
		aux = firstWord;
		firstWord = firstWord->next;
		ExFreePool(aux);

		dprintf("MyDriver.SYS: One Word removed");
	}

	firstWord = lastWord = NULL;

	dprintf("MyDriver.SYS: Removed is complete.");
}

/*++

Routine Description:

    Find PortList element by port number or make new element if no there.

Arguments:
	Portnumber which we want to find

Return Value:
	Return pointer to PortList
 
--*/

PortList *FindPort(USHORT port)
{
	struct portList *aux = firstPort;

	//free the linked list
	//dprintf("MyDriver.SYS: Finding the port List...");
	
	while(aux != NULL)
	{
		if(aux->pusage.port == port)
		{
			//dprintf("MyDriver.SYS: Found it");
			break;
		}
		aux = aux->next;
	}
	if(aux == NULL)
	{
		aux=(struct portList *) ExAllocatePool(NonPagedPool, sizeof(struct portList));

		if(aux == NULL)
		{
			dprintf("MyDriver.SYS: Problem reserving memory\n");
			return NULL;
		}

		aux->pusage.usage = 0;
		aux->pusage.port = port;

		
		//Add the new filter to the filter list
		if(firstPort == NULL)
		{
			firstPort = lastPort = aux;
			firstPort->next = NULL;
		}
		else
		{
			lastPort->next = aux;
			lastPort = aux;
			lastPort->next = NULL;
		}

		dprintf("MyDriver.SYS: Port Added\t%d\n", aux->pusage.port);
	}
	return aux;
}

/*++

Routine Description:

    Remove the linked list where the ports were saved.

Arguments:


Return Value:

 
--*/
void ClearPortList(void)
{
	struct portList *aux = NULL;

	//free the linked list
	dprintf("MyDriver.SYS: Removing the port List...");
	
	while(firstPort != NULL)
	{
		aux = firstPort;
		firstPort = firstPort->next;
		ExFreePool(aux);

		dprintf("MyDriver.SYS: One port removed");
	}

	firstPort = lastPort = NULL;

	dprintf("MyDriver.SYS: Removed is complete.");
}

/*++

Routine Description:

    Remove the rule by id

Arguments:
	rule id

Return Value:

 
--*/
void DeleteRule(USHORT id)
{
	struct filterList *aux = NULL;
	struct filterList *target = NULL;

	if(firstFilter == NULL || lastFilter == NULL)
		return;

	if(firstFilter->ipf.id == id)
	{
		target = firstFilter;
		if(lastFilter->ipf.id == id)
			firstFilter = lastFilter = NULL;
		else
			firstFilter = firstFilter->next;
	}
	else
	{
		aux = firstFilter;
		while(aux->next != NULL)
		{
			if(aux->next->ipf.id == id)
			{
				target = aux->next;
				aux->next = aux->next->next;
				if(target == lastFilter)
				{
					lastFilter = aux;
				}
				break;
			}
			aux = aux->next;
		}
	}
	ExFreePool(target);
}

/*++

Routine Description:

    Remove the word by id

Arguments:
	word id

Return Value:

 
--*/
void DeleteWord(USHORT id)
{
	struct wordList *aux = NULL;
	struct wordList *target = NULL;

	if(firstWord == NULL || lastWord == NULL)
		return;

	if(firstWord->wordf.id == id)
	{
		target = firstWord;
		if(lastWord->wordf.id == id)
			firstWord = lastWord = NULL;
		else
			firstWord = firstWord->next;
	}
	else
	{
		aux = firstWord;
		while(aux->next != NULL)
		{
			if(aux->next->wordf.id == id)
			{
				target = aux->next;
				aux->next = aux->next->next;
				if(target == lastWord)
				{
					lastWord = aux;
				}
				break;
			}
			aux = aux->next;
		}
	}
	ExFreePool(target);
}

/*++

Routine Description:

    Remove the port by portnumber

Arguments:
	Port number

Return Value:

 
--*/
void DeletePort(USHORT port)
{
	struct portList *aux = NULL;
	struct portList *target = NULL;

	//free the linked list
	//dprintf("MyDriver.SYS: Removing the port List...");
	
	if(firstPort == NULL || lastPort == NULL)
		return;

	if(firstPort->pusage.port == port)
	{
		target = firstPort;
		if(lastPort->pusage.port == port)
			firstPort = lastPort = NULL;
		else
			firstPort = firstPort->next;
	}
	else
	{
		aux = firstPort;
		while(aux->next != NULL)
		{
			if(aux->next->pusage.port == port)
			{
				target = aux->next;
				aux->next = aux->next->next;
				if(target == lastPort)
				{
					lastPort = aux;
				}
				break;
			}
			aux = aux->next;
		}
	}
	ExFreePool(target);
}