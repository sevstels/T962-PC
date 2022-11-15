//==============================================================================
//File name:   "bt-dongles.h"
//Purpose:      Header File
//Version:      2.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _BT_DONGLES_H_
#define _BT_DONGLES_H_

#include <WinDef.h>
#include <afxcoll.h>
#include <afxmt.h>	  //MFC: CCriticalSection, CEvent
#include "threads.h"
#include <string>   //для string
#include <vector>   //для vector
#include "bt-firmware.h"
#include "win-usb.h"

//transport id
#define	RXTX_BLE            2
#define TRANSPORT_BLE       2
//----
#define EP1_IN              0x81  //Event Input, interrupt, maxsz=16
#define EP1_OUT             0x01  //
#define EP2_IN              0x82  //ACL Input, builk, maxsz=64 
#define EP2_OUT             0x02  //ACL Output, isoh

#define BT_DEV_ARRIVED      4
#define BT_DEV_EXIT         5
#define BT_DEV_CONNECTED    6
#define BT_DEV_DISCONNECTED 7
#define BT_DEV_CLOSE_CONN   8

//---- If need Debug
#define BLE_DEBUG
#define USE_WINUSB

#ifdef USE_WINUSB
#ifdef _WIN64
//--- win64
#ifdef NDEBUG
//#pragma comment(lib,"") //64bit Win
#else
//#pragma comment(lib,"")//64bit Win
#endif
#else
//--- win32
#ifdef NDEBUG
#pragma comment(lib,"C:/WinDDK/lib/win7/i386/winusb.lib")//32bit Win
#else
#pragma comment(lib,"C:/WinDDK/lib/win7/i386/winusb.lib")//32bit Win
#endif 
#endif //end _WIN64
#else
#include <libusb.h>

#endif


//struct BD_Addr{char addr[6];}

#pragma once

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class CBTDONGLE
{
  //Construction
  public:

  CBTDONGLE();
 ~CBTDONGLE();
  
  CWINUSB WUSB;

  //CThreadR ThreadRx;
  int thead;
  ВT_manufacturers ВT_DNGL;

  //Operations
  int  OpenUSB(void);
  void CloseUSB(void);
  int  CheckDriver(int vid, int pid, long dev_num);
  int  Find_Dongles(void);
  void Dongle_Supported(void);
  int  Dongle_Open(int index);
  int  Dongle_WriteFirmware(void);
  int  Dongle_BLEIni(void);
  int  Dongle_BLE_ini(void);
  int  Dongle_Debug_ini(void);
  void FifoCLR(void);
  int  FindAdapters(void);
  //----
  int  BCM20702_ini(void);
  int  CSR8510A10_ini(void);

  //-- это проверить потом
  void BT_AddrToTxt1(CString *pTxt, long long Addr);
  void BT_AddrToStr1(char *pAddr, long long Addr);
  void BT_AddrToStr(std::string &adr_txt, char *pAddr);
  int  AddFilter_DeviceName(char *pDeviceName);
  int  AddFilter_AdapterName(char *pAdapterName);
  int  Parse_DeviceName(char *pBuf, int length, std::string *pName);

  //----
  int  Disconnect2(void);
  int  Wait_Response(void);
  int  WaitEvent(int event_id);
  int  WaitEvData(void);
  int  WaitEvBLE(void);
  void Write_Message(unsigned char len1, unsigned char *data1, 
                     unsigned short len2,unsigned char *data2);
  int DataTx(char handle, char *pBuf, int length);
  int DataRx(char *pBuf);
  int Read_Message(void);
  int HCI_Send(unsigned short opcode, const void *pData, unsigned char size);
  void Show_Errors(int error_code);
  
  //----
  int CheckResponse(int opcode, unsigned char *pRxBuf, int RxLength);
  int HCI_CMD(unsigned short opcode, //CMD opcode
              const void *pTxBuf, 	  //CMD buffer
              unsigned char TxLength,//CMD length
              const void *pRxBuf, 	  //Reply buffer
              int RxLength,  //Reply length
              int RxTimeout);//Reply wait timeout
  
  int HCI_FWR(unsigned short opcode, //CMD opcode
              const void *pTxBuf, 	  //CMD buffer
              unsigned char TxLength,//CMD length
              const void *pRxBuf, 	  //Reply buffer
              int RxLength,  //Reply length
              int RxTimeout);//Reply wait timeout

  int HCI_DATA(const void *pTxBuf,    //buffer
               unsigned char TxLength);//length

  //BD_ADDR of the Bluetooth Host Device 
  unsigned char HOST_BT_ADDR[6];
  unsigned char DEV_BT_ADDR[6];
  //Storage to hold recently received data
  unsigned char HCI_buf[256+64];	//HCI RxTx
  unsigned char L2CAP_buf[1024+64];	//L2CAP RxTx
  unsigned short dev_connection_handle;

  //----
  char sync_cmd[12];
  char *pSyncBuf;
  int sync_ready;
  int sync_length;
  int sync_completed;

  //=========================================
  //BLE
  //=========================================
  int  LE_AdvertisingReport(unsigned char *pBuf, int length);
  void LE_Connection_Complete(unsigned char *pBuf, int length);
  void LE_Read_Remote_Features_Complete(unsigned char *pBuf, int length); 
  void Read_Remote_Version_Information_Complete(unsigned char *pBuf, int length);
  //----
  void L2CAP_Decoder(int length);
  void L2CAP_Encoder(unsigned char *pBuf, int length);

  protected:   
  int subsystem;
  int usb_opened;
  int usb_interface;
  int usb_configuration;
  int usb_dev_vid;
  int usb_dev_pid;
  int dongle_ready;
   
  //----
  inline void Error(void);
  void SetEventMask(long long &mask);
  void SetEventMaskLE(long long &mask); 
  void SetEventMaskPage2(long long &mask);

  public:
  //==== 3d part =====================================
  //unsigned long btcontroller_clock_begin;
  unsigned char beacon_data[32];
  int sync_on;
  int beacon_mode; //le_test / beacon
  int Beacon_ini(void);
  int BeaconEnable(int on_off);
  int Beacon_DataUpdate(void);
  int Sync_Beacon(void);
  int Tx_LeTest(char freq, char cmd, unsigned char param);

  //-------
  int LE_Connect(char freq, char cmd, unsigned char param); 
  //====================================================================================


  public:
   //First for ini
  //int  Open(void);
  void Close(void);

  //Application Data handler, function pointer
  //Must be the 'static void' defenition!
  void (*pRXhandler)(char*, int);
  //Place RXhandler function before(above) AddRxHandler call
  int AddRxHandler(void(*pRXhandler)(char*, int));

  //Application BT Events handler, function pointer
  //Must be the 'static void' defenition!
  void (*pBTEventsHandler)(int, char*, int);
  //Place BTevents function before(above) AddBTeventsHandler call
  int AddBTeventsHandler(void(*pBTEventsHandler)(int, char*, int));

  //Return connected Bluetooth adapter list on PC and put it to ComboBox control 
  int GetAdaptersList(HANDLE lpComboBox, HANDLE pCStringArrayName);
  
  //Return last used adapter name
  int GetLastUsedAdapterName(char *pBuffer, int length);
  
  //Switch to another Bluetooth adapter
  int ChangeAdapter(int index);
  
  //Open Bluetooth adapter and get Handle 
  int AdapterOpen(unsigned short vid, unsigned short pid);
  
  //Close Bluetooth adapter
  void AdapterClose(void);

  //Return BLE device list 
  int GetDevicesList(HANDLE lpListBox,  //list recipient
                     LPVOID pDevIDs,    //CDWordArray  pointer
                     LPVOID pDevNames); //CStringArray pointer
  
  int AdvertisingData(unsigned char *pBuf, int length);  
  
  //BLE Devices
  int Connect(char *pDevAddress);
  int Disconnect(void);
  int FindInList(char *pStrDeviceID);
  int FindAddrInList(char *pStrDevAddress);
  int GetIdByName(char *pStrDevName);

  //Send bytes array to device
  int SendData(char *pBuffer, int length);
  int SendSync(void);
  int SendSync_CMD(char *pBuffer, int length);
  int SendSync_LeTest(void);
  int SendCommand(char cmd, char param1, char param2);
   
  int adapters_number; //BT Adapters number detected
  int devices_number;  //BT Devices number detected
  int device_index;    //BT Device index currently connected
  int adapter_opened;  //BT adapter status
  int adapter_model;   //BT adapter index in the ВT_DNGL list
  int adapter_index;   //BT opened adapter index in the list
  unsigned short adapter_vid;
  unsigned short adapter_pid;
  int dev_connected;   //BLE Device status
  int dll_opened;
  int dev_selected;    //1 if device was selected

  //---- BLE Devices
  CEvent ev_CMD;
  CEvent ev_DATA;
  CEvent ev_SYNC;
  CEvent ev_BLE;

  //Bluetooth Adapter
  std::vector <std::string> Filter_AdapterName;
  std::vector <std::string> DevicesName;
  std::vector <std::string> DevicesID;
  std::string Last_Opened_DeviceID;

  //Bluetooth devices
  std::vector <std::string> Filter_DeviceName;
  std::vector <std::string> Devices_Name;
  std::vector <std::vector<char>> Devices_BT_ADDR;
  std::vector <signed char> Devices_RSSI;
  std::vector <std::string> Devices_BT_ADDR_str;
  std::string dev_name;
};

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif
