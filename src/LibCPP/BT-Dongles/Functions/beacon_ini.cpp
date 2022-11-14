//==============================================================================
//File name:   "beacom_ini.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "./Functions/bt.h"
#include "trace.h"
#include "bt-dongle.h"

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Beacon_ini(void)
{  
  if(dongle_ready != 1) return -1;
  
  TRACE("==== Ini Beacon mode ====\n");
  
  unsigned long btcontroller_clock_begin;
  unsigned long time, old_time=0;
  unsigned int cnt = 0;
  float delta;
  int idelta;
  unsigned char buf[64];
  int result;
  
  btcontroller_clock_begin = 0;


  //LE_Set_Advertise_Enable  HCI Opcode: 0x200A
  buf[0] = 0x01;
  result = HCI_CMD(BT_HCI_CMD_LE_SET_ADV_ENABLE, buf, 1, buf, -1, 40);
  if(result<0) return -1;
  //---- run запустилось
  Sleep(40);
    

   /*
  
  for(;;)
  {
    //читаем внутренние часы контроллера
    //HCI_Read_Clock: Connection_Handle Which_Clock  HCI Opcode: 0x1407
    buf[0] = 0x00; 
    buf[1] = 0x00; //handle 0 
    buf[2] = 0x00; //0x00 = Local Clock
    result = HCI_CMD(BT_HCI_CMD_READ_CLOCK, buf, 3, buf, 0x0E, 2);
  
    if(result>0 && buf[0]==0x0e && buf[1]==0x0c)
     {
       memcpy(&time, &buf[8], 4);
	   delta = time - old_time;
	   delta *= 0.3125;// msec;
	   idelta = (int)delta;
	   old_time = time;
       TRACE("%d %d delta: %d\n",cnt, time, idelta);
     }
  
     cnt++;
     Sleep(100);
   } */
  
  TRACE("BLE-advertising mode starting\n");
  return 1;  
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::BeaconEnable(int on_off)
{  
  if(dongle_ready != 1) return -1;
    
  unsigned char TxRxBuf[16];
  unsigned long btcontroller_clock_begin;
  unsigned long time, old_time = 0;
  unsigned int cnt = 0, TxLength = 0;
  long d1, d2, d3, d4, d5;
  int idelta;
  int result, rx_accepted;

  //LE_Set_Advertise_Enable  HCI Opcode: 0x200A
  unsigned short opcode = BT_HCI_CMD_LE_SET_ADV_ENABLE;
  btcontroller_clock_begin = 0;
  
  //cmd data lenght
  TxRxBuf[0] = 0x0A;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 1; //CMD length 
  TxRxBuf[3] = on_off; //Advertising enable/disable
  //tx lenght
  TxLength = 4;

  //Начало передачи
  QueryPerformanceFrequency(&Frequency);
  QueryPerformanceCounter(&timeStart);

  //============================ Enable Beacon ===========================
  //Transmit packet to dongle
  result = libusb_control_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE |
	                                     LIBUSB_REQUEST_TYPE_CLASS | 
										 LIBUSB_ENDPOINT_OUT,
										 0, //set/get test
										 0, //test type								 
										 usb_interface, //interface id 
										 TxRxBuf, TxLength, 0);    
  
  QueryPerformanceCounter(&t1);
  d1 = Calc_Delay(&timeStart, &t1);

  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   TxRxBuf, 0x0E, &rx_accepted, 20); 
/*
  QueryPerformanceCounter(&t2);
  d2 = Calc_Delay(&t1, &t2);

  //cmd data lenght
  TxRxBuf[0] = 0x0A;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 1; //CMD length 
  TxRxBuf[3] = 0; //Advertising is disabled
  //tx lenght
  TxLength = 4;
  
  Sleep(20);
    
  QueryPerformanceCounter(&t3);
  d3 = Calc_Delay(&t2, &t3);

  //============================ Disable Beacon ===========================
  //Transmit packet to dongle
  result = libusb_control_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE |
	                                     LIBUSB_REQUEST_TYPE_CLASS | 
										 LIBUSB_ENDPOINT_OUT,
										 0, //set/get test
										 0, //test type								 
										 usb_interface, //interface id 
										 TxRxBuf, TxLength, 0);    
  
  QueryPerformanceCounter(&t4);
  d4 = Calc_Delay(&t3, &t4);
  
  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   TxRxBuf, 0x0E, &rx_accepted, 4); 
  QueryPerformanceCounter(&t5);
  d5 = Calc_Delay(&t4, &t5);
*/  
 // TRACE("Tx cmd: %d, Rx answ: %d, Sleep: %d, Tx cmd: %d, Rx Answ: %d\n", d1,d2,d3,d4,d5);

  return 1;  
}
