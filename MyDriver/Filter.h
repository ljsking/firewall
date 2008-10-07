#pragma once
#define FILE_DEVICE_DRVFLTIP  0x00654322
#define DRVFLTIP_IOCTL_INDEX  0x830

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe
#define METHOD_BUFFERED                 0

#define START_IP_HOOK CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX,METHOD_BUFFERED, FILE_ANY_ACCESS)
#define STOP_IP_HOOK CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ADD_FILTER CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CLEAR_FILTER CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define ADD_WORD CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define SET_SETTING CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GET_SETTING CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+6, METHOD_BUFFERED, FILE_WRITE_ACCESS|FILE_READ_ACCESS)
#define GET_PORTUSAGE CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+7, METHOD_BUFFERED, FILE_WRITE_ACCESS|FILE_READ_ACCESS)
#define DEL_PORT CTL_CODE(FILE_DEVICE_DRVFLTIP, DRVFLTIP_IOCTL_INDEX+8, METHOD_BUFFERED, FILE_READ_ACCESS)

//struct to define filter rules
typedef struct firewallSetting
{
	ULONG IP;
	int IPFilter;
	int WordFilter;
	int PortMonitor;
	int SessionMonitor;
	int MaxSession;
	int NowSession;
}FirewallSetting;

//struct to define filter rules
typedef struct filter
{
	ULONG id;
	USHORT protocol;		//protocol used

	ULONG sourceIp;			//source ip address
	ULONG destinationIp;	//destination ip address

	ULONG sourceMask;		//source mask
	ULONG destinationMask;	//destination mask

	USHORT sourcePort;		//source port
	USHORT destinationPort; //destination port
}IPFilter;

typedef struct wordFilter
{
	ULONG id;
	char word[10];
}WordFilter;



//struct to build a linked list 
typedef struct filterList
{
	IPFilter ipf;
	struct filterList *next;
}FilterList;

//struct to build a linked list 
typedef struct wordList
{
	WordFilter wordf;
	struct wordList *next;
}WordList;

typedef struct portUsage
{
	USHORT	port;
	ULONG	usage;
}PortUsage;

typedef struct portList
{
	PortUsage pusage;
	struct portList *next;
}PortList;

//Ip Header
typedef struct IPHeader 
{
    UCHAR     iphVerLen;      // Version and length 
    UCHAR     ipTOS;          // Type of service 
    USHORT    ipLength;       // Total datagram length 
    USHORT    ipID;		      // Identification 
    USHORT    ipFlags;	      // Flags
    UCHAR     ipTTL;	      // Time to live 
    UCHAR     ipProtocol;	  // Protocol 
    USHORT    ipChecksum;     // Header checksum 
    ULONG     ipSource;       // Source address 
    ULONG     ipDestination;  // Destination address 
} IPPacket; 


//TCP Header
typedef struct _TCPHeader
{
	USHORT			sourcePort;			// Source Port
	USHORT			destinationPort;	// Destination Port
	ULONG			sequenceNumber;		// Number of Sequence
	ULONG			acknowledgeNumber;	// Number of aknowledge
	UCHAR			dataoffset;			// Pointer to data
	UCHAR			flags;				// Flags
	USHORT			windows;			// Size of window
	USHORT			checksum;			// Total checksum
	USHORT			urgentPointer;		// Urgent pointer
} TCPHeader;


//UDP Header
typedef struct _UDPHeader
{
	USHORT			sourcePort;			// Source Port
	USHORT			destinationPort;	// Destination Port
	USHORT			len;				// Total length
	USHORT			checksum;			// Total checksum
} UDPHeader;
