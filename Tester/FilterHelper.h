//ERROR CODES
#pragma once

#define DRV_SUCCESS						 (DWORD)0		//ALL OK

#define DRV_ERROR_SCM					 (DWORD)-1		//ERROR at Open de Service Manager
#define DRV_ERROR_SERVICE				 (DWORD)-2		//ERROR at create service
#define DRV_ERROR_MEMORY				 (DWORD)-3		//ERROR	at reserving memory
#define DRV_ERROR_INVALID_PATH_OR_FILE	 (DWORD)-4		//ERROR, the path gived is not valid
#define DRV_ERROR_INVALID_HANDLE		 (DWORD)-5		//ERROR, driver handle is not valid
#define DRV_ERROR_STARTING				 (DWORD)-6		//ERROR at starting the driver
#define DRV_ERROR_STOPPING				 (DWORD)-7		//ERROR at stopping the driver
#define DRV_ERROR_REMOVING				 (DWORD)-8		//ERROR at removing the driver "service"
#define DRV_ERROR_IO					 (DWORD)-9		//ERROR at io operation
#define DRV_ERROR_NO_INITIALIZED		 (DWORD)-10		//ERROR, class not initialized
#define DRV_ERROR_ALREADY_INITIALIZED	 (DWORD)-11		//ERROR, class already initialized
#define DRV_ERROR_NULL_POINTER			 (DWORD)-12		//ERROR, pointer introduced is NULL
#define DRV_ERROR_UNKNOWN				 (DWORD)-13		//UNKNOWN ERROR

class FilterHelper
{
public:
	FilterHelper(void);		//constructor
	~FilterHelper(void);		//destructor

	//functions to initialized the driver variables
	DWORD InitDriver(LPCTSTR name, LPCTSTR path, LPCTSTR dosName=NULL);
	DWORD InitDriver(LPCTSTR path);

	//functions to load and unload drivers. If start = TRUE, the driver will be started.
	DWORD LoadDriver(BOOL start = TRUE);
	DWORD LoadDriver(LPCTSTR name, LPCTSTR path, LPCTSTR dosName=NULL, BOOL start=TRUE);
	DWORD LoadDriver(LPCTSTR path, BOOL start=TRUE);

	//if forceClearData == TRUE, will remove variables although we cant remove driver "service"
	DWORD UnloadDriver(BOOL forceClearData = FALSE);
	
	//functions to start and stop, driver "service"
	DWORD StartDriver(void);
	DWORD StopDriver(void);

	//if true, the driver havent been removed at finish
	void SetRemovable(BOOL value);

	//funtion to return class status
	BOOL IsInitialized();
	BOOL IsStarted();
	BOOL IsLoaded();

	//function to get driver handle
	HANDLE GetDriverHandle(void);

	//funtions to make IO operation with driver
	DWORD WriteIo(DWORD code, PVOID buffer, DWORD count);
	DWORD ReadIo(DWORD code, PVOID buffer, DWORD count);
	DWORD RawIo(DWORD code, PVOID inBuffer, DWORD inCount, PVOID outBuffer, DWORD outCount);
	
private:
	HANDLE driverHandle;	//driver handle
	
	LPTSTR driverName;		//driver name
	LPTSTR driverPath;		//driver disk path
	LPTSTR driverDosName;	//driver's dos name, to link with it

	BOOL initialized;	//variables to store the status of this class
	BOOL started;
	BOOL loaded;
	BOOL removable;

	//get a handle to the driver
	DWORD OpenDevice(void);	
};