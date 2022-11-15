//==============================================================================
//File name:   "bt-send.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:   (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dbt.h>
#include "parser.h"
#include "bt-dongle.h"

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

/*
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CBTDONGLE::SendData(char *pBuffer, int length)
{ 
  //if no device
  if(dev_opened==0) return -1;
  int result = 1;
  char acl_buf[64];
  
  int l2cap_length = 11;

  //------------------------------------------
  //Bluetooth L2CAP Packet layer
  //------------------------------------------
  // 0 - Connection handle : PB=00 : BC=00 
  acl_buf[0] = dev_connection_handle;
  acl_buf[1] = dev_connection_handle>>8;
  acl_buf[1] &= 0x0F;
  //!!! тут следует добавить флаги режима!
     
  // 2 - ACL length
  //bt_store_16(acl_buffer, 2,  len + 4);
  int acl_length = l2cap_length + 4;
  acl_buf[2] = (unsigned char)acl_length;
  acl_buf[3] = (unsigned char)acl_length>>8;
    
  // 4 - L2CAP packet length
  //bt_store_16(acl_buffer, 4,  len + 0);
  acl_buf[4] = (unsigned char)l2cap_length;
  acl_buf[5] = (unsigned char)l2cap_length>>8;
  
  // 6 - L2CAP channel CID DEST
  int acl_cid = 0x0004;
  acl_buf[6] = (unsigned char)acl_cid;
  acl_buf[7] = (unsigned char)acl_cid>>8;
  
  //------------------------------------------
  //Bluetooth ACL Packet layer
  //------------------------------------------
  acl_buf[8] =  0x12; //? 
  acl_buf[9] =  0x0e; //?
  acl_buf[10] = 0x00; //?

  //data payload
  memcpy(&acl_buf[11], pBuffer, length);  
  result = HCI_DATA(acl_buf, 11+length);
  
  return result;
} */


	/*
    uint8_t *acl_buffer = hci_get_outgoing_packet_buffer();

    // 0 - Connection handle : PB=00 : BC=00 
    bt_store_16(acl_buffer, 0, handle | (0 << 12) | (0 << 14));
    // 2 - ACL length
    bt_store_16(acl_buffer, 2,  len + 4);
    // 4 - L2CAP packet length
    bt_store_16(acl_buffer, 4,  len + 0);
    // 6 - L2CAP channel CID DEST
    bt_store_16(acl_buffer, 6, cid);    
    // send
    int err = hci_send_acl_packet_buffer(len+8);
	 */
  //								   - operation code 
  //handle					 канал	  |    length
  //40 00 |  0f 00 | 0b 00 | 04 00 | 12 0e | 00 -> 0f ac 03 41 1a fc ff ea
  //40 00    0f 00   0b 00   04 00   12 0e   00    0f ac 03 41 1a fe ff e8
  //40 00    07 00   03 00   04 00   0a 07   00
  //40 00    07 00   03 00   04 00   02 0f   02
  //40 00    1b 00   17 00   04 00   06 01   00    ff ff 00 28 9e ca dc 24 0e e5 a9 e0 93 f3 a3 b5 01 00 40 6e

  //40 00    0b 00   07 00   04 00   08 07   00    07 00 03 28
  //40 00    1b 00   17 00   04 00   06 01   00    ff ff 00 28 9e ca dc 24 0e e5 a9 e0 93 f3 a3 b5 01 00 40 6e
  //40 00    0f 00   0b 00   04 00   12 0e   00 -> 0f ac 01 43 07 f8  1 байт послали команду устройтву


  //char acl[] = {0x40, 0x00, 0x0f, 0x00, 0x0b, 0x00, 0x04, 0x00, 0x12, 0x0e, 0x00};
  //memcpy(acl_buf, acl, 11);
  //memcpy(&acl_buf[11], pBuffer, 11+length);
  
  ///TRACE("Send > "); 
  //print_buffer(acl_buf, 0, 11+length);

#define LLID   6
#define NESN   5
#define SN	   4
#define MD	   3
#define RFU    0   //RESERVED FOR FUTURE USE

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
int CBTDONGLE::SendSync_CMD(char *pBuffer, int length)
{  
  
  //PDU Header: PROTOCOL DATA UNIT FORMAT	
  //1 byte  - PDU ID 
  //2 bytes - Transaction ID 
  //2 bytes - ParameterLength	
	
  //------------------------------------------
  //Bluetooth L2CAP Packet layer
  //HCI Synchronous Data Packets
  //BLUETOOTH SPECIFICATION Version 4.2 [Vol 2, Part E] page 475
  //------------------------------------------
  // 0 - Connection handle 12-bit: PB=00 : BC=00 
  pSyncBuf[0] = dev_connection_handle;
  pSyncBuf[1] = dev_connection_handle>>8;
  pSyncBuf[1] &= 0x0F;
  //!!! тут следует добавить флаги режима!
  //Packet_Status_Flag bits consist of two bits


  //------------------------------------------
  //Data Channel PDU Header 16 bit	LL Data PDU
  //BLUETOOTH SPECIFICATION Version 4.0 [Vol 6] page 44 of 138
  //BLUETOOTH SPECIFICATION Version 4.2 [Vol 6, Part B] page 46
  //------------------------------------------
  //LLID (2 bits) indicates whether the packet is an LL Data PDU or an LL Control PDU
  //00b = Reserved
  //01b = LL Data PDU: Continuation fragment of an L2CAP message, or an Empty PDU
  //10b = LL Data PDU: Start of an L2CAP message or a complete L2CAP message with no fragmentation
  //11b = LL Control PDU
  //для отправки данных L2CAP. Поле LLID в заголовке PDU канала данных LL 
  //имеет значение либо 01b или 10b. PDU данных LL называется пустым PDU, если
  //Поле LLID заголовка PDU канала данных LL имеет значение 01b.
  //Поле Length заголовка PDU канала данных LL имеет значение 00000000b.
  //С LLID установленным в 10b значение, не имеет поля Length, установленного в 00000000b значение
  //https://www.mathworks.com/help/comm/ug/bluetooth-packet-structure.html

  //----
  //NESN (1 bit) Next Expected Sequence Number
  
  //----
  //SN   (1 bit)Sequence Number
  
  //----
  //MD   (1 bit) 0 = No More Data
  
  //RESERVED FOR FUTURE USE (RFU)

  //L2CAP packet length
  //opcode=1 + AttributeHandle=2 + data length
  unsigned char l2cap_length = 3 + length;

  //----  - 0F - 
  //Length or RFU  (3 bits)
  //v4.0 Length (5 bits)RFU(3 bits), v4.2 Length (8 bits)
  unsigned char pdu_length = l2cap_length + 4;
  pSyncBuf[2] = pdu_length;
  
  //----
  unsigned char pdu_header;
  pdu_header = (0<<LLID)|(0<<NESN)|(0<<SN)|(0<<MD)|(0<<RFU);
  pSyncBuf[3] = pdu_header;
  
  //------------------------------------------
  //L2CAP Header 
  //------------------------------------------
  // 4 - L2CAP packet length  // 0B 00 
  //opcode=1 + AttributeHandle=2 + data length
  //l2cap_length = 3 + length;
  pSyncBuf[4] = (unsigned char)l2cap_length;
  pSyncBuf[5] = (unsigned char)l2cap_length>>8;
  
  // 6 - L2CAP channel CID DEST	ID
  int acl_cid = 0x0004;		 // 04 00
  pSyncBuf[6] = (unsigned char)acl_cid;
  pSyncBuf[7] = (unsigned char)acl_cid>>8;
  
  // 1A 2F 4B 65 50 0A 0F - 0B 00 - 04 00 - 12 0E 00 - 0F AC 03 41 1A 01 00 E4
  //BLUETOOTH SPECIFICATION Version 4.0 [Vol 3] page 502 of 656
  unsigned char  opcode = 0x12; //Opcode (0x12) — Write Request opcode
  pSyncBuf[8] =  opcode;        //Opcode
  //Write Without Response 0x12; 

  //Handle of an attribute, which stores a value of Device Config Characteristic
  //unsigned short attr_handle = 0x000e;
  pSyncBuf[9] =  0x0e;          //Attribute Handle 
  pSyncBuf[10] = 0x00;          //2 octets

  //data payload
  memcpy(&pSyncBuf[11], pBuffer, length);
  	  
  //Send a message to the connected BT device
  sync_length = 11 + length;
  ev_SYNC.PulseEvent();

  return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
int CBTDONGLE::SendSync(void)
{
  //PDU Header: PROTOCOL DATA UNIT FORMAT	
  //1 byte  - PDU ID 
  //2 bytes - Transaction ID 
  //2 bytes - ParameterLength	
	
  //------------------------------------------
  //Bluetooth L2CAP Packet layer
  //HCI Synchronous Data Packets
  //BLUETOOTH SPECIFICATION Version 4.2 [Vol 2, Part E] page 475
  //------------------------------------------
  // 0 - Connection handle 12-bit: PB=00 : BC=00 
  pSyncBuf[0] = dev_connection_handle;
  pSyncBuf[1] = dev_connection_handle>>8;
  pSyncBuf[1] &= 0x0F;
  //!!! тут следует добавить флаги режима!
  //Packet_Status_Flag bits consist of two bits


  //------------------------------------------
  //Data Channel PDU Header 16 bit	LL Data PDU
  //BLUETOOTH SPECIFICATION Version 4.0 [Vol 6] page 44 of 138
  //BLUETOOTH SPECIFICATION Version 4.2 [Vol 6, Part B] page 46
  //------------------------------------------
  //LLID (2 bits) indicates whether the packet is an LL Data PDU or an LL Control PDU
  //00b = Reserved
  //01b = LL Data PDU: Continuation fragment of an L2CAP message, or an Empty PDU
  //10b = LL Data PDU: Start of an L2CAP message or a complete L2CAP message with no fragmentation
  //11b = LL Control PDU
  //для отправки данных L2CAP. Поле LLID в заголовке PDU канала данных LL 
  //имеет значение либо 01b или 10b. PDU данных LL называется пустым PDU, если
  //Поле LLID заголовка PDU канала данных LL имеет значение 01b.
  //Поле Length заголовка PDU канала данных LL имеет значение 00000000b.
  //С LLID установленным в 10b значение, не имеет поля Length, установленного в 00000000b значение
  //https://www.mathworks.com/help/comm/ug/bluetooth-packet-structure.html

  //----
  //NESN (1 bit) Next Expected Sequence Number
  
  //----
  //SN   (1 bit)Sequence Number
  
  //----
  //MD   (1 bit) 0 = No More Data
  
  //RESERVED FOR FUTURE USE (RFU)

  //L2CAP packet length
  //opcode=1 + AttributeHandle=2 + data length
  unsigned char l2cap_length = 3;

  //----  - 0F - 
  //Length or RFU  (3 bits)
  //v4.0 Length (5 bits)RFU(3 bits), v4.2 Length (8 bits)
  unsigned char pdu_length = l2cap_length + 4;
  pSyncBuf[2] = pdu_length;
  
  //----
  unsigned char pdu_header;
  pdu_header = (0<<LLID)|(0<<NESN)|(0<<SN)|(0<<MD)|(0<<RFU);
  pSyncBuf[3] = pdu_header;
  
  //------------------------------------------
  //L2CAP Header 
  //------------------------------------------
  // 4 - L2CAP packet length  // 0B 00 
  //opcode=1 + AttributeHandle=2 + data length
  //l2cap_length = 3 + length;
  pSyncBuf[4] = (unsigned char)l2cap_length;
  pSyncBuf[5] = (unsigned char)l2cap_length>>8;
  
  // 6 - L2CAP channel CID DEST	ID
  int acl_cid = 0x0004;		 // 04 00
  pSyncBuf[6] = (unsigned char)acl_cid;
  pSyncBuf[7] = (unsigned char)acl_cid>>8;
  
  // 1A 2F 4B 65 50 0A 0F - 0B 00 - 04 00 - 12 0E 00 - 0F AC 03 41 1A 01 00 E4
  //BLUETOOTH SPECIFICATION Version 4.0 [Vol 3] page 502 of 656
  unsigned char  opcode = 0x52; //Opcode (0x12) — Write Request opcode
  pSyncBuf[8] =  opcode;        //Opcode
  //Write Without Response 0x12; 

  //Handle of an attribute, which stores a value of Device Config Characteristic
  //unsigned short attr_handle = 0x000e;
  pSyncBuf[9] =  0x13;          //Attribute Handle 	0x0e
  pSyncBuf[10] = 0x00;          //2 octets

  //data payload = 0
  //Send a message to the connected BT device
  sync_length = 11;
  ev_SYNC.PulseEvent();

  return 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CBTDONGLE::SendData(char *pBuffer, int length)
{ 
  //if no device
  if(dev_connected==0) return -1;
  int result = 1;
  char acl_buf[64] = {0};
  
  //------------------------------------------
  //Bluetooth L2CAP Packet layer
  //------------------------------------------
  // 0 - Connection handle : PB=00 : BC=00 
  acl_buf[0] = dev_connection_handle;
  acl_buf[1] = dev_connection_handle>>8;
  acl_buf[1] &= 0x0F;
  //!!! тут следует добавить флаги режима!
     
  // 2 - ACL length
  //bt_store_16(acl_buffer, 2,  len + 4);
  int l2cap_length = 3 + length;
  unsigned char pdu_length = l2cap_length + 4;
  acl_buf[2] = pdu_length;

  //----
  unsigned char pdu_header;
  pdu_header = (0<<LLID)|(0<<NESN)|(0<<SN)|(0<<MD)|(0<<RFU);
  acl_buf[3] = pdu_header;
    
  // 4 - L2CAP packet length
  //bt_store_16(acl_buffer, 4,  len + 0);
  acl_buf[4] = (unsigned char)l2cap_length;
  acl_buf[5] = (unsigned char)l2cap_length>>8;
  
  // 6 - L2CAP channel CID DEST
  int acl_cid = 0x0004;
  acl_buf[6] = (unsigned char)acl_cid;
  acl_buf[7] = (unsigned char)acl_cid>>8;
  
  //------------------------------------------
  //Bluetooth ACL Packet layer
  //------------------------------------------
  acl_buf[8] =  0x12; //? 
  acl_buf[9] =  0x0e; //?
  acl_buf[10] = 0x00; //?

  //data payload
  memcpy(&acl_buf[11], pBuffer, length);  
  result = HCI_DATA(acl_buf, 11+length);
  
  return result-11;
} 
