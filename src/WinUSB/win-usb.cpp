//==============================================================================
//File name:   "bt-.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:   (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dbt.h>
#include <algorithm>
#include <SetupAPI.h>
#include <Hidsdi.h> 
#include <devguid.h>
#include <usb.h>
#include <setupapi.h>
#include <iomanip>
//----
#include "parser.h"
#include "win-usb.h"
#include "bt-dongle.h"

#pragma comment(lib, "setupapi.lib")
//#pragma comment(lib, "hid.lib")
//#pragma comment(lib, "winusb.lib")

//---- Mem Leakage Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//----
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DEFINE_GUID(GUID_DEVCLASS_WINUSB, 0x88bae032L, 0x5a81, 0x49f0, 0xbc, 0x3d, 0xa4, 0xff, 0x13, 0x82, 0x16, 0xd6);
DEFINE_GUID(GUID_DEVCLASS_STL, 0xf177724dL, 0x74d3, 0x430e, 0x86, 0xb5, 0xf0, 0x36, 0x89, 0x10, 0xeb, 0x23);
static GUID GUID_DEVINTERFACE_USB_DEVICE = {0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

//---- If need Debug
#define  WUSB_DEBUG

//------------------------------------------------------------------------------ 
//Constructor 
//------------------------------------------------------------------------------
CWINUSB::CWINUSB()
{ 
  hDevice = NULL;
  hUSB = NULL;  

  //----
  lib_opened = 0;
  dev_opened = 0;
  dev_vid = 0;
  dev_pid = 0;
  dev_speed = 0;
  dev_serial = 0;
  usb_interface = 0;
  usb_configuration = 1;
 
  //----
  VIDs.empty();
  PIDs.empty();
  IDs.empty();
  Manufacturers.empty();
  Names.empty();
  DevPatchs.empty();
  
  //----
  memset(&gdWINUSB, 0, sizeof(gdWINUSB));
  //memset(&descr_conf, 0, sizeof(descr_conf));
}

//------------------------------------------------------------------------------
//Destructor 
//------------------------------------------------------------------------------
CWINUSB::~CWINUSB()
{
  //Close();
}

//------------------------------------------------------------------------------
//Find & Return USB device list 
//------------------------------------------------------------------------------
int CWINUSB::Open(void)
{  
  #ifdef WUSB_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  int result;
  gdWINUSB = GUID_DEVCLASS_WINUSB;
  
  //найти все USB устройства 
  result = GetDeviceList();
  
  if(result>=0) lib_opened = 1;
  else lib_opened = 0;

  //выйти
  return result;
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
void CWINUSB::Close(void)
{
  #ifdef WUSB_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
   
  //if Device connected - disconnect
  if(dev_opened) 
  { 
    CloseHandle(hDevice);
    hDevice = NULL;
    dev_opened = 0; 
  }

  if(lib_opened) 
  {
    WinUsb_Free(hUSB);
    hUSB = NULL;
    lib_opened = 0;
  }
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int mainv()
{/*
  DWORD n;
  DWORD   numEvents;
  HANDLE rHnd;    

  WinusbHandle_Open = 0;
  DeviceHandle_Open = 0;

  winusb_guid = GUID_DEVCLASS_WINUSB;
///  stl_guid = GUID_DEVCLASS_STL; 
  //----
  usb_out_buffer[0] = 0;
  usb_out_buffer[1] = 1;
  usb_out_buffer[2] = 2;
  usb_out_buffer[3] = 3;

  ULONG bytesWritten;
  ULONG timeout;
  timeout = 100;
  rHnd = GetStdHandle(STD_INPUT_HANDLE);

  WinUsb_SetPipePolicy(WinusbHandle, EP1_OUT, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);

  timeout = TRUE;
  WinUsb_SetPipePolicy(WinusbHandle, EP1_OUT, AUTO_CLEAR_STALL, sizeof(ULONG), &timeout);


  timeout = TRUE;
  //Bypasses queuing and error handling to boost performance for multiple read requests.							
  WinUsb_SetPipePolicy(WinusbHandle, EP1_OUT, RAW_IO, sizeof(ULONG), &timeout);


 while(true)
 {
  if(!WinusbHandle_Open) 
  { 
    if(!connectusb()) 
        Sleep(2000);
  }

  bytesWritten = 0;
  
  SendDatatoDefaultEndpoint(WinusbHandle);     */
  /*
  int ret = WinUsb_WritePipe(WinusbHandle, EP1_OUT, &usb_out_buffer[0], 2, &bytesWritten, NULL);
  if(!ret)
  {
    TRACE1("WinUsb_WritePipe: %u\n", GetLastError()); 
    disconnectusb();
  }	*/ /*
  Sleep(2000);
 }
      */
 return 0;
}

typedef struct _DEVICE_DATA {

    BOOL                    HandlesOpen;
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle;
    TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, *PDEVICE_DATA;

struct PIPE_ID
{
  UCHAR  PipeInId;
  UCHAR  PipeOutId;
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT RetrieveDevicePath(
    _Out_bytecap_(BufLen) LPTSTR DevicePath,
    _In_                  ULONG  BufLen,
    _Out_opt_             PBOOL  FailureDeviceNotFound
    )
/*++

Routine description:

    Retrieve the device path that can be used to open the WinUSB-based device.

    If multiple devices have the same device interface GUID, there is no
    guarantee of which one will be returned.

Arguments:

    DevicePath - On successful return, the path of the device (use with CreateFile).

    BufLen - The size of DevicePath's buffer, in bytes

    FailureDeviceNotFound - TRUE when failure is returned due to no devices
        found with the correct device interface (device not connected, driver
        not installed, or device is disabled in Device Manager); FALSE
        otherwise.

Return value:

    HRESULT

--*/
{
    BOOL                             bResult = FALSE;
    HDEVINFO                         deviceInfo;
    SP_DEVICE_INTERFACE_DATA         interfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
    ULONG                            length;
    ULONG                            requiredLength=0;
    HRESULT                          hr;

    if (NULL != FailureDeviceNotFound) {

        *FailureDeviceNotFound = FALSE;
    }
            /*
    //
    // Enumerate all devices exposing the interface
    //
    deviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USBApplication1,
                                     NULL,
                                     NULL,
                                     DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (deviceInfo == INVALID_HANDLE_VALUE) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    //
    // Get the first interface (index 0) in the result set
    //
    bResult = SetupDiEnumDeviceInterfaces(deviceInfo,
                                          NULL,
                                          &GUID_DEVINTERFACE_USBApplication1,
                                          0,
                                          &interfaceData);

    if (FALSE == bResult) {

        //
        // We would see this error if no devices were found
        //
        if (ERROR_NO_MORE_ITEMS == GetLastError() &&
            NULL != FailureDeviceNotFound) {

            *FailureDeviceNotFound = TRUE;
        }

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Get the size of the path string
    // We expect to get a failure with insufficient buffer
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                              &interfaceData,
                                              NULL,
                                              0,
                                              &requiredLength,
                                              NULL);

    if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Allocate temporary space for SetupDi structure
    //
    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
        LocalAlloc(LMEM_FIXED, requiredLength);

    if (NULL == detailData)
    {
        hr = E_OUTOFMEMORY;
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    length = requiredLength;

    //
    // Get the interface's path string
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                              &interfaceData,
                                              detailData,
                                              length,
                                              &requiredLength,
                                              NULL);

    if(FALSE == bResult)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        LocalFree(detailData);
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Give path to the caller. SetupDiGetDeviceInterfaceDetail ensured
    // DevicePath is NULL-terminated.
    //
    hr = StringCbCopy(DevicePath,
                      BufLen,
                      detailData->DevicePath);

    LocalFree(detailData);
    SetupDiDestroyDeviceInfoList(deviceInfo);
        */
    return hr;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
HRESULT OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
    )
/*++

Routine description:

    Open all needed handles to interact with the device.

    If the device has multiple USB interfaces, this function grants access to
    only the first interface.

    If multiple devices have the same device interface GUID, there is no
    guarantee of which one will be returned.

Arguments:

    DeviceData - Struct filled in by this function. The caller should use the
        WinusbHandle to interact with the device, and must pass the struct to
        CloseDevice when finished.

    FailureDeviceNotFound - TRUE when failure is returned due to no devices
        found with the correct device interface (device not connected, driver
        not installed, or device is disabled in Device Manager); FALSE
        otherwise.

Return value:

    HRESULT

--*/
{
    HRESULT hr = S_OK;
    int    bResult;

    DeviceData->HandlesOpen = FALSE;

    hr = RetrieveDevicePath(DeviceData->DevicePath,
                            sizeof(DeviceData->DevicePath),
                            FailureDeviceNotFound);

    if (FAILED(hr)) {

        return hr;
    }

    DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
                                          GENERIC_WRITE | GENERIC_READ,
                                          FILE_SHARE_WRITE | FILE_SHARE_READ,
                                          NULL,
                                          OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                          NULL);

    if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    bResult = WinUsb_Initialize(DeviceData->DeviceHandle,
                                &DeviceData->WinusbHandle);

    if (FALSE == bResult) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        CloseHandle(DeviceData->DeviceHandle);
        return hr;
    }

    DeviceData->HandlesOpen = TRUE;


	///WinUsb_DeviceReset()
    return hr;
}

///https://stackoverflow.com/questions/987958/how-do-i-reset-usb-devices-using-the-windows-api
/*
// TODO: (post hotplug): see if we can force eject the device and redetect it (reuse hotplug?)
static int winusbx_reset_device(int sub_api, struct libusb_device_handle *dev_handle)
{
	struct winusb_device_handle_priv *handle_priv = usbi_get_device_handle_priv(dev_handle);
	struct winusb_device_priv *priv = usbi_get_device_priv(dev_handle->dev);
	HANDLE winusb_handle;
	int i, j;

	CHECK_WINUSBX_AVAILABLE(sub_api);

	// Reset any available pipe (except control)
	for (i = 0; i < USB_MAXINTERFACES; i++) {
		winusb_handle = handle_priv->interface_handle[i].api_handle;
		if (HANDLE_VALID(winusb_handle)) {
			for (j = 0; j < priv->usb_interface[i].nb_endpoints; j++) {
				usbi_dbg("resetting ep %02X", priv->usb_interface[i].endpoint[j]);
				if (!WinUSBX[sub_api].AbortPipe(winusb_handle, priv->usb_interface[i].endpoint[j]))
					usbi_err(HANDLE_CTX(dev_handle), "AbortPipe (pipe address %02X) failed: %s",
						priv->usb_interface[i].endpoint[j], windows_error_str(0));

				// FlushPipe seems to fail on OUT pipes
				if (IS_EPIN(priv->usb_interface[i].endpoint[j])
						&& (!WinUSBX[sub_api].FlushPipe(winusb_handle, priv->usb_interface[i].endpoint[j])))
					usbi_err(HANDLE_CTX(dev_handle), "FlushPipe (pipe address %02X) failed: %s",
						priv->usb_interface[i].endpoint[j], windows_error_str(0));

				if (!WinUSBX[sub_api].ResetPipe(winusb_handle, priv->usb_interface[i].endpoint[j]))
					usbi_err(HANDLE_CTX(dev_handle), "ResetPipe (pipe address %02X) failed: %s",
						priv->usb_interface[i].endpoint[j], windows_error_str(0));
			}
		}
	}

	// libusbK & libusb0 have the ability to issue an actual device reset
	if ((sub_api != SUB_API_WINUSB) && (WinUSBX[sub_api].ResetDevice != NULL)) {
		winusb_handle = handle_priv->interface_handle[0].api_handle;
		if (HANDLE_VALID(winusb_handle))
			WinUSBX[sub_api].ResetDevice(winusb_handle);
	}

	return LIBUSB_SUCCESS;
}
*/

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::OpenDevice(int index)
{
  int result;
  char deviceSpeed;
  std::string dev_patch;
  HANDLE hWDev = NULL;
  WINUSB_INTERFACE_HANDLE hWUSB = NULL;

  int vid = VIDs.at(index);
  int pid = PIDs.at(index); 
  long long serial = 0;
  
  ///connectusb();

  //----
  result = GetDevicePath(vid, pid, serial, &dev_patch);
  if(!result) return -1;

  //----
  result = GetDeviceHandle(&dev_patch, hWDev);
  if(!result) return -1;
  
  //---- 
  result = GetWinUSBHandle(hWDev, hWUSB);
  if(!result) return -1;
    
  USB_DEVICE_DESCRIPTOR deviceDesc;
  ULONG                 lengthReceived;
  //Get device descriptor
  result = WinUsb_GetDescriptor(hWUSB,
		   USB_DEVICE_DESCRIPTOR_TYPE,
		   0,
		   0,
		  (PBYTE)&deviceDesc,
		   sizeof(deviceDesc),
		   &lengthReceived);
  if(!result) return -1;

  //----
  result = GetUSBDeviceSpeed(hWUSB, &deviceSpeed);
  if(!result) return -1;

  //----
  result = QueryDeviceEndpoints(hWUSB);//(HANDLE)&PipeID
  if(!result) return -1;
  /*
  //----
  unsigned long timeout = TRUE;
  result = WinUsb_SetPipePolicy(hWUSB, EP1_OUT, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);

  //----
  timeout = TRUE;
  result = WinUsb_SetPipePolicy(hWUSB, EP1_OUT, AUTO_CLEAR_STALL, sizeof(ULONG), &timeout);

  //----
  timeout = TRUE;
  //Bypasses queuing and error handling to boost performance for multiple read requests.							
  result = WinUsb_SetPipePolicy(hWUSB, EP1_OUT, RAW_IO, sizeof(ULONG), &timeout);
*/
  /*
  struct libusb_device_handle *heUSB;
  heUSB = (libusb_device_handle *)0x2367;
  libusb_reset_device(heUSB);
  */


  //----  
  result = WinUsb_AbortPipe(hWUSB, EP1_IN);
  result = WinUsb_AbortPipe(hWUSB, EP2_IN);

  //----  
  result = WinUsb_FlushPipe(hWUSB, EP1_IN);
  result = WinUsb_FlushPipe(hWUSB, EP2_IN);

  //----  
  result = WinUsb_ResetPipe(hWUSB, EP1_IN);
  result = WinUsb_ResetPipe(hWUSB, EP2_IN);
  
  Sleep(200);

  hUSB = hWUSB; 
  hDevice = hWDev;
  
  /* 
  //---- TEST
  char buf[] = {0x03, 0x0c, 0x00};  
  result = ControlSend(hWUSB, buf, 3);
  if(!result) return -1;
  
  //----
  result = ControlRead(hWUSB, (char*)in_buffer, 6);
  if(!result) return -1;				  
 
  //----
  result = BulkSend(hWUSB, EP2_OUT, (char*)out_buffer, 4);
  if(!result) return -1;
  
  //----
  result = BulkRead(hWUSB, EP2_IN, (char*)in_buffer, 4);
  if(!result) return -1;
   */

  return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::CloseDevice(void)
{
  //if Device connected - disconnect
  if(dev_opened) 
  { 
    CloseHandle(hDevice);
    hDevice = NULL;
    dev_opened = 0; 
  }

  return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
void CWINUSB::FlushFifo(void)
{
  if(hUSB==NULL) return;
  
  //----  
  WinUsb_FlushPipe(hUSB, EP1_IN);
  WinUsb_FlushPipe(hUSB, EP2_IN);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hWUSB)
{
  if(hDevice==INVALID_HANDLE_VALUE)
  {
    return -1;
  }

  PIPE_ID pipeid;
  BOOL result = TRUE;

  USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
  ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

  WINUSB_PIPE_INFORMATION  Pipe;
  ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

  //---- 
  result = WinUsb_QueryInterfaceSettings(hWUSB, 0, &InterfaceDescriptor);

  if(result)
  {
    for(int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
    {
      result = WinUsb_QueryPipe(hWUSB, 0, index, &Pipe);

      if(result)
      {
        if(Pipe.PipeType == UsbdPipeTypeControl)
        {
          TRACE3("Endpoint index: %d Pipe type: %d Control Pipe ID: %X\n", 
			      index, Pipe.PipeType, Pipe.PipeId);
        }
                
		if(Pipe.PipeType == UsbdPipeTypeIsochronous)
        {
          TRACE3("Endpoint index: %d Pipe type: %d Isochronous Pipe ID: %X\n", 
			      index, Pipe.PipeType, Pipe.PipeId);
        }
                
		if(Pipe.PipeType == UsbdPipeTypeBulk)
        {
          if(USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
          {
            TRACE3("Endpoint index: %d Pipe type: %d Bulk Pipe ID: %X\n", 
				    index, Pipe.PipeType, Pipe.PipeId);
            pipeid.PipeInId = Pipe.PipeId;
          }
                    
		  if(USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
          {
            TRACE3("Endpoint index: %d Pipe type: %d Bulk Pipe ID: %X\n", 
				    index, Pipe.PipeType, Pipe.PipeId);
            pipeid.PipeOutId = Pipe.PipeId;
          }
        }
                
		if(Pipe.PipeType == UsbdPipeTypeInterrupt)
        {
          TRACE3("Endpoint index: %d Pipe type: %d Interrupt Pipe ID: %X\n", 
			      index, Pipe.PipeType, Pipe.PipeId);
        }
       }
      else
      {
        continue;
      }
     }
  }

  return result;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::GetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hDevice, char *pDevSpeed)
{
    
  if(!pDevSpeed || hDevice==INVALID_HANDLE_VALUE)
  {
    return -1;
  }

  int result;
  ULONG length = sizeof(UCHAR);

  result = WinUsb_QueryDeviceInformation(hDevice, DEVICE_SPEED, 
                                         &length, pDevSpeed);
  if(!result)
  {
    TRACE1("Error getting device speed: %d.\n", GetLastError());
    goto done;
  }

  if(*pDevSpeed == LowSpeed)
  {
    TRACE1("Device speed: %d (Low speed).\n", *pDevSpeed);
    goto done;
  }
    
  if(*pDevSpeed == FullSpeed)
  {
    TRACE1("Device speed: %d (Full speed).\n", *pDevSpeed);
    goto done;
  }
    
  if(*pDevSpeed == HighSpeed)
  {
    TRACE1("Device speed: %d (High speed).\n", *pDevSpeed);
    goto done;
  }

  done:
  return result;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::ControlSend(WINUSB_INTERFACE_HANDLE hWUSB, char *pBuf, int length)
{
  //----
  if(hWUSB==INVALID_HANDLE_VALUE)
  {
    return -1;
  }
  
  unsigned long sent = 0;
  int result;
  
  //----
  WINUSB_SETUP_PACKET SetupPacket;
  ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
 

  //Create the setup packet
  SetupPacket.RequestType = 0x20;
  SetupPacket.Request = 0; 
  SetupPacket.Value = 0;
  SetupPacket.Index = 0;
  SetupPacket.Length = length;

  result = WinUsb_ControlTransfer(hWUSB, SetupPacket, 
                                 (unsigned char*)pBuf, 
                                  length, &sent, 0);
  if(result)
  {
    ///TRACE2("Data sent: %d, transferred: %d.\n", length, sent); 
  }

  return result;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::ControlRead(WINUSB_INTERFACE_HANDLE hWUSB, char *pBuf, 
	                     int buf_length, int &rx, int timeout)
{
  if(hDevice==INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  int result;
  unsigned long read;
  unsigned char *pBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*buf_length);
   
  //PIPE_TRANSFER_TIMEOUT is the only one which is effective for the no-data endpoints.
  //Did you pass ULONG variable to WinUsb_SetPipePolicy, as follows?

  ULONG time_out = timeout; // ms
  WinUsb_SetPipePolicy(hWUSB, EP1_IN, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &time_out);

  result = WinUsb_ReadPipe(hWUSB, EP1_IN, pBuffer, buf_length, &read, 0);
  if(result>0)
  {
    ///TRACE3("Read from EP %X: %s, data read: %d\n", EP1_IN, pBuffer, read);
    ///TRACE("RX > "); print_buffer((char*)pBuffer, 0, read);
	memcpy(pBuf, pBuffer, read);
	rx = (int) read; 
  }

  LocalFree(pBuffer);
  return result;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::BulkSend(WINUSB_INTERFACE_HANDLE hWUSB,
                      unsigned char EP, char *pBuf, int length)
{
  //----
  if(hDevice==INVALID_HANDLE_VALUE || pBuf==NULL || length==0)
  {
    return -1;
  }

  int result;
  unsigned long sent;
  //----
  result = WinUsb_WritePipe(hWUSB, EP, (unsigned char*)pBuf, 
                            length, &sent, 0);
  if(result>0)
  {
    //TRACE3("Send to EP %X: %d, data transferred: %d\n", EP, length, sent);
	return (int) sent;
  }
  
  return -1; 
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::BulkRead(WINUSB_INTERFACE_HANDLE hWUSB,
                      unsigned char EP, char *pBuf, int length, int &rx)
{
  if(hDevice==INVALID_HANDLE_VALUE)
  {
    return -1;
  }

  int result;
  unsigned long read;
  unsigned char *pBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*length);    
  //----
  result = WinUsb_ReadPipe(hWUSB, EP, pBuffer, length, &read, 0);
  if(result>0)
  {
    TRACE2("Read from EP: %X,  data read: %d\n", EP, read);
    TRACE("RX > "); print_buffer((char*)pBuffer, 0, read);
	memcpy(pBuf, pBuffer, read);
	rx = (int)read; 
  }
    
  LocalFree(pBuffer);
  return result;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::GetDeviceList(void)
{
  #ifdef WUSB_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  char data_buffer[2048];
  int i = 0, ret, found = 0;

  //----
  SP_DEVINFO_DATA                  deviceData;
  SP_DEVICE_INTERFACE_DATA         interfaceData;
  PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
  
  //Get all connected USB devices  
  //GUID_DEVINTERFACE_USB_DEVICE  GUID_DEVCLASS_WINUSB   GUID_DEVCLASS_USB
  //36FC9E60 - C465 - 11CF - 8056 - 444553540000
  //ClassGuid = {88BAE032-5A81-49f0-BC3D-A4FF138216D6}
  
  HDEVINFO infoSet;
  infoSet = SetupDiGetClassDevs( NULL, NULL, NULL,  //&gdWINUSB
	                            DIGCF_DEVICEINTERFACE | 
	                            DIGCF_PRESENT | DIGCF_ALLCLASSES);
  if(infoSet == INVALID_HANDLE_VALUE) 
  { 
    
    #ifdef WUSB_DEBUG
    TRACE1("%s Error: No USB devices list\n", __FUNCTION__);
    #endif	  

	  AfxMessageBox(_T("Error to do: SetupDiGetClassDevs"));
	//No devices ?
	return -1; 
  }

  //----------------------------------------------------------
  //
  //----------------------------------------------------------
  for(;;)
  {
    //----
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    
	// GUID_DEVINTERFACE_USB_DEVICE  GUID_DEVCLASS_WINUSB   GUID_DEVCLASS_USB
    ret = SetupDiEnumDeviceInterfaces(infoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &interfaceData);
    if(ret!=1)
    {
       TRACE1("SetupDiEnumDeviceInterfaces: %u\n", GetLastError()); 
	     //AfxMessageBox(_T("Error to do: SetupDiEnumDeviceInterfaces"));
       break;
    }

    //----
    memset(data_buffer, 0, sizeof(data_buffer)); 
    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)data_buffer;
    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    deviceData.cbSize = sizeof(deviceData);
          
    if(!SetupDiGetDeviceInterfaceDetail(infoSet, &interfaceData, detailData, 
                                        sizeof(data_buffer), NULL, &deviceData)) 
    { 
      TRACE1("SetupDiGetDeviceInterfaceDetail: %u\n", GetLastError()); 
	     AfxMessageBox(_T("Error to do: SetupDiGetDeviceInterfaceDetail"));
      break; 
    }
    
    std::string dev_info(detailData->DevicePath);
    std::transform(dev_info.begin(), dev_info.end(), dev_info.begin(), tolower);

    //VID_0A5C&PID_21E8
    int vid_pos = (int)dev_info.find("vid_");
    int pid_pos = (int)dev_info.find("pid_");
    
    //---- ищем номер
    //"\\?\usb#vid_0a5c&pid_21e8#5cf3706479ca#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"
    //int pos_num = (int)std::find(devid.begin(), devid.end(), "#");
        
    if(vid_pos>=0 && pid_pos>=0)
    {
      //сохранить путь к устройству  
      DevPatchs.push_back(detailData->DevicePath);
	  
	  //---- VID PID
	  std::string str_vid(dev_info.substr(vid_pos+4));
	  str_vid.erase(4);
      std::string str_pid(dev_info.substr(pid_pos+4));
	  str_pid.erase(4);

	  //----
	  unsigned short dig;
	  dig = (unsigned short)std::stoi(str_vid,0,16);
	  VIDs.push_back(dig);
	  dig = (unsigned short)std::stoi(str_pid,0,16);
	  PIDs.push_back(dig);
	  
	  //---- это будет заполнено потом или дополнительными функциями
	  IDs.push_back(0);
      Manufacturers.push_back(" ");
	  Names.push_back(" ");
      
	  found++;
    }

    i++;
  }

  //----
  SetupDiDestroyDeviceInfoList(infoSet);

  return found;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
template <typename I> std::string int_to_hex(I w, int hex_len = sizeof(I)<<1) 
{
  static const char* digits = "0123456789ABCDEF";
  std::string rc(hex_len,'0');
  
  for(int i=0, j=(hex_len-1)*4; i<hex_len; ++i, j-=4)
  {
    rc[i] = digits[(w>>j) & 0x0f];
  }
  
  return rc;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::GetDevicePath(int vid, int pid, long long serial, std::string *pPath)
{
  #ifdef WUSB_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  char data_buffer[2048];
  int i = 0, ret, found = 0;

  //----
  SP_DEVINFO_DATA                  deviceData;
  SP_DEVICE_INTERFACE_DATA         interfaceData;
  PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
  
  //Get all connected USB devices 
  HDEVINFO infoSet;
  infoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_DEVICEINTERFACE | 
	                            DIGCF_PRESENT | DIGCF_ALLCLASSES);
  if(infoSet == INVALID_HANDLE_VALUE) 
  { 
    
    #ifdef WUSB_DEBUG
    TRACE1("%s Error: No USB devices list\n", __FUNCTION__);
    #endif	  
	//No devices ?
	return -1; 
  }

  //----	
  std::string str_vid, str_pid, str_ser; 
  str_vid = "vid_";
  str_vid += int_to_hex(vid, 4);
  std::transform(str_vid.begin(), str_vid.end(), str_vid.begin(), tolower);
  //----
  str_pid = "pid_";
  str_pid += int_to_hex(pid, 4);
  std::transform(str_pid.begin(), str_pid.end(), str_pid.begin(), tolower);
  //----
  str_ser = "#";
  str_ser += std::to_string((long long)serial); str_ser += "#";
  std::transform(str_ser.begin(), str_ser.end(), str_ser.begin(), tolower);

  //----------------------------------------------------------
  //
  //----------------------------------------------------------
  for(;;)
  {
    //----
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    
	// GUID_DEVINTERFACE_USB_DEVICE  GUID_DEVCLASS_WINUSB   GUID_DEVCLASS_USB
    ret = SetupDiEnumDeviceInterfaces(infoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &interfaceData);
    if(ret!=1)
    {
       TRACE1("SetupDiEnumDeviceInterfaces: %u\n", GetLastError()); 
       break;
    }

    //----
    memset(data_buffer, 0, sizeof(data_buffer)); 
    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)data_buffer;
    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    deviceData.cbSize = sizeof(deviceData);
          
    if(!SetupDiGetDeviceInterfaceDetail(infoSet, &interfaceData, detailData, 
                                        sizeof(data_buffer), NULL, &deviceData)) 
    { 
      TRACE1("SetupDiGetDeviceInterfaceDetail: %u\n", GetLastError()); 
      break; 
    }
    
    //ret = IsEqualGUID(deviceData.ClassGuid, winusb_guid);
    //if(ret==1)
    //{  

    std::string devid(detailData->DevicePath);
    std::transform(devid.begin(), devid.end(), devid.begin(), tolower);

    //VID_0A5C&PID_21E8
    int fvid = (int)devid.find(str_vid);
    int fpid = (int)devid.find(str_pid);
    
    //---- ищем номер
    //"\\?\usb#vid_0a5c&pid_21e8#5cf3706479ca#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"
    //int pos_num = (int)std::find(devid.begin(), devid.end(), "#");
        
    if(fvid>=0 && fpid>=0)
    {
     *pPath = detailData->DevicePath;
      found = 1;
      break;
    }

    i++;
  }

  //----
  SetupDiDestroyDeviceInfoList(infoSet);

  if(found>0) return 1;
  return 0;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
int connectusb(void)
{
  int i, ret, found;     
  BOOL                             bResult = FALSE;
  HDEVINFO                         infoSet;
  SP_DEVINFO_DATA                  deviceData;
  SP_DEVICE_INTERFACE_DATA         interfaceData;
  PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
  DWORD n;
  char devdetailbuffer[2048];
  std::string DevicePath;

  infoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT | DIGCF_ALLCLASSES);
  if(infoSet == INVALID_HANDLE_VALUE) 
  { 
    return -1; 
  }
   /*
  // enumerute device information
  DWORD required_size = 0;
  int i = 0;
  for(;;)
  {
    //----
    deviceData.cbSize = sizeof(deviceData);
    int ret = SetupDiEnumDeviceInfo(deviceInfo, i, &deviceData);
    if(ret==0) 
       break;
    
    
    DWORD DataT;
    char friendly_name[2046] = { 0 };
    DWORD buffersize = 2046;
    DWORD req_bufsize = 0;

    // get device description information
    if(!SetupDiGetDeviceRegistryPropertyA(deviceInfo, &deviceData, 
                                          SPDRP_CLASSGUID, &DataT, 
                                         (PBYTE)friendly_name, 
                                          buffersize, &req_bufsize))
    {
      int res = GetLastError();
      continue;
    }

    char temp[512] = { 0 };
    sprintf_s(temp, 512, "USB device %d: %s", i, friendly_name);
    puts(temp);
  }	*/
   
  found = 0;
  i = 0;
  for(;;)
  {
    //----
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    //dev_info, dev_info_data, guid, 0, &dev_interface_data)) // GUID_DEVINTERFACE_USB_DEVICE  GUID_DEVCLASS_WINUSB
    ret = SetupDiEnumDeviceInterfaces(infoSet, NULL, &GUID_DEVINTERFACE_USB_DEVICE, i, &interfaceData);
    if(ret!=1)	// GUID_DEVCLASS_USB
    {
       TRACE1("SetupDiEnumDeviceInterfaces: %u\n", GetLastError()); 
       break;
    }
    
    memset(devdetailbuffer, 0, sizeof(devdetailbuffer)); 
    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)devdetailbuffer;
    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    deviceData.cbSize = sizeof(deviceData);
          
    if(!SetupDiGetDeviceInterfaceDetail(infoSet, &interfaceData, detailData, 
                                        sizeof(devdetailbuffer), NULL, &deviceData)) 
    { 
      TRACE1("SetupDiGetDeviceInterfaceDetail: %u\n", GetLastError()); 
      break; 
    }
    
    //ret = IsEqualGUID(deviceData.ClassGuid, winusb_guid);
    //if(ret==1)
    //{  
    std::string devid(detailData->DevicePath);
    std::transform(devid.begin(), devid.end(), devid.begin(), tolower);

    //VID_0A5C&PID_21E8
    int pvid = (int)devid.find("vid_0a5c");
    int ppid = (int)devid.find("pid_21e8");
    
    //---- ищем номер
    //"\\?\usb#vid_0a5c&pid_21e8#5cf3706479ca#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"
    //int pos_num = (int)std::find(devid.begin(), devid.end(), "#");
        
    if(pvid>=0 && ppid>=0)
    {
      DevicePath = detailData->DevicePath;
      found = 1;
      break;
    }

    i++;
 }

 //----
 SetupDiDestroyDeviceInfoList(infoSet);
 if(found<1) 
    return 0;
  
 HANDLE DeviceHandle;
 WINUSB_INTERFACE_HANDLE WinusbHandle;

 //----
 DeviceHandle = CreateFile(DevicePath.c_str(),
                           GENERIC_WRITE | GENERIC_READ,
                           FILE_SHARE_WRITE | FILE_SHARE_READ,
                           NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                           NULL);


 if(INVALID_HANDLE_VALUE == DeviceHandle) 
 {
   TRACE1("CreateFile: %u\n", GetLastError()); 
 }

 if(INVALID_HANDLE_VALUE == DeviceHandle) return 0;
 //DeviceHandle_Open = true;

 if(!WinUsb_Initialize(DeviceHandle, &WinusbHandle))
 {
   TRACE1("WinUsb_Initialize: %u\n", GetLastError()); 
   CloseHandle(DeviceHandle); 
   //DeviceHandle_Open = false;
   return 0;
 }

 //WinusbHandle_Open = true;
 return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::GetDeviceHandle(std::string *pPatch, HANDLE &hDevice)
{
  if(pPatch == NULL) return -1;
  
  //Get handle to device
  hDevice = CreateFile(pPatch->c_str(),
                       GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
					   NULL);

  if(hDevice == INVALID_HANDLE_VALUE)
  {
    TRACE1("CreateFile failed, Error: %d", GetLastError());
    return -1;      
  }
 
  return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::GetWinUSBHandle(HANDLE hDevice, WINUSB_INTERFACE_HANDLE &hUSB)
{
  int result;
  if(hDevice == NULL) return -1;
  WINUSB_INTERFACE_HANDLE hWUSB = NULL;

  //----	
  result = WinUsb_Initialize(hDevice, &hWUSB);
 
  if(result<1)
  {
    TRACE1("Error WinUsb_Initialize: %u\n", GetLastError()); 
    CloseHandle(hDevice);
	//----
	lib_opened = 0;
    dev_opened = 0;
    return -1;
  }

  lib_opened = 1;
  dev_opened = 1;
  hUSB = hWUSB;

  return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CWINUSB::GetDeviceHandle(GUID gDevice, WINUSB_INTERFACE_HANDLE &hDevice)
{
  int result;
   		/*
  if(pDeviceInterfaceDetail == NULL)
  {
    //status = GetDevicePath();
  }	  */
    
		  /*
  if(result!=1) return -1;

  //Get handle to device
  hDevice = CreateFile(pDeviceInterfaceDetail->DevicePath,
                       GENERIC_READ|GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL,
                       OPEN_EXISTING,
                       0, NULL);

  if(hDevice == INVALID_HANDLE_VALUE)
  {
    TRACE1("CreateFile failed, Error: %d", GetLastError());
    return -1;      
  }
      */
  return 1;
}

