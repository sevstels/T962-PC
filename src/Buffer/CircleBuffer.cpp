//==============================================================================
//File name:    "CircleBuffer.cpp"
//Purpose:      Source File, Circle Memory Buffer class
//Version:      1.00
//Copyright:    (c) 2011, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "CircleBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------ 
//Constructor 
//------------------------------------------------------------------------------
CCircleBuffer::CCircleBuffer(int nLength, int nExtra)
{
	m_Length = nLength;
	m_Extra = nExtra;
	m_Head = m_Tail = m_Size = 0;

	pBuffer = new unsigned char[m_Length];
}

//------------------------------------------------------------------------------
//Destructor 
//------------------------------------------------------------------------------
CCircleBuffer::~CCircleBuffer()
{
	delete pBuffer;
}

//------------------------------------------------------------------------------
//Function: 
//------------------------------------------------------------------------------
bool CCircleBuffer::ReadByte(unsigned char &byte)
{
  CSingleLock Lock(&m_Lock, TRUE);

  //Exit, if data finished
  if (m_Tail == m_Head)	return FALSE;

  byte = pBuffer[m_Tail++];
  m_Tail %= m_Length;
  
  return TRUE;
}

//------------------------------------------------------------------------------
//Function: 
//------------------------------------------------------------------------------
bool CCircleBuffer::ReadBlock(unsigned char *pRxBuf)
{
  CSingleLock Lock(&m_Lock, TRUE);

  //Exit, if data finished
  if (m_Tail == m_Head)	return FALSE;

  //Calc data size for read
  m_Size = m_Head - m_Tail;
  
  //Copy data to local buffer 
  memcpy(pRxBuf, pBuffer, m_Size);

  //set pointer on new position
  m_Tail = m_Head;

  return TRUE;
}

//------------------------------------------------------------------------------
//Function: 
//------------------------------------------------------------------------------
void CCircleBuffer::WriteByte(unsigned char byte)
{ 
  CSingleLock Lock(&m_Lock, TRUE);

  pBuffer[m_Head++] = byte;
  m_Head %= m_Length;

  //If current buffer space is finished, add next space
  if (m_Head == m_Tail)
  {
    unsigned char *pBuff = new unsigned char[m_Length+m_Extra];
	memcpy(pBuff, pBuffer, m_Head);
	memcpy(pBuff+m_Tail+m_Extra, pBuffer+m_Tail, m_Length-m_Tail);

	m_Length += m_Extra;
	m_Tail += m_Extra;

    delete pBuffer;
	pBuffer = pBuff;
  }
  
  flag_bufferNotEmpty.SetEvent();
}
//------------------------------------------------------------------------------
//Function: Write block
//------------------------------------------------------------------------------
void CCircleBuffer::WriteBlock(unsigned char *pTxBuf, int size)
{
  CSingleLock Lock(&m_Lock, TRUE);

  unsigned char *pBuff = new unsigned char[size];
  memcpy(pBuff, pTxBuf, size);

  m_Head = size;
  m_Tail = 0;
  m_Size = size; 
  
  delete pBuffer;
  pBuffer = pBuff;
 
  flag_bufferNotEmpty.SetEvent();
}
