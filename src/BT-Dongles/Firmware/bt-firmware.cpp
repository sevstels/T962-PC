//==============================================================================
//File name:   "bt-firmware.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "bt.h"
#include "bt-firmware.h"
#include "bt-dongle.h"

//firmware files
#include "BCM20702A0.h"
#include "CSR8510A10.h"
#include "RTL8761A.h"
	
//#define  BLE_DEBUG

//------------------------------------------------------------------------------
//Function ВT_manufacturers
//------------------------------------------------------------------------------
void CBTDONGLE::Dongle_Supported(void)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  //---- 
  memset(&ВT_DNGL, 0, sizeof(ВT_DNGL));
  
  //число известных программе моделей донглов
  ВT_DNGL.numbers = DONGLES_NUM;

  //---- 1
  ВT_DNGL.name[0] = "BCM20702 A0";
  ВT_DNGL.manufacturer[0] = "Asus";
  ВT_DNGL.vid[0] = 0x0B05;
  ВT_DNGL.pid[0] = 0x17CB;
  ВT_DNGL.id[0] = 0;
  ВT_DNGL.pFirmware[0] = BCM20702A0;
  ВT_DNGL.firmwareLength[0] = sizeof(BCM20702A0);

  //---- 2
  ВT_DNGL.name[1] = "CSR8510 A10";
  ВT_DNGL.manufacturer[1] = "Qualcomm";
  ВT_DNGL.vid[1] = 0x0A12;
  ВT_DNGL.pid[1] = 0x0001;
  ВT_DNGL.id[1] = 0;
  ВT_DNGL.pFirmware[1] = NULL;
  ВT_DNGL.firmwareLength[1] = 0;
  
  //---- 3 
  ВT_DNGL.name[2] = "BCM20702 A1";
  ВT_DNGL.manufacturer[2] = "Broadcom";
  ВT_DNGL.vid[2] = 0x0A5C;
  ВT_DNGL.pid[2] = 0x21E6;
  ВT_DNGL.id[2] = 0;
  ВT_DNGL.pFirmware[2] = NULL;
  ВT_DNGL.firmwareLength[2] = 0; 

  //---- 4
  ВT_DNGL.name[3] = "CC2540";
  ВT_DNGL.manufacturer[3] = "Texas Instruments";
  ВT_DNGL.vid[3] = 0x0451;
  ВT_DNGL.pid[3] = 0x16B3;
  ВT_DNGL.id[3] = 0;
  ВT_DNGL.pFirmware[3] = NULL;
  ВT_DNGL.firmwareLength[3] = 0;

  //---- 5
  ВT_DNGL.name[4] = "RTL8761A";
  ВT_DNGL.manufacturer[4] = "Realtek";
  ВT_DNGL.vid[4] = 0x0BDA;
  ВT_DNGL.pid[4] = 0x8771;
  ВT_DNGL.id[4] = 0;
  ВT_DNGL.pFirmware[4] = NULL;
  ВT_DNGL.firmwareLength[4] = 0;
  
  //---- 6
  ВT_DNGL.name[5] = "CC2400";
  ВT_DNGL.manufacturer[5] = "Ubertooth-one";
  ВT_DNGL.vid[5] = 0x1D50;
  ВT_DNGL.pid[5] = 0x6002;
  ВT_DNGL.id[5] = 0;
  ВT_DNGL.pFirmware[5] = NULL;
  ВT_DNGL.firmwareLength[5] = 0;


  //---- 7
  ВT_DNGL.name[6] = "BCM20702 A0";
  ВT_DNGL.manufacturer[0] = "Broadcom";
  ВT_DNGL.vid[6] = 0x0A5C;
  ВT_DNGL.pid[6] = 0x21E8;
  ВT_DNGL.id[6] = 0;
  ВT_DNGL.pFirmware[0] = BCM20702A0;
  ВT_DNGL.firmwareLength[0] = sizeof(BCM20702A0);

  /*
  //---- 
  ВT_DNGL.name[2] = "BLED112";
  ВT_DNGL.manufacturer[2] = "Bluegiga";
  ВT_DNGL.vid[2] = 0x2458;
  ВT_DNGL.pid[2] = 0x0001;
  ВT_DNGL.id[2] = 0;
  ВT_DNGL.pFirmware[2] = NULL;
  ВT_DNGL.firmwareLength[2] = 0; 

  //---- 7
  ВT_DNGL.name[4] = "";
  ВT_DNGL.manufacturer[4] = "";
  ВT_DNGL.vid[4] = 0x;
  ВT_DNGL.pid[4] = 0x;
  ВT_DNGL.id[4] = 0;
  ВT_DNGL.pFirmware[4] = NULL;
  ВT_DNGL.firmwareLength[4] = 0;
  */
}

//------------------------------------------------------------------------------
//Function 
//------------------------------------------------------------------------------     
int CBTDONGLE::Dongle_WriteFirmware(void)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif
  
  //------------------------------------------------
  int result;  
  int state = 0;
  int block = 0;
  int length;
  unsigned char  buf[12];
  unsigned short opcode;
  unsigned int   fw_length;
  unsigned char *pBuf;   

  //проверяем, нужно ли грузить фирмваре
  ///ВT_DNGL.firmwareLength[5] = 0;

  if(adapter_vid == 0x0A5C && adapter_pid == 0x21E8)
  {
     fw_length = sizeof(BCM20702A0);
     pBuf = BCM20702A0;
  }
  else if(adapter_vid == 0x0A12 && adapter_pid == 0x0001)
  {
     //Cambridge Silicon Radio Ltd. 
	 //fw_length = 0;//sizeof(CSR8510A10);
     //pBuf = CSR8510A10;
	 return 1;
  } 

  else if(adapter_vid == 0x0BDA && adapter_pid == 0x8771)
  {
     fw_length = sizeof(RTL8761A);
     pBuf = RTL8761A;
  } 
  else return 1;

  //Reset   
  //HCI_Reset cmd: 0x0C03 ret:0e 04 01 03 0c 00    
  result = HCI_CMD(BT_HCI_CMD_RESET, NULL, 0, buf, 1, 2000);
  		 
  //0x00 Reset command succeeded, was received and will be executed
  if(result!=1 || buf[0]!=0)  
	 return -2; 

 /*
  //HCI_Read_Buffer_Size  HCI Opcode: 0x1005
  struct bt_hci_rsp_read_buffer_size buf_size;
  memset(&buf_size, 0, sizeof(buf_size));
  HCI_CMD(0x1005, NULL, 0, &buf_size, sizeof(buf_size), 500);
   
  TRACE("Read_Buffer_Size:\n");
  TRACE1("Status %02X\n", buf_size.status); //Status: Success (0x00)	 	
  TRACE1("HC ACL Data Packet Length %d\n", buf_size.acl_mtu);
  TRACE1("HC Synchronous_Data_Packet Length %d\n", buf_size.sco_mtu);
  TRACE1("HC Total_Num ACL Data_Packets %d\n", buf_size.acl_max_pkt);
  TRACE1("HC Total Num Synchronous Data Packets %d\n", buf_size.sco_max_pkt);

  //HCI_Host_Buffer_Size  HCI Opcode: 0x0C33
  buf[0] = 0x9B; buf[1] = 0x06; buf[2] = 0xFF; buf[3] = 0x14; 
  buf[4] = 0x00; buf[5] = 0x0A; buf[6] = 0x00;
  HCI_CMD(0x0C33, buf, 7, NULL, 0, 500);
*/ 
  //HCI Vendor command: Broadcom Read Verbose Config Version Info
  result = HCI_CMD(0xfc79, NULL, 0, buf, 7, 500);
  if(result<0) return -1;
  //buf[1] Chip ID: 0x3f

  //Start Download
  //HCI Vendor command: Broadcom Download Minidrv command
  result = HCI_CMD(0xfc2e, NULL, 0, NULL, 0, 500);
  if(result<0) return -1;

  //50 msec delay after Download Minidrv completes 
  Sleep(100);

  //-----------------------------------------------------
  for(;;)
  {
	memcpy(&opcode, pBuf, 2); pBuf +=2;
	//opcode = libusb_le16_to_cpu(opcode);
	length = *pBuf++;
  	 
	if(opcode!=0xfc4c || length>255 || length<1)
	{
	  return -1;
	}

	//----
	result = HCI_FWR(opcode, pBuf, length, NULL, 0, 200);
    if(result<0) 
	   return -1;
   
	//----
	fw_length -= (length+3);
	if(fw_length<=0)
	 {state = 1; break;} 
	
	//next
	pBuf +=	length;
	block++;
	/*
	if(block==364)
	{
	  state = 1;
	}*/
  }

  Sleep(100);  
  
  //HCI Broadcom Launch RAM cmd:0xfC4e
  //Address: 0xffffffff
  memset(buf, 0xFF, 4);
  result = HCI_CMD(0xfc4e, buf, 4, NULL, 0, 500);
  if(result<0) return -1;

  WaitEvent(0xFF);

  //250 msec delay after Launch Ram completes
  Sleep(250);

  WUSB.FlushFifo();

  #ifdef BLE_DEBUG
  TRACE1("%s OK\n", __FUNCTION__);
  #endif

  return 1;
}

