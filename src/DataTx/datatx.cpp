//==============================================================================
//File name:    "datatx.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2021, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "datatx.h"
#include "protocol-ex.h"
#include "bt-dongle.h"

static CCriticalSection TX_CriticalSection;

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------  
CDataTX::CDataTX()
{
  pBLE = NULL;
}
 
CDataTX::~CDataTX()
{
  pBLE = NULL;
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------  
void CDataTX::SethBLE(HANDLE hBLE)
{
  pBLE = (CBTDONGLE*) hBLE;
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------  
int CDataTX::Tx(unsigned char cmd, char *pBuf, int length)
{
  TX_CriticalSection.Lock();

  int result = 0;
  PROTOCOL.FreeMem();
  int block_length = PROTOCOL.CalcBlockLength(length);
  char *pBufOut = (char*) new char[block_length];
  PROTOCOL.CreateDataBlock(pBufOut, pBuf, cmd, length);

  //If selected BLE channel 
  //Send a message to the connected BT device
  if(pBLE!=NULL)
     result = pBLE->SendData(pBufOut, block_length); 

  delete [] pBufOut;
  pBufOut = NULL;
   
  TX_CriticalSection.Unlock();

  if(result!=block_length) return -1;
  else return 1;
}
