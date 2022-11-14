//==============================================================================
//File name:   "parser.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2013, Akimov Vladimir  E-mail: decoder@rambler.ru
//==============================================================================
#ifndef _HCI_PARSER_H_
#define _HCI_PARSER_H_

extern char *hci_error_code_str[];

#ifdef __cplusplus
extern "C"
{
#endif

 void print_buffer(char *pBuffer, int address, int page_size);
 void wr_parser(void *pcmd_data, int cmd_size);
 int  hci_cmd_read_parser(unsigned char *pData, int size);
 int  hci_acl_read_parser(unsigned char *pData, int size);

 void Connection_Completed_0x03(unsigned char *pData, int size);
 void BLE_Connection_Completed_0x3E(unsigned char *pData, int size);
 void Connection_Request_0x04(unsigned char *pData, int size);
 void VendorIRQ_Connection_Request(unsigned char *pData, int size);
 void VendorIRQ_Lost3DSync(void);
 void Disconnection_Completed_0x05(void);
 void Send_Synchronization_Train(void);
 
 //-----
 void LE_AdvertisingReport(unsigned char *pBuf, int length);

#ifdef __cplusplus
}
#endif

#endif //_FILES_H_
