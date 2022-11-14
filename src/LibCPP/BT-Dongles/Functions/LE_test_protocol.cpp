 //==============================================================================
//File name:   ".cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "LE_test_protocol.h"

#define CMD 0
#define VAL 1

#pragma pack(push)
#pragma pack(1)
typedef struct cmd 
{
  //тип патерна
  unsigned char length;
  //число байт в патерне
  unsigned char pattern;
  //описатель, команда или величина
  unsigned char type;
  //параметр
  unsigned char output;
} decode;

static const struct cmd table[] =
{
  //----
  {0,0,VAL,0},
  {0,1,VAL,1},
  {0,2,VAL,2},
  {0,3,VAL,3},
  {0,4,VAL,4},
  {0,5,VAL,5},
  {0,6,VAL,6},
  {0,7,VAL,7},
 
  //----
  {1,0,VAL,8},
  {1,1,VAL,9},
  {1,2,CMD,0},
  {1,3,CMD,1},
  {1,4,CMD,2},
  {1,5,CMD,3},
  {1,6,CMD,4},
  {1,7,CMD,5},
/*  
  //---
  {3,1,CMD,5},
  {3,2,CMD,6},
  {3,3,CMD,7},
  {3,4,CMD,8},
  {3,5,CMD,9}, */
};
#pragma pack(pop)

//--------------------------------------------------------------------------------------
//Обработка сообщения
//--------------------------------------------------------------------------------------
int protocol_encoder(char type, unsigned char param, char *pBuf)
{
  int i;
  int length = sizeof(table)/sizeof(cmd);


	  *pBuf++ = 2;
      *pBuf = 0x07;
	   return 1;

  for(i=0; i<length; i++)
  {
  	//поиск совпадения code + arg
	if(table[i].type==type && table[i].output==param)
	{
	  //найдено
	  //0x00-0x25 Length in bytes of payload data in each packet
	  //передача есть с параметром от 0 до 25
	  ///*pBuf = table[i].length;
	  *pBuf++ = 1;

	  //Packet Payload type: 0x00-0x07, 0x08-0xFF Reserved for future use
	  //Broadcom поддерживает полную шкалу, CSR только до 7
      *pBuf = 0x08;
	  
	  return 1;
	}
  }

 return -1;
}
//--------------------------------------------------------------------------------------
//Обработка сообщения
//--------------------------------------------------------------------------------------
int protocol_decoder(char *pBuf, char &type, unsigned char &output)
{
  int i;
  int length = sizeof(table)/sizeof(cmd);

  for(i=0; i<length; i++)
  {
  	//поиск совпадения code + arg
	if(table[i].length==pBuf[0] && table[i].pattern==pBuf[1])
	{
	  //найдено
	  type = table[i].type;
      output = table[i].output;
	  return 1;
	}
  }

 return -1;
}
