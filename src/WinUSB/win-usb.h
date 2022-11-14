//==============================================================================
//File name:   "win-usb.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _LIB_WINUSB_DRIVER_H_
#define _LIB_WINUSB_DRIVER_H_

#include <winusb.h>
#include <string>
#include <vector>

#pragma once

//----
#define EP1_IN       0x81  //
#define EP1_OUT      0x01  //
#define EP2_IN       0x82  //
#define EP2_OUT      0x02  //

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class CWINUSB
{
  //Construction
  public:

  CWINUSB();
 ~CWINUSB();
   
  //Operations
  int Open(void);
  void Close(void);

  //----
  int ControlSend(WINUSB_INTERFACE_HANDLE hUSB, char *pBuf, int length);
  int ControlRead(WINUSB_INTERFACE_HANDLE hUSB, char *pBuf, int buf_length, 
	                                            int &rx, int timeout);
  
  //----
  int BulkSend(WINUSB_INTERFACE_HANDLE hUSB, unsigned char EP, 
	                                         char *pBuf, int length);
  int BulkRead(WINUSB_INTERFACE_HANDLE hUSB, unsigned char EP, char *pBuf, 
	                                         int length, int &rx);

  //----
  int GetDeviceList(void);
  int OpenDevice(int index);
  int CloseDevice(void);
  int FindDevice(int vid, int pid, long long serial);
  void FlushFifo(void);

  //----
  int GetDevicePath(int vid, int pid, long long serial, std::string *pPath);
  int GetDeviceHandle(GUID gDevice, HANDLE &hDevice);
  int GetDeviceHandle(std::string *pPatch, HANDLE &hDevice);
  int GetWinUSBHandle(HANDLE hDevice, WINUSB_INTERFACE_HANDLE &hUSB);
  
  //----
  int GetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hUSB, char *pDevSpeed);
  int QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hUSB);

  int  lib_opened;
  int  dev_opened;
  int  dev_vid;
  int  dev_pid;
  char dev_speed;
  long long dev_serial;
  //std::string dev_serial;
  int  usb_interface;
  int  usb_configuration;
  
  public:
  //---- USB Adapters
  std::vector <unsigned short> VIDs; //USB Vendor ID
  std::vector <unsigned short> PIDs; //USB Product ID
  std::vector <long long> IDs;
  std::vector <std::string> Manufacturers;
  std::vector <std::string> Names;
  std::vector <std::string> DevPatchs;
  WINUSB_INTERFACE_HANDLE hUSB;

  protected:  
  GUID gdWINUSB; 
  HANDLE hDevice;
  std::string DevicePath;
};

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif
