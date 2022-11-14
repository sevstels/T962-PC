//==============================================================================
//File name:   "tx_letest.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "./Functions/bt.h"
#include "trace.h"
#include "bt-dongle.h"
#include "LE_test_protocol.h"

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------
int CBTDONGLE::Tx_LeTest(char freq, char cmd, unsigned char param)
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

  //---- параметры пакета передачи патерна
  TxRxBuf[4] = 8;   //Length in bytes of payload data in each packet
  TxRxBuf[5] = 1;   //0x04=Pattern mode, of All С1Т bits
  
  result = protocol_encoder(cmd, param, (char*)&TxRxBuf[4]);

  if(result<0)
  {
    TRACE("cmd: 0x%X Failed\n", cmd);
	return -1;
  }
  
  //tx lenght
  TxLength = 6;

  //============================ Enable LeTest ===========================
  //Transmit packet to dongle
  result = libusb_control_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE |
	                                     LIBUSB_REQUEST_TYPE_CLASS | 
										 LIBUSB_ENDPOINT_OUT,
										 0, //set/get test
										 0, //test type								 
										 usb_interface, //interface id 
										 TxRxBuf, TxLength, 2);
  //memset(TxRxBuf, 0xcc, 16);

  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   TxRxBuf, 0x06, &rx_accepted, 100); 

  if(TxRxBuf[5]!=0)TRACE("cmd: 0x%X Failed\n", opcode); 


  //HCI_LE_Test_End	  
  opcode = BT_HCI_CMD_LE_TEST_END;
  TxRxBuf[0] = 0x1F;
  TxRxBuf[1] = 0x20;
  TxRxBuf[2] = 0; //CMD length 

  //tx lenght
  TxLength = 3;
  
  //Sleep(1);
    
  //============================ Disable LeTest ===========================
  //Transmit packet to dongle
  result = libusb_control_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE |
	                                     LIBUSB_REQUEST_TYPE_CLASS | 
										 LIBUSB_ENDPOINT_OUT,
										 0, //set/get test
										 0, //test type								 
										 usb_interface, //interface id 
										 TxRxBuf, TxLength, 2);    
    
  //Read answer packet from dongle
  result = libusb_interrupt_transfer(hUSB, LIBUSB_RECIPIENT_INTERFACE | 
	                                       LIBUSB_ENDPOINT_IN,
										   TxRxBuf, 12, &rx_accepted, 100); 
 
  if(TxRxBuf[5]!=0 )TRACE("cmd: 0x%X Failed\n", opcode); 

  return 1;  
}
