//==============================================================================
//File name:   "bt-ble-ini.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:   (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dbt.h>
#include "bt.h"
#include "bt-utils.h"
#include "bt-dongle.h"
#include "parser.h"

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

//---- If need Debug
#define  BLE_DEBUG
#define  CMD_TIMEOUT 200

  /*
  //тут запускать потоковый прием из 
  int rx_accepted;
  for(;;)
  {
  	
	Sleep(200);
	memset(buf, 0, sizeof(buf));
    //выгрести из контроллера все сообщения из канала 
    //Read answer packet from dongle
    libusb_interrupt_transfer(hUSB, EP1_IN,
								   (unsigned char*)buf, 
									64, &rx_accepted, 100);
	//----
	if(rx_accepted>0)
	{
	   //TRACE1(" > Device Event: 0x%02X OK\n", id);
	   //print_buffer((char*)buf, 0, rx_accepted);
	  TRACE(" > Device Event: OK\n");
	} 
	  
	libusb_interrupt_transfer(hUSB, EP2_IN,
								   (unsigned char*)buf, 
									64, &rx_accepted, 100);
	//----
	if(rx_accepted>0)
	{
	   //TRACE1(" > Device Event: 0x%02X OK\n", id);
	   //print_buffer((char*)buf, 0, rx_accepted);
	  TRACE(" > Device Event: OK\n");
	} 
  }	 */

//------------------------------------------------------------------------------
//Open by USB device name
//------------------------------------------------------------------------------
int CBTDONGLE::BCM20702_ini(void)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  int length, ret;
  unsigned char buf[256];
  struct bt_hci_rsp_read_bd_addr bt_addr_host;
  struct bt_hci_rsp_read_local_version ver;
  
  //HCI Vendor command: Broadcom Read Verbose Config Version Info
  HCI_CMD(0xfc79, NULL, 0, buf, 7, CMD_TIMEOUT);
  //buf[1] Chip ID: 0x3f

  //HCI_Reset: ret: 0e 04 01 03 0c 00
  HCI_CMD(0x0c03, NULL, 0, NULL, 0, CMD_TIMEOUT);
  //Sleep(1000);

  //HCI_Read_BD_ADDR 
  length = sizeof(bt_addr_host);
  HCI_CMD(0x1009, NULL, 0, &bt_addr_host, length, CMD_TIMEOUT);
 
  //========= сохраняем полученный адрес 0x1009
  //5c:f3:70:64:79:ca
  //memcpy(&bt_addr_host.bdaddr, buf, length);
  TRACE("Host BT_ADDR: ");
  memcpy(HOST_BT_ADDR, &bt_addr_host.bdaddr, 6);
  printAddr6((unsigned char*)&bt_addr_host.bdaddr,':'); 
  TRACE(", 0x"); printAddr6((unsigned char*)&bt_addr_host.bdaddr,' '); 
  TRACE("\n");
  //TRACE1("Host BT_ADDR: %012I64X\n\n", bt_addr_host.bdaddr); 
  //====================================================

  //HCI Broadcom Unknown Command 0x0036 (opcode 0xFC36)	
  HCI_CMD(0xFC36, NULL, 0, NULL, 0, CMD_TIMEOUT); //+ совпадает

  //HCI Read Local Supported Commands
  //Local Supported Commands: ffffff03ccffefffffffec1ff20fe8fe3ff78fff1c00040061f7ffff7f00000000000000
  memset(buf, 0, length);
  HCI_CMD(0x1002, NULL, 0, buf, 65, CMD_TIMEOUT);//  !!!! тут 70

  //HCI Read Buffer Size
  struct bt_hci_rsp_read_buffer_size buf_size;
  memset(&buf_size, 0, sizeof(buf_size));
  HCI_CMD(0x1005, NULL, 0, &buf_size, sizeof(buf_size), 500);
   
  TRACE("Read_Buffer_Size:\n");
  TRACE1("Status %02X\n", buf_size.status); //Status: Success (0x00)	 	
  TRACE1("HC ACL Data Packet Length %d\n", buf_size.acl_mtu);
  TRACE1("HC Synchronous_Data_Packet Length %d\n", buf_size.sco_mtu);
  TRACE1("HC Total_Num ACL Data_Packets %d\n", buf_size.acl_max_pkt);
  TRACE1("HC Total Num Synchronous Data Packets %d\n", buf_size.sco_max_pkt);

  //HCI Read_Local_Version_Information
  length = sizeof(bt_hci_rsp_read_local_version);
  memset(&ver, 0, length);
  HCI_CMD(0x1001, NULL, 0, &ver, length, 500);
  
  TRACE("Local_Version_Information:\n");
  TRACE1("hci_status=%02X\n", ver.status);  //Status: Success (0x00)
  TRACE1("hci_ver=%02X\n", ver.hci_ver);    //HCI Version: 4.1 (0x07)
  TRACE1("hci_rev=%d\n", ver.hci_rev);      //HCI Revision: 5860
  TRACE1("lmp_ver=%02X\n", ver.lmp_ver);    //LMP Version: 4.1 (0x07)
  TRACE1("lmp_subver=%d\n", ver.lmp_subver);//LMP Subversion: 8718
  TRACE1("hci_manufacturer=%04X\n", ver.manufacturer);//Manufacturer Name: Broadcom Corporation (0x000f)
 
  //HCI Broadcom Unknown Command 0x0034 (opcode 0xFC34)
  buf[0] = 0x01;
  HCI_CMD(0xFC34, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Read Local Supported Features	 0x1003
  struct bt_hci_rsp_read_local_features features;
  length = sizeof(features);
  HCI_CMD(0x1003, NULL, 0, &features, length, CMD_TIMEOUT);

  //HCI Broadcom Unknown Command 0x0178 (opcode 0xFD78)
  buf[0] = 0x00;
  HCI_CMD(0xFD78, buf, 1, buf, 12, CMD_TIMEOUT); 

  //HCI Write Simple Pairing Mode
  buf[0] = 1; //Simple Pairing Mode: enabled (1)
  HCI_CMD(0x0C56, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Write Authentication Enable
  buf[0] = 0; //Authentication disabled (0x00)
  HCI_CMD(0x0C20, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Set Event Mask
  //long long mask;
  //SetEventMask(mask);
  buf[0] = 0xff; buf[1] = 0xff; buf[2] = 0xff; buf[3] = 0xff;
  buf[4] = 0xff; buf[5] = 0xff; buf[6] = 0xbf; buf[7] = 0x3d;
  HCI_CMD(0x0C01, buf, 8, NULL, 0, CMD_TIMEOUT);
 
  //HCI Read Inquiry Response Tx Power Level
  struct bt_hci_rsp_read_inquiry_resp_tx_power tx_power;
  length = sizeof(tx_power);
  HCI_CMD(0x0C58, NULL, 0, &tx_power, length, CMD_TIMEOUT);
  
  //HCI Write Page Timeout
  buf[0] = 0x00; buf[1] = 0x20; //Timeout: 8192 slots (5120 msec) 
  HCI_CMD(0x0C18, buf, 2, NULL, 0, CMD_TIMEOUT);

  //HCI Write Page Scan Activity
  buf[0] = 0x00; buf[1] = 0x04; //Interval: 1024 slots (640 msec)
  buf[2] = 0x12; buf[3] = 0x00; //Window: 18 slots (11,25 msec)
  HCI_CMD(0x0C1C, buf, 4, NULL, 0, CMD_TIMEOUT);

  //HCI Write Page Scan Type
  buf[0] = 1;  //Scan Type: Interlaced Scan (1)
  HCI_CMD(0x0C47, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Write Inquiry Scan Activity
  buf[0] = 0x00; buf[1] = 0x08; //Interval: 2048 slots (1280 msec)
  buf[2] = 0x12; buf[3] = 0x00; //Window: 18 slots (11,25 msec)
  HCI_CMD(0x0C1E, buf, 4, NULL, 0, CMD_TIMEOUT);

  //HCI Write Inquiry Scan Type
  buf[0] = 1; //Scan Type: Interlaced Scan (1)
  HCI_CMD(0x0C43, buf, 1, NULL, 0, CMD_TIMEOUT);
  
  //HCI Write Inquiry Mode
  buf[0] = 2; //Results With RSSI or Extended Results
  HCI_CMD(0x0C45, buf, 1, NULL, 0, CMD_TIMEOUT); 
  
  //HCI Write Class of Device
  //Computer:Desktop workstation - services
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x00;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI Sent Host Buffer Size
  buf[0] = 0xFC; buf[1] = 0x0F;	//Host ACL Data Packet Length (bytes): 4092
  buf[2] = 0xFF;                //Host SCO Data Packet Length (bytes): 255
  buf[3] = 0xFF; buf[4] = 0xFF; //Host Total Num ACL Data Packets: 65535
  buf[5] = 0xFF; buf[6] = 0xFF; //Host Total Num SCO Data Packets: 65535
  HCI_CMD(0x0C33, buf, 7, NULL, 0, CMD_TIMEOUT);

  //HCI Write Local Name  0x0C13	Bluetooth HCI Command - Change Local Name
  memset(buf, 0, 256);
  memcpy(buf,"HOMEWORK", 8); //Device Name: HOMEWORK
  HCI_CMD(0x0C13, buf, 248, NULL, 0, CMD_TIMEOUT);
  
  //HCI LE Read Local Supported Features  ?
  struct bt_hci_rsp_le_read_local_features le_features;
  length = sizeof(le_features);
  HCI_CMD(0x2003, NULL, 0, &le_features, length, CMD_TIMEOUT);
  
  //HCI LE Read Supported States
  struct bt_hci_rsp_le_read_supported_states supp_st;
  length = sizeof(supp_st);
  HCI_CMD(0x201C, NULL, 0, &supp_st, length, CMD_TIMEOUT);

  //HCI LE Read Buffer Size
  struct bt_hci_rsp_le_read_buffer_size	le_buf_size;
  length = sizeof(le_buf_size);
  HCI_CMD(0x2002, NULL, 0, &le_buf_size, length, CMD_TIMEOUT);

  //HCI LE Read White List Size
  struct bt_hci_rsp_le_read_white_list_size white_list_size;
  length = sizeof(white_list_size);
  HCI_CMD(0x200F, NULL, 0, &white_list_size, length, CMD_TIMEOUT);

  //HCI Sent Write LE Host Supported
  buf[0] = 0x01; //LE Supported Host: true (0x01)
  buf[1] = 0x01; //Simultaneous LE Host: true (0x01)
  HCI_CMD(0x0C6D, buf, 2, NULL, 0, CMD_TIMEOUT);

  //HCI LE Read Advertising Channel Tx Power
  struct bt_hci_rsp_le_read_adv_tx_power le_tx_power;
  length = sizeof(white_list_size);
  HCI_CMD(0x2007, NULL, 0, &le_tx_power, length, CMD_TIMEOUT);

  //HCI LE Set Event Mask
  memset(buf, 0, 256);
  buf[0] = 0x1F; buf[1] = 0; buf[2] = 0; buf[3] = 0;
  buf[4] = 0; buf[5] = 0; buf[6] = 0; buf[7] = 0;  
  HCI_CMD(0x2001, buf, 8, NULL, 0, CMD_TIMEOUT);

  //HCI Write Scan Enable !!!!!!!! тут должно быть 2 байта
  buf[0] = 0x0; //Scan Enable: No Scans enabled (0x00)
  HCI_CMD(0x0C1A, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Broadcom Unknown Command 0x0178 (opcode 0xFD78)
  buf[0] = 0x05; buf[1] = 0x01;
  HCI_CMD(0xFD78, buf, 2, buf, 2, CMD_TIMEOUT);
  //buf[1] = 5	совпадает

  //HCI LE Set Scan Parameters
  buf[0] = 0x00;                //Scan Type: Passive (0x00)
  buf[1] = 0xBD; buf[2] = 0x00; //Scan Interval: 189 (118,125 msec)
  buf[3] = 0x1D; buf[4] = 0x00; //Scan Window: 29 (18,125 msec)
  buf[5] = 0x00; //Own Address Type: Public Device Address (0x00)
  //Scan Filter Policy: Accept only advertisements from devices in the White List
  //Ignore directed advertisements not addressed to this device (0x01)
  buf[6] = 0x01;  
  HCI_CMD(0x200B, buf, 7, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x05; //Length: 5 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)

  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI Write Class of Device
  //Class of Device: 0x080104 
  //(Computer:Desktop workstation - services: Capturing)
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x08;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x07; //Length: 7 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)
  buf[20] = 0x0e; //UUID 16: A/V Remote Control (0x110e)
  buf[21] = 0x11; //UUID 16: A/V Remote Control (0x110e)

  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI Write Class of Device
  //Class of Device: 0x0a0104 
  //(Computer:Desktop workstation - services: Networking Capturing)
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x0A;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x09; //Length: 9 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  //----
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)
  //----
  buf[20] = 0x0e; //UUID 16: A/V Remote Control (0x110e)
  buf[21] = 0x11; //UUID 16: A/V Remote Control (0x110e)
  //----
  buf[22] = 0x1f; //UUID 16: Handsfree Audio Gateway (0x111f)
  buf[23] = 0x11; //UUID 16: Handsfree Audio Gateway (0x111f)

  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Advertise Enable	   
  buf[0] = 0x0; //Advertising Enable: false (0x00)
  HCI_CMD(0x200A, buf, 1, NULL, 0, CMD_TIMEOUT);
  //Status: Command Disallowed (0x0c) +совпадает
  
  //HCI LE Set Random Address
  //BD_ADDR: 39:ab:ba:39:9c:64 (39:ab:ba:39:9c:64)
  buf[0] = 0x39; buf[1] =  0xab; buf[2] =  0xba; 
  buf[3] = 0x39; buf[4] =  0x9c; buf[5] =  0x64; 
  HCI_CMD(0x2005, buf, 6, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Advertising Data
  memset(buf, 0, 256);
  buf[0] = 0x1f; //Advertising_Data_Length Packet: 
  buf[1] = 0x1e; //Data Length: 30
  buf[2] = 0xff; //Type: Manufacturer Specific (0xff)
  //----
  buf[3] = 0x06; //Company ID: Microsoft (0x0006)
  buf[4] = 0x00; //Company ID: Microsoft (0x0006)
  //---- Advertising Data
  buf[5] = 0x01;  buf[6] = 0x09;  buf[7] = 0x20;  buf[8] = 0x02; 
  buf[9] = 0xd9;  buf[10] = 0x73; buf[11] = 0x65; buf[12] = 0x87; 
  buf[13] = 0x9c; buf[14] = 0x87; buf[15] = 0x73; buf[16] = 0x93; 
  buf[17] = 0x97; buf[18] = 0x7d; buf[19] = 0x0f; buf[20] = 0x17; 
  buf[21] = 0xf1; buf[22] = 0x4a; buf[23] = 0xb3; buf[24] = 0x7c; 
  buf[25] = 0x53; buf[26] = 0x73; buf[27] = 0xee; buf[28] = 0xd8; 
  buf[29] = 0x6e; buf[30] = 0xb5; buf[31] = 0x16;
  //----
  HCI_CMD(0x2008, buf, 32, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Advertising Parameters
  //Advertising Interval Min: 160 (100 msec)
  buf[0] = 0xA0; buf[1] = 0x00;
  //Advertising Interval Max: 160 (100 msec)
  buf[2] = 0xA0; buf[3] = 0x00;
  //Advertising Type: Non-Connectable Undirected Advertising (0x03)
  buf[4] = 0x03; 
  //Own Address Type: Random Device Address (0x01)
  buf[5] = 0x01;
  //Peer_Address_Type: Public Device Address (default) or Public Identity Address
  buf[6] = 0x00;
  //Peer_Address: 
  //Public Device Address, Random Device Address, Public Identity
  //Address, or Random (static) Identity Address of the device to be connected
  //BD_ADDR: 00:00:00_00:00:00 (00:00:00:00:00:00) 6 bytes
  buf[7]  = 0x00; buf[8]  = 0x00; buf[9]  = 0x00;
  buf[10] = 0x00; buf[11] = 0x00; buf[12] = 0x00;
  //Advertising_Channel_Map: Default (all channels enabled) 0x07
  buf[13] = 0x07;//.... ...1 = Channel 37 38 39: true (0x1)	 
  //Advertising Filter Policy: Process scan and connection requests from all devices
  buf[14] = 0x00; //Allow Scan Req from Any, Allow Connect Req from Any (0x00)
  //----
  HCI_CMD(0x2006, buf, 15, NULL, 0, CMD_TIMEOUT); // 0x2005

  //HCI	LE Set Advertise Enable
  buf[0] = 0x01; //Advertising Enable: true (0x01)
  HCI_CMD(0x200A, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI	Write Scan Enable !!!!!!!! тут должно быть 2 байта
  buf[0] = 0x02; //Scan Enable: Inquiry Scan disabled/Page Scan enabled (0x02)
  HCI_CMD(0x0C1A, buf, 1, NULL, 0, CMD_TIMEOUT); 

  //HCI	Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x0b; //Length: 11 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  //----
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  //----
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)
  //----
  buf[20] = 0x0e; //UUID 16: A/V Remote Control (0x110e)
  buf[21] = 0x11; //UUID 16: A/V Remote Control (0x110e)
  //----
  buf[22] = 0x1f; //UUID 16: Handsfree Audio Gateway (0x111f)
  buf[23] = 0x11; //UUID 16: Handsfree Audio Gateway (0x111f)
  //----
  buf[24] = 0x1e; //UUID 16: Handsfree 
  buf[25] = 0x11; //UUID 16: Handsfree 
  //---
  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI	Write Class of Device
  //Class of Device: 0x2a0104 
  //(Computer:Desktop workstation - services: Networking Capturing Audio)
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x2A;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);
  
  //----------
  //HCI Broadcom Unknown Command 0x0178 (opcode 0xFD78)
  buf[0] = 0x05; buf[1] = 0x00;
  HCI_CMD(0xFD78, buf, 2, buf, 2, CMD_TIMEOUT);
  //buf[1] = 5	ok

  //HCI LE Set Scan Parameters
  buf[0] = 0x01;                //Scan Type: Active (0x01)
  buf[1] = 0x12; buf[2] = 0x00; //Scan Interval: 18 (11,25 msec)
  buf[3] = 0x12; buf[4] = 0x00; //Scan Window: 18 (11,25 msec)
  buf[5] = 0x00; //Own Address Type: Public Device Address (0x00)
  //Scan Filter Policy: 
  //Accept all advertisements, except directed advertisements 
  //not addressed to this device (0x00)
  buf[6] = 0x00;  
  HCI_CMD(0x200B, buf, 7, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Scan Enable
  buf[0] = 0x1; //Scan Enable: true (0x01)
  buf[1] = 0x0; //Filter Duplicates: false (0x00)
  HCI_CMD(0x200c, buf, 2, NULL, 0, CMD_TIMEOUT);

/* дебаг
  ev_HCI_CMD.ResetEvent();
  CMD_ThreadLaunch(this);
  //ждем запуск процесса
  long state = WaitForSingleObject(ev_HCI_CMD.m_hObject, 5000);

  //тут запускать потоковый прием из
  for(;;)
  {
    //HCI Read_Local_Version_Information
    length = sizeof(bt_hci_rsp_read_local_version);
    memset(&ver, 0, length);

    ret = HCI_CMD(0x1001, NULL, 0, &ver, length, 500);
  	if(ret)
	{
      TRACE("Local_Version_Information:\n");
      TRACE1("hci_status=%02X\n", ver.status);  //Status: Success (0x00)
      TRACE1("hci_ver=%02X\n", ver.hci_ver);    //HCI Version: 4.1 (0x07)
      TRACE1("hci_rev=%d\n", ver.hci_rev);      //HCI Revision: 5860
      TRACE1("lmp_ver=%02X\n", ver.lmp_ver);    //LMP Version: 4.1 (0x07)
      TRACE1("lmp_subver=%d\n", ver.lmp_subver);//LMP Subversion: 8718
      TRACE1("hci_manufacturer=%04X\n", ver.manufacturer);//Manufacturer Name: Broadcom (0x000f)  	
	}
	Sleep(200);
  }	*/ 

  #ifdef BLE_DEBUG
  TRACE1("%s OK\n", __FUNCTION__);
  #endif

  return 1;
}

//------------------------------------------------------------------------------
//Open by USB device name
//------------------------------------------------------------------------------
int CBTDONGLE::CSR8510A10_ini(void)
{
  #ifdef BLE_DEBUG
  TRACE1("%s\n", __FUNCTION__);
  #endif

  int length, ret;
  unsigned char buf[256];
  struct bt_hci_rsp_read_bd_addr bt_addr_host;
  struct bt_hci_rsp_read_local_version ver;
  
  //HCI_Reset: ret: 0e 04 01 03 0c 00
  HCI_CMD(0x0c03, NULL, 0, NULL, 0, CMD_TIMEOUT);
  Sleep(1000);

  //HCI_Read_BD_ADDR 
  length = sizeof(bt_addr_host);
  HCI_CMD(0x1009, NULL, 0, &bt_addr_host, length, CMD_TIMEOUT);
 
  //========= сохраняем полученный адрес 0x1009
  //5c:f3:70:64:79:ca
  //memcpy(&bt_addr_host.bdaddr, buf, length);
  TRACE("Host BT_ADDR: ");
  memcpy(HOST_BT_ADDR, &bt_addr_host.bdaddr, 6);
  printAddr6((unsigned char*)&bt_addr_host.bdaddr,':'); 
  TRACE(", 0x"); printAddr6((unsigned char*)&bt_addr_host.bdaddr,' '); 
  TRACE("\n");
  //TRACE1("Host BT_ADDR: %012I64X\n\n", bt_addr_host.bdaddr); 
  //====================================================

  //HCI Read Local Supported Commands
  //Local Supported Commands: ffffff03ccffefffffffec1ff20fe8fe3ff78fff1c00040061f7ffff7f00000000000000
  memset(buf, 0, length);
  HCI_CMD(0x1002, NULL, 0, buf, 65, CMD_TIMEOUT);//  !!!! тут 70

  //HCI Read Buffer Size
  struct bt_hci_rsp_read_buffer_size buf_size;
  memset(&buf_size, 0, sizeof(buf_size));
  HCI_CMD(0x1005, NULL, 0, &buf_size, sizeof(buf_size), 500);
   
  TRACE("Read_Buffer_Size:\n");
  TRACE1("Status %02X\n", buf_size.status); //Status: Success (0x00)	 	
  TRACE1("HC ACL Data Packet Length %d\n", buf_size.acl_mtu);
  TRACE1("HC Synchronous_Data_Packet Length %d\n", buf_size.sco_mtu);
  TRACE1("HC Total_Num ACL Data_Packets %d\n", buf_size.acl_max_pkt);
  TRACE1("HC Total Num Synchronous Data Packets %d\n", buf_size.sco_max_pkt);

  //HCI Read_Local_Version_Information
  length = sizeof(bt_hci_rsp_read_local_version);
  memset(&ver, 0, length);
  HCI_CMD(0x1001, NULL, 0, &ver, length, 500);
  
  TRACE("Local_Version_Information:\n");
  TRACE1("hci_status=%02X\n", ver.status);  //Status: Success (0x00)
  TRACE1("hci_ver=%02X\n", ver.hci_ver);    //HCI Version: 4.1 (0x07)
  TRACE1("hci_rev=%d\n", ver.hci_rev);      //HCI Revision: 5860
  TRACE1("lmp_ver=%02X\n", ver.lmp_ver);    //LMP Version: 4.1 (0x07)
  TRACE1("lmp_subver=%d\n", ver.lmp_subver);//LMP Subversion: 8718
  TRACE1("hci_manufacturer=%04X\n", ver.manufacturer);//Manufacturer Name: Broadcom Corporation (0x000f)
 
  //HCI Read Local Supported Features	 0x1003
  struct bt_hci_rsp_read_local_features features;
  length = sizeof(features);
  HCI_CMD(0x1003, NULL, 0, &features, length, CMD_TIMEOUT);

  //HCI Write Simple Pairing Mode
  buf[0] = 1; //Simple Pairing Mode: enabled (1)
  HCI_CMD(0x0C56, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Write Authentication Enable
  buf[0] = 0; //Authentication disabled (0x00)
  HCI_CMD(0x0C20, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Set Event Mask
  //long long mask;
  //SetEventMask(mask);
  buf[0] = 0xff; buf[1] = 0xff; buf[2] = 0xff; buf[3] = 0xff;
  buf[4] = 0xff; buf[5] = 0xff; buf[6] = 0xbf; buf[7] = 0x3d;
  HCI_CMD(0x0C01, buf, 8, NULL, 0, CMD_TIMEOUT);
 
  //HCI Read Inquiry Response Tx Power Level
  struct bt_hci_rsp_read_inquiry_resp_tx_power tx_power;
  length = sizeof(tx_power);
  HCI_CMD(0x0C58, NULL, 0, &tx_power, length, CMD_TIMEOUT);
  
  //HCI Write Page Timeout
  buf[0] = 0x00; buf[1] = 0x20; //Timeout: 8192 slots (5120 msec) 
  HCI_CMD(0x0C18, buf, 2, NULL, 0, CMD_TIMEOUT);

  //HCI Write Page Scan Activity
  buf[0] = 0x00; buf[1] = 0x04; //Interval: 1024 slots (640 msec)
  buf[2] = 0x12; buf[3] = 0x00; //Window: 18 slots (11,25 msec)
  HCI_CMD(0x0C1C, buf, 4, NULL, 0, CMD_TIMEOUT);

  //HCI Write Page Scan Type
  buf[0] = 1;  //Scan Type: Interlaced Scan (1)
  HCI_CMD(0x0C47, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI Write Inquiry Scan Activity
  buf[0] = 0x00; buf[1] = 0x08; //Interval: 2048 slots (1280 msec)
  buf[2] = 0x12; buf[3] = 0x00; //Window: 18 slots (11,25 msec)
  HCI_CMD(0x0C1E, buf, 4, NULL, 0, CMD_TIMEOUT);

  //HCI Write Inquiry Scan Type
  buf[0] = 1; //Scan Type: Interlaced Scan (1)
  HCI_CMD(0x0C43, buf, 1, NULL, 0, CMD_TIMEOUT);
  
  //HCI Write Inquiry Mode
  buf[0] = 2; //Results With RSSI or Extended Results
  HCI_CMD(0x0C45, buf, 1, NULL, 0, CMD_TIMEOUT); 
  
  //HCI Write Class of Device
  //Computer:Desktop workstation - services
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x00;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI Sent Host Buffer Size
  buf[0] = 0xFC; buf[1] = 0x0F;	//Host ACL Data Packet Length (bytes): 4092
  buf[2] = 0xFF;                //Host SCO Data Packet Length (bytes): 255
  buf[3] = 0xFF; buf[4] = 0xFF; //Host Total Num ACL Data Packets: 65535
  buf[5] = 0xFF; buf[6] = 0xFF; //Host Total Num SCO Data Packets: 65535
  HCI_CMD(0x0C33, buf, 7, NULL, 0, CMD_TIMEOUT);

  //HCI Write Local Name  0x0C13	Bluetooth HCI Command - Change Local Name
  memset(buf, 0, 256);
  memcpy(buf,"HOMEWORK", 8); //Device Name: HOMEWORK
  HCI_CMD(0x0C13, buf, 248, NULL, 0, CMD_TIMEOUT);
  
  //HCI LE Read Local Supported Features  ?
  struct bt_hci_rsp_le_read_local_features le_features;
  length = sizeof(le_features);
  HCI_CMD(0x2003, NULL, 0, &le_features, length, CMD_TIMEOUT);
  
  //HCI LE Read Supported States
  struct bt_hci_rsp_le_read_supported_states supp_st;
  length = sizeof(supp_st);
  HCI_CMD(0x201C, NULL, 0, &supp_st, length, CMD_TIMEOUT);

  //HCI LE Read Buffer Size
  struct bt_hci_rsp_le_read_buffer_size	le_buf_size;
  length = sizeof(le_buf_size);
  HCI_CMD(0x2002, NULL, 0, &le_buf_size, length, CMD_TIMEOUT);

  //HCI LE Read White List Size
  struct bt_hci_rsp_le_read_white_list_size white_list_size;
  length = sizeof(white_list_size);
  HCI_CMD(0x200F, NULL, 0, &white_list_size, length, CMD_TIMEOUT);

  //HCI Sent Write LE Host Supported
  buf[0] = 0x01; //LE Supported Host: true (0x01)
  buf[1] = 0x01; //Simultaneous LE Host: true (0x01)
  HCI_CMD(0x0C6D, buf, 2, NULL, 0, CMD_TIMEOUT);

  //HCI LE Read Advertising Channel Tx Power
  struct bt_hci_rsp_le_read_adv_tx_power le_tx_power;
  length = sizeof(white_list_size);
  HCI_CMD(0x2007, NULL, 0, &le_tx_power, length, CMD_TIMEOUT);

  //HCI LE Set Event Mask
  memset(buf, 0, 256);
  buf[0] = 0x1F; buf[1] = 0; buf[2] = 0; buf[3] = 0;
  buf[4] = 0; buf[5] = 0; buf[6] = 0; buf[7] = 0;  
  HCI_CMD(0x2001, buf, 8, NULL, 0, CMD_TIMEOUT);

  //HCI Write Scan Enable !!!!!!!! тут должно быть 2 байта
  buf[0] = 0x0; //Scan Enable: No Scans enabled (0x00)
  HCI_CMD(0x0C1A, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Scan Parameters
  buf[0] = 0x00;                //Scan Type: Passive (0x00)
  buf[1] = 0xBD; buf[2] = 0x00; //Scan Interval: 189 (118,125 msec)
  buf[3] = 0x1D; buf[4] = 0x00; //Scan Window: 29 (18,125 msec)
  buf[5] = 0x00; //Own Address Type: Public Device Address (0x00)
  //Scan Filter Policy: Accept only advertisements from devices in the White List
  //Ignore directed advertisements not addressed to this device (0x01)
  buf[6] = 0x01;  
  HCI_CMD(0x200B, buf, 7, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x05; //Length: 5 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)

  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI Write Class of Device
  //Class of Device: 0x080104 
  //(Computer:Desktop workstation - services: Capturing)
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x08;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x07; //Length: 7 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)
  buf[20] = 0x0e; //UUID 16: A/V Remote Control (0x110e)
  buf[21] = 0x11; //UUID 16: A/V Remote Control (0x110e)

  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI Write Class of Device
  //Class of Device: 0x0a0104 
  //(Computer:Desktop workstation - services: Networking Capturing)
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x0A;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);

  //HCI Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x09; //Length: 9 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  //----
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)
  //----
  buf[20] = 0x0e; //UUID 16: A/V Remote Control (0x110e)
  buf[21] = 0x11; //UUID 16: A/V Remote Control (0x110e)
  //----
  buf[22] = 0x1f; //UUID 16: Handsfree Audio Gateway (0x111f)
  buf[23] = 0x11; //UUID 16: Handsfree Audio Gateway (0x111f)

  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Advertise Enable	   
  buf[0] = 0x0; //Advertising Enable: false (0x00)
  HCI_CMD(0x200A, buf, 1, NULL, 0, CMD_TIMEOUT);
  //Status: Command Disallowed (0x0c) +совпадает
  
  //HCI LE Set Random Address
  //BD_ADDR: 39:ab:ba:39:9c:64 (39:ab:ba:39:9c:64)
  buf[0] = 0x39; buf[1] =  0xab; buf[2] =  0xba; 
  buf[3] = 0x39; buf[4] =  0x9c; buf[5] =  0x64; 
  HCI_CMD(0x2005, buf, 6, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Advertising Data
  memset(buf, 0, 256);
  buf[0] = 0x1f; //Advertising_Data_Length Packet: 
  buf[1] = 0x1e; //Data Length: 30
  buf[2] = 0xff; //Type: Manufacturer Specific (0xff)
  //----
  buf[3] = 0x06; //Company ID: Microsoft (0x0006)
  buf[4] = 0x00; //Company ID: Microsoft (0x0006)
  //---- Advertising Data
  buf[5] = 0x01;  buf[6] = 0x09;  buf[7] = 0x20;  buf[8] = 0x02; 
  buf[9] = 0xd9;  buf[10] = 0x73; buf[11] = 0x65; buf[12] = 0x87; 
  buf[13] = 0x9c; buf[14] = 0x87; buf[15] = 0x73; buf[16] = 0x93; 
  buf[17] = 0x97; buf[18] = 0x7d; buf[19] = 0x0f; buf[20] = 0x17; 
  buf[21] = 0xf1; buf[22] = 0x4a; buf[23] = 0xb3; buf[24] = 0x7c; 
  buf[25] = 0x53; buf[26] = 0x73; buf[27] = 0xee; buf[28] = 0xd8; 
  buf[29] = 0x6e; buf[30] = 0xb5; buf[31] = 0x16;
  //----
  HCI_CMD(0x2008, buf, 32, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Advertising Parameters
  //Advertising Interval Min: 160 (100 msec)
  buf[0] = 0xA0; buf[1] = 0x00;
  //Advertising Interval Max: 160 (100 msec)
  buf[2] = 0xA0; buf[3] = 0x00;
  //Advertising Type: Non-Connectable Undirected Advertising (0x03)
  buf[4] = 0x03; 
  //Own Address Type: Random Device Address (0x01)
  buf[5] = 0x01;
  //Peer_Address_Type: Public Device Address (default) or Public Identity Address
  buf[6] = 0x00;
  //Peer_Address: 
  //Public Device Address, Random Device Address, Public Identity
  //Address, or Random (static) Identity Address of the device to be connected
  //BD_ADDR: 00:00:00_00:00:00 (00:00:00:00:00:00) 6 bytes
  buf[7]  = 0x00; buf[8]  = 0x00; buf[9]  = 0x00;
  buf[10] = 0x00; buf[11] = 0x00; buf[12] = 0x00;
  //Advertising_Channel_Map: Default (all channels enabled) 0x07
  buf[13] = 0x07;//.... ...1 = Channel 37 38 39: true (0x1)	 
  //Advertising Filter Policy: Process scan and connection requests from all devices
  buf[14] = 0x00; //Allow Scan Req from Any, Allow Connect Req from Any (0x00)
  //----
  HCI_CMD(0x2006, buf, 15, NULL, 0, CMD_TIMEOUT); // 0x2005

  //HCI	LE Set Advertise Enable
  buf[0] = 0x01; //Advertising Enable: true (0x01)
  HCI_CMD(0x200A, buf, 1, NULL, 0, CMD_TIMEOUT);

  //HCI	Write Scan Enable !!!!!!!! тут должно быть 2 байта
  buf[0] = 0x02; //Scan Enable: Inquiry Scan disabled/Page Scan enabled (0x02)
  HCI_CMD(0x0C1A, buf, 1, NULL, 0, CMD_TIMEOUT); 

  //HCI	Write Extended Inquiry Response
  memset(buf, 0, 256);
  buf[0] = 0x01;  //FEC Required: true (1)
  buf[1] = 0x09;  //Length: 9
  buf[2] = 0x09;  //Type: Device Name (0x09)
  buf[3] = 0x48;  //Device Name: HOMEWORK
  buf[4] = 0x4f;  
  buf[5] = 0x4d;  
  buf[6] = 0x45; 
  buf[7] = 0x57;  
  buf[8] = 0x4f;  
  buf[9] = 0x52; 
  buf[10] = 0x4b; 
  
  buf[11] = 0x02; //Length: 2 
  buf[12] = 0x0a; //Type: Tx Power Level (0x0a)
  buf[13] = 0x00; //Power Level (dBm): 0
  
  buf[14] = 0x0b; //Length: 11 
  buf[15] = 0x03; //Type: 16-bit Service Class UUIDs (0x03)
  //----
  buf[16] = 0x0c; //UUID 16: A/V Remote Control Target (0x110c)
  buf[17] = 0x11; //UUID 16: A/V Remote Control Target (0x110c)
  //----
  buf[18] = 0x0a; //UUID 16: Audio Source (0x110a)
  buf[19] = 0x11; //UUID 16: Audio Source (0x110a)
  //----
  buf[20] = 0x0e; //UUID 16: A/V Remote Control (0x110e)
  buf[21] = 0x11; //UUID 16: A/V Remote Control (0x110e)
  //----
  buf[22] = 0x1f; //UUID 16: Handsfree Audio Gateway (0x111f)
  buf[23] = 0x11; //UUID 16: Handsfree Audio Gateway (0x111f)
  //----
  buf[24] = 0x1e; //UUID 16: Handsfree 
  buf[25] = 0x11; //UUID 16: Handsfree 
  //---
  HCI_CMD(0x0C52, buf, 241, NULL, 0, CMD_TIMEOUT);

  //HCI	Write Class of Device
  //Class of Device: 0x2a0104 
  //(Computer:Desktop workstation - services: Networking Capturing Audio)
  buf[0] = 0x04; buf[1] = 0x01;	buf[2] = 0x2A;
  HCI_CMD(0x0C24, buf, 3, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Scan Parameters
  buf[0] = 0x01;                //Scan Type: Active (0x01)
  buf[1] = 0x12; buf[2] = 0x00; //Scan Interval: 18 (11,25 msec)
  buf[3] = 0x12; buf[4] = 0x00; //Scan Window: 18 (11,25 msec)
  buf[5] = 0x00; //Own Address Type: Public Device Address (0x00)
  //Scan Filter Policy: 
  //Accept all advertisements, except directed advertisements 
  //not addressed to this device (0x00)
  buf[6] = 0x00;  
  HCI_CMD(0x200B, buf, 7, NULL, 0, CMD_TIMEOUT);

  //HCI LE Set Scan Enable
  buf[0] = 0x1; //Scan Enable: true (0x01)
  buf[1] = 0x0; //Filter Duplicates: false (0x00)
  HCI_CMD(0x200c, buf, 2, NULL, 0, CMD_TIMEOUT);

/* дебаг
  ev_HCI_CMD.ResetEvent();
  CMD_ThreadLaunch(this);
  //ждем запуск процесса
  long state = WaitForSingleObject(ev_HCI_CMD.m_hObject, 5000);

  //тут запускать потоковый прием из
  for(;;)
  {
    //HCI Read_Local_Version_Information
    length = sizeof(bt_hci_rsp_read_local_version);
    memset(&ver, 0, length);

    ret = HCI_CMD(0x1001, NULL, 0, &ver, length, 500);
  	if(ret)
	{
      TRACE("Local_Version_Information:\n");
      TRACE1("hci_status=%02X\n", ver.status);  //Status: Success (0x00)
      TRACE1("hci_ver=%02X\n", ver.hci_ver);    //HCI Version: 4.1 (0x07)
      TRACE1("hci_rev=%d\n", ver.hci_rev);      //HCI Revision: 5860
      TRACE1("lmp_ver=%02X\n", ver.lmp_ver);    //LMP Version: 4.1 (0x07)
      TRACE1("lmp_subver=%d\n", ver.lmp_subver);//LMP Subversion: 8718
      TRACE1("hci_manufacturer=%04X\n", ver.manufacturer);//Manufacturer Name: Broadcom (0x000f)  	
	}
	Sleep(200);
  }	*/ 

  #ifdef BLE_DEBUG
  TRACE1("%s OK\n", __FUNCTION__);
  #endif

  return 1;
}

//------------------------------------------------------------------------------
//Open by USB device name
//------------------------------------------------------------------------------
int CBTDONGLE::Dongle_BLE_ini(void)
{
  int result = -1;
  
  if(adapter_vid == 0x0A5C && adapter_pid == 0x21E8){result = BCM20702_ini();}   
  if(adapter_vid == 0x0A12 && adapter_pid == 0x0001){result = CSR8510A10_ini();}

  return result;
}

#define LT_ADDR  0x01
//==============================================================================
//это предпоследний дебаг
//==============================================================================
int CBTDONGLE::Dongle_Debug_ini(void)
{
  struct bt_hci_cmd_le_create_conn le_conn;
  struct bt_hci_cmd_le_set_adv_parameters le_param;
  struct bt_hci_cmd_set_slave_broadcast cmd;
  struct bt_hci_rsp_read_bd_addr bt_addr_host;

  int ret, length;
  unsigned long addr; 
 /// unsigned char sspmode = 0x01;
  long long mask;
  unsigned char lt_addr = LT_ADDR;
  unsigned char buf[256];
  unsigned char byte;
  char *pString;

  TRACE("BCM20702 BLE-mode controller starting up..\n");
 
  /////////////////////////////////////////////////////////////////////
  /*
  //HCI Opcode: 0x0C13  "Write_Local_Name" CMD length: 251 bytes
  //CMD Param Name: TVBluetooth
  pString = "Broadcom BCM20702";
  memset(buf, 0, sizeof(buf));
  length = strlen(pString);
  memcpy(buf, pString, length);
  bt_hci_send(hci_dev, BT_HCI_CMD_WRITE_LOCAL_NAME, buf, 248, NULL, NULL, NULL);
  Sleep(20); */
	

  //Reset   
  //HCI_Reset cmd: 0x0C03 ret:0e 04 01 03 0c 00
  HCI_CMD(BT_HCI_CMD_RESET, NULL, 0, NULL, 0, 100);
  Sleep(1000);

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



  memset(&le_param, 0, sizeof(le_param));
  le_param.min_interval	= 0x800;
  le_param.max_interval	= 0x800;
  le_param.channel_map = 0x7; //all
  le_param.type = 0x0;
  le_param.own_addr_type = 0x0;
  le_param.filter_policy = 0x0;

  //HCI_LE_Set_Advertising_Parameters  HCI Opcode: 0x2006
  HCI_CMD(BT_HCI_CMD_LE_SET_ADV_PARAMETERS,
         &le_param, sizeof(le_param), NULL, NULL, NULL);
  Sleep(40);

  //LE_Set_Advertise_Enable  HCI Opcode: 0x200A
  buf[0] = 0x01;
  HCI_CMD(BT_HCI_CMD_LE_SET_ADV_ENABLE,
                       buf, 1, NULL, NULL, NULL); 
  Sleep(40);
  
  buf[0] = 0x01; buf[1] = 0x01;
  //HCI_LE_Set_Scan_Enable HCI Opcode: 0x
  HCI_CMD(BT_HCI_CMD_LE_SET_SCAN_ENABLE,
                       buf, 2, NULL, NULL, NULL);
  Sleep(40);


  //========== пытаемся приконнектиться 
  //Target MAC address d0:fd:00:a3:dd:5c
  //bd_addr target = { {0xd0, 0xfd, 0x00, 0xa3, 0xdd, 0x5c} };
  /*bd_addr target = {{0xe1, 0x5d, 0x48, 0x85, 0xc0, 0xa5}};


  //API needs address reversed 
  reverseArray(target.addr, sizeof(target.addr));

  // BLE settings 	  
  //ble_cmd_gap_connect_direct(&btle_dev, , 60,76,100,0); //40, 60, 100,0);
  app_connection.addr_type = gap_address_type_random;
  app_connection.target = target;
  app_connection.conn_interval_min = 80; //100ms
  app_connection.conn_interval_max = 3200; //1s
  app_connection.timeout = 1000;//10s
  app_connection.latency = 0;//0ms

  // Provide place to store data 
  app_attclient.value.data = value_buffer;
  app_attclient.value.len = 0;

  //Initialize status flags 
  app_connection.state = APP_DEVICE_INIT;
  app_attclient.state = 0;
  app_state = 0;  */
			
  //Astar0000@6565

//UUID type for the Nordic UART Service (vendor specific).
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  

//The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms).
#define APP_ADV_INTERVAL                64                                          
                    

  Sleep(200);
  memset(&le_conn, 0, sizeof(le_conn));
  le_conn.scan_interval = 2048;
  le_conn.scan_window = 1024;
  le_conn.filter_policy = 0;
  le_conn.peer_addr_type = 0;	    

  //собственный адрес	//0x20734a100014;//7802ee078;  //Nordic UART: d0 fd 00 a3 dd 5c	  0xe1, 0x5d, 0x48, 0x85, 0xc0, 0xa5
  //le_conn.peer_addr[0] = 0x20; le_conn.peer_addr[1] = 0x73; le_conn.peer_addr[2] = 0x4a;
  //le_conn.peer_addr[3] = 0x10; le_conn.peer_addr[4] = 0x00; le_conn.peer_addr[5] = 0x14;
  //le_conn.peer_addr[0] = 0xd0; le_conn.peer_addr[1] = 0xfd; le_conn.peer_addr[2] = 0x00;
  //le_conn.peer_addr[3] = 0xa3; le_conn.peer_addr[4] = 0xdd; le_conn.peer_addr[5] = 0x5c;
  //Nordic UART: 0xe1, 0x5d, 0x48, 0x85, 0xc0, 0xa5
  le_conn.peer_addr[0] = 0xe1; le_conn.peer_addr[1] = 0x5d; le_conn.peer_addr[2] = 0x48;
  le_conn.peer_addr[3] = 0x85; le_conn.peer_addr[4] = 0xc0; le_conn.peer_addr[5] = 0xa5;

  le_conn.own_addr_type = 0;  //Public Device Address

   /* Minimum acceptable connection interval (20 ms), 
   Connection interval uses 1.25 ms units. */
  le_conn.min_interval = 7; // 6*1.25 = 7.25
  
  /* Maximum acceptable connection interval (75 ms), 
   Connection interval uses 1.25 ms units. */   
  le_conn.max_interval = 14; // *1.25
  
  //Slave latency
  le_conn.latency = 0;
  
  le_conn.supv_timeout = 1000;
  le_conn.min_ce_length = 120;
  le_conn.max_ce_length = 120;

  //HCI_LE_Create_Connection HCI Opcode: 0x  
  HCI_CMD(BT_HCI_CMD_LE_CREATE_CONN,
         &le_conn, sizeof(le_conn), NULL, NULL, NULL);
  Sleep(200);

/*
  //========= сохраняем полученный адрес 0x1009
  bt_addr_host = 0;
  memcpy(&bt_addr_host, ANSW_DATA, 6); 
  memcpy(BD_ADDR_host, ANSW_DATA, 6); 
  TRACE("Host BT_ADDR: 0x%012I64x\n\n", bt_addr_host);
  //====================================================
*/
  /*
  //HCI Opcode: 0xFC6E  "OPCODE NOT FOUND" CMD length: 3 bytes
  bt_hci_send(hci_dev, 0xFC6E, NULL, 0, NULL, NULL, NULL);
  Sleep(100); */

  return 0;
} 
