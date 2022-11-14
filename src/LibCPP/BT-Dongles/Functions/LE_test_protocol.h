//==============================================================================
//File name:   "protocol.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _BT_LE_TEST_PROTOCOL_H_
#define _BT_LE_TEST_PROTOCOL_H_


#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

int protocol_encoder(char type, unsigned char param, char *pBuf);
int protocol_decoder(char *pBuf, char &type, unsigned char &output);

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif