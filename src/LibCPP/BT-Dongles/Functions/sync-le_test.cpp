//==============================================================================
//File name:   "sync-le_test.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "./Functions/bt.h"
#include "trace.h"
#include "bt-dongle.h"

static unsigned char patern = 4;

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::SendSync_LeTest(void)
{    
  unsigned char TxRxBuf[16];
  unsigned int cnt = 0, TxLength;
  unsigned long tx_sent;
  int result, rx_accepted;

  //HCI_LE_Transmitter_Test  HCI Opcode: 0x200A
  unsigned short opcode = BT_HCI_CMD_LE_TRANSMITTER_TEST;
  
  //HCI_LE_Transmitter_Test
  TxRxBuf[0] = 0x1E;//Opcode
  TxRxBuf[1] = 0x20;//Opcode
  TxRxBuf[2] = 3;   //CMD length 
  //TxRxBuf[3] = (2424-2402)>>1; //TX_Frequency:	 (2424 -2402)/2
  TxRxBuf[3] = 0;	//2=2406
  TxRxBuf[4] = 4;   //Length in bytes of payload data in each packet
  TxRxBuf[5] = patern;   //0x04=Pattern of All С1Т bits

  if(patern!=5) patern = 5;
  else patern = 4;

  //tx lenght
  TxLength = 6;

  //==== Ќачало передачи ===========================
  //Transmit cmd to dongle
  WUSB.ControlSend(WUSB.hUSB, (char*)TxRxBuf, TxLength);
  
  Sleep(4);

  //HCI_LE_Test_End	  
  opcode = BT_HCI_CMD_LE_TEST_END;
  TxRxBuf[0] = 0x1F;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 0; //CMD length 

  //tx lenght
  TxLength = 3;
  
  //==== Disable Test ===========================
  //Transmit cmd to dongle
  WUSB.ControlSend(WUSB.hUSB, (char*)TxRxBuf, TxLength);

  return 1;  
}

/* это вроде работало раньше
//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::SendSync_LeTest(void)
{  
  if(dongle_ready != 1) return -1;
    
  unsigned char TxRxBuf[16];
  unsigned int cnt = 0, TxLength;
  long d1, d2, d3, d4, d5;
  int result, rx_accepted;

  //HCI_LE_Transmitter_Test  HCI Opcode: 0x200A
  unsigned short opcode = BT_HCI_CMD_LE_TRANSMITTER_TEST;
  
  //HCI_LE_Transmitter_Test
  TxRxBuf[0] = 0x1E;//Opcode
  TxRxBuf[1] = 0x20;//Opcode
  TxRxBuf[2] = 3;   //CMD length 
  //TxRxBuf[3] = (2424-2402)>>1; //TX_Frequency:	 (2424 -2402)/2
  TxRxBuf[3] = 0;	//2=2406
  TxRxBuf[4] = 4;   //Length in bytes of payload data in each packet
  TxRxBuf[5] = patern;   //0x04=Pattern of All С1Т bits

  if(patern!=5) patern = 5;
  else patern = 4;

  //tx lenght
  TxLength = 6;

  //Ќачало передачи
  QueryPerformanceFrequency(&Frequency);
  QueryPerformanceCounter(&timeStart);
/*  d1 = Calc_Delay(&framePeriod, &timeStart);
  framePeriod = timeStart;

  if(d1<33333-3000 || d1>33333+3000) return -1;
 */
/*
  //============================ Enable Beacon ===========================
  //Transmit packet to dongle
  result = libusb_control_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE |
	                                     LIBUSB_REQUEST_TYPE_CLASS | 
										 LIBUSB_ENDPOINT_OUT,
										 0, //set/get test
										 0, //test type								 
										 usb_interface, //interface id 
										 TxRxBuf, TxLength, 0);    
  
  ///QueryPerformanceCounter(&t1);
  ///d1 = Calc_Delay(&timeStart, &t1);
  //memset(TxRxBuf, 0xcc, 16);

  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   TxRxBuf, 0x06, &rx_accepted, 4); 

  ///QueryPerformanceCounter(&t2);
  ///d2 = Calc_Delay(&t1, &t2);

  if(TxRxBuf[5]!=0 )TRACE("cmd: 0x%X Failed\n", opcode); 


  //HCI_LE_Test_End	  
  opcode = BT_HCI_CMD_LE_TEST_END;
  TxRxBuf[0] = 0x1F;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 0; //CMD length 

  //tx lenght
  TxLength = 3;
  
  //Sleep(1);
    
  ///QueryPerformanceCounter(&t3);
  ///d3 = Calc_Delay(&t2, &t3);

  //============================ Disable Beacon ===========================
  //Transmit packet to dongle
  result = libusb_control_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE |
	                                     LIBUSB_REQUEST_TYPE_CLASS | 
										 LIBUSB_ENDPOINT_OUT,
										 0, //set/get test
										 0, //test type								 
										 usb_interface, //interface id 
										 TxRxBuf, TxLength, 0);    
  
  ///QueryPerformanceCounter(&t4);
  ///d4 = Calc_Delay(&t3, &t4);
  
  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   TxRxBuf, 12, &rx_accepted, 4); 
  ///QueryPerformanceCounter(&t5);
  ///d5 = Calc_Delay(&t4, &t5);
 
  if(TxRxBuf[5]!=0 )TRACE("cmd: 0x%X Failed\n", opcode); 

  return 1;  
}
*/

/* STARY
//==============================================================================
//File name:   ".cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "./Functions/bt.h"
#include "trace.h"
#include "bt-dongle.h"
//#include "protocol.h"

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Sync_LeTest(void)
{  
  if(dongle_ready != 1) return -1;
    
  unsigned char TxRxBuf[16];
  unsigned int cnt = 0, TxLength;
  long d1, d2, d3, d4, d5;
  int result, rx_accepted;

  //HCI_LE_Transmitter_Test  HCI Opcode: 0x200A
  unsigned short opcode = BT_HCI_CMD_LE_TRANSMITTER_TEST;
  
  //HCI_LE_Transmitter_Test
  TxRxBuf[0] = 0x1E;//Opcode
  TxRxBuf[1] = 0x20;//Opcode
  TxRxBuf[2] = 3;   //CMD length 
  //TxRxBuf[3] = (2424-2402)>>1; //TX_Frequency:	 (2424 -2402)/2
  TxRxBuf[3] = 0;	//2=2406
  TxRxBuf[4] = 4;   //Length in bytes of payload data in each packet
  TxRxBuf[5] = 4;   //0x04=Pattern of All С1Т bits

  //tx lenght
  TxLength = 6;

  //Ќачало передачи
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
  //memset(TxRxBuf, 0xcc, 16);

  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   TxRxBuf, 0x06, &rx_accepted, 4); 

  QueryPerformanceCounter(&t2);
  d2 = Calc_Delay(&t1, &t2);

  if(TxRxBuf[5]!=0 )TRACE("cmd: 0x%X Failed\n", opcode); 


  //HCI_LE_Test_End	  
  opcode = BT_HCI_CMD_LE_TEST_END;
  TxRxBuf[0] = 0x1F;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 0; //CMD length 

  //tx lenght
  TxLength = 3;
  
  //Sleep(1);
    
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
										   TxRxBuf, 12, &rx_accepted, 4); 
  QueryPerformanceCounter(&t5);
  d5 = Calc_Delay(&t4, &t5);
 
  if(TxRxBuf[5]!=0 )TRACE("cmd: 0x%X Failed\n", opcode); 

  return 1;  
}
 */

