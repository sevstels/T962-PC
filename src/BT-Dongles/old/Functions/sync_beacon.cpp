//==============================================================================
//File name:   "beacom.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "./Functions/bt.h"
#include "trace.h"
#include "bt-dongles.h"
#include "beacon_msg.h"
#include "cmd.h"

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Sync_Beacon(void)
{  
  if(usb_dev_ready != 1) return -1;
    
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
  
  if(sync_on==0)sync_on=1;
  else sync_on = 0;

  //cmd data lenght
  TxRxBuf[0] = 0x0A;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 1; //CMD length 
  TxRxBuf[3] = sync_on; //Advertising is enabled
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
 
unsigned char cntr = 0;
unsigned char old_cmd = 0;
//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDG::Beacon_DataUpdate(void)
{  
  if(usb_dev_ready != 1) return -1;
    
  unsigned char TxRxBuf[40];
  unsigned long btcontroller_clock_begin;
  unsigned long time, old_time = 0;
  unsigned int cnt = 0, TxLength = 0;
  long d1, d2, d3, d4, d5;
  int idelta;
  int result, rx_accepted;

  //LE_Set_Advertise_Enable  HCI Opcode: 0x2008
  unsigned short opcode = BT_HCI_CMD_LE_SET_ADV_DATA;

  //cmd data lenght
  TxRxBuf[0] = 0x08;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 32; //CMD length 

  //получить текущее значение времени
  QueryPerformanceFrequency(&Frequency);
  QueryPerformanceCounter(&timeStart);
  
  //инициализация передаваемых данных
  beacon_tx.cmd_header = CMD_BEACON;
  beacon_tx.tx_counter++;
  beacon_tx.advert_interval_ms = 100;
  beacon_tx.frame_rate_us = 16666;
  beacon_tx.pc_clock_us = Calc_Time(&timeStart);

/*  //делать только одну передачу если это команда
  if(beacon_data[1] == CMD_HEADER &&
	 old_cmd == CMD_HEADER)
  {
  	beacon_data[2] = ;
   // old_cmd = 0;
  }*/
  
  //old_cmd = beacon_data[1];

  memcpy(&TxRxBuf[3], &beacon_tx, 32);

  //tx lenght
  TxLength = 32+3;

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
										   TxRxBuf, 0x0E, &rx_accepted, 10); 

  QueryPerformanceCounter(&t2);
  d2 = Calc_Delay(&t1, &t2);

  return 1;  
}

