//==============================================================================
//File name:   "bt-dongles.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _BT_DONGLES_H_
#define _BT_DONGLES_H_

#include <windows.h>
#include <string>
#include "cmd_def.h"
#include "utils.h"
#include "thread.h"
#include <libusb.h>
#include "beacon_msg.h"

#pragma once

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------
class CBTDG
{
//Construction
public:

  CBTDG();
 ~CBTDG();
   
  CThreadR ThreadRx;

  //vars
  unsigned short m_state;
  hci_connection_t app_connection;
  hci_attclient_t app_attclient;
 
//Operations
public:
   int  Open(void);
   int  Ini(void);
   void Close(void);
   int  Connect(void);
   int  Disconnect(void);
   int  Wait_Response(void);
   int  Wait_Event(void);
   void Write_Message(unsigned char len1, unsigned char *data1, 
                      unsigned short len2,unsigned char *data2);
   int DataTx(char handle, char *pBuf, int length);
   int DataRx(char *pBuf);
   int Read_Message(void);
   int Find_Dongles(void);
   int HCI_Send(unsigned short opcode, const void *pData, unsigned char size);
   void Show_Errors(int error_code);

   int HCI_TxCMD(unsigned short opcode,  //CMD opcode
                 const void *pTxBuf, 	 //CMD buffer
                 unsigned char TxLength, //CMD length
                 const void *pRxBuf, 	 //Reply buffer
                 int RxLength,  //Reply length
                 int RxTimeout);//Reply wait timeout
protected:   
   int  subsystem;
   int  usb_open;
   int  usb_interface;
   int  usb_configuration;
   int  usb_dev_vid;
   int  usb_dev_pid;
   int  usb_dev_ready;
   
   //BD_ADDR of the Bluetooth Host Device 
   unsigned char BD_ADDR[6];

   //Storage to hold recently received data
   unsigned char buffer[256];

   struct libusb_context *ctx;
   struct libusb_device_handle *hUSB;
   struct libusb_transfer *rx_xfer;
   struct libusb_device_descriptor descr_dev;
   struct libusb_config_descriptor descr_conf;
   
   inline void Error(void);   
   void SetEventMask(long long &mask);
   void SetEventMaskLE(long long &mask); 
   void SetEventMaskPage2(long long &mask);
   inline int CheckResponse(int opcode, unsigned char *pRxBuf, int RxLength);

public:
   //==== 3d part =====================================
   //unsigned long btcontroller_clock_begin;
   LARGE_INTEGER Frequency, timeStart, timeStop, t1, t2, t3, t4, t5;   
   unsigned char beacon_data[32];
   int sync_on;
   int beacon_mode; //le_test / beacon
   int Beacon_ini(void);
   int BeaconEnable(int on_off);
   int Beacon_DataUpdate(void);
   int Sync_Beacon(void);
   int Sync_LeTest(void);
   unsigned long Calc_Delay(LARGE_INTEGER *pTimeStart, LARGE_INTEGER *pTimeEnd);
   unsigned long Calc_Time(LARGE_INTEGER *pTime);
   struct Beacon_tx_struct beacon_tx;
};

extern CBTDG *pBTDG;

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif
